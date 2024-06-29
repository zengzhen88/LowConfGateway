#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/ip_addr.h"

#include <update.h>
#include <message.h>
#include "esp_timer.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_wifi.h"
/* #include "ble_api.h" */

static void *gPriv = NULL;
static UpdatePrint gPrint;
static LogUpdate gLevel = LogUpdate_Info;


/*************************************************/
/**************LogPrintf**************************/
/*************************************************/
static int UpdateLogPrintf(LogUpdate level,
        const char *file, const char *func,
        int line, const char *format, ...) {
    char logBuf[256];
    va_list args;
    int funcLine        = 0;

    if (level > gLevel) return -1;

    snprintf (logBuf, sizeof(logBuf), "[%s][%s][%d]", file, func, line);
    funcLine = strlen(logBuf);

    /*va_list*/
    va_start(args, format);
    vsnprintf(&logBuf[funcLine], sizeof(logBuf) - funcLine, format, args);
    va_end(args);
    /*va_list*/

    if (gPrint) {
        return gPrint(gPriv, logBuf);
    }

    return -1;
}

#define LogPrintf(level, ...)\
    UpdateLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogUpdate_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogUpdate_Debug, __VA_ARGS__);
#endif

#ifndef ERRP
#define ERRP(con, ret, flag, ...)               \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif

#define UPDATE_UPDATE_USERPASSWORD BIT0

typedef struct {
    int32_t isRunning;
    int32_t retryNum;
    int32_t updateSok;

    char url[128];

    esp_event_loop_handle_t event;
    esp_timer_handle_t timer;
    esp_ota_img_states_t otaState;
    SemaphoreHandle_t binarySemHandle;
    TaskHandle_t pTask;

    UpdateSigSend send;
    UpdateSigRecv recv;
} Update;

//设置日志输出对象，主要看你想输出到哪里，就对应填充回调
int32_t UpdateInitLog(void *priv, UpdatePrint print) {
    gPriv   = priv;
    gPrint  = print;

    return 0;
}

//设置日志输出等级，看需要，调试、错误、宕机等
int32_t UpdateSetLogLevel(LogUpdate level) {
    gLevel  = level;

    return 0;
}

static esp_err_t _http_client_init_cb(esp_http_client_handle_t http_client) {
    esp_err_t err = ESP_OK;
    /* Uncomment to add custom headers to HTTP request */
    // err = esp_http_client_set_header(http_client, "Custom-Header", "Value");
    return err;
}

void UpdateAdvancedOtaTask(void *args) {
    Update *update = (Update *)args;

    while (1) {
        BaseType_t xReturn = pdPASS;

        xReturn = xSemaphoreTake(update->binarySemHandle, portMAX_DELAY);
        if (pdTRUE == xReturn) {
            esp_err_t ota_finish_err = ESP_OK;
            esp_http_client_config_t config = {
                .url = update->url,//CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL,
                /* .cert_pem = (char *)server_cert_pem_start, */
                .timeout_ms = 5000,
                .keep_alive_enable = true,
            };

            esp_https_ota_config_t ota_config = {
                .http_config = &config,
                .http_client_init_cb = _http_client_init_cb, // Register a callback to be invoked after esp_http_client is initialized
            };

            esp_https_ota_handle_t https_ota_handle = NULL;
            esp_err_t err = esp_https_ota_begin(&ota_config, &https_ota_handle);
            ERRP(err != ESP_OK, goto ERR0, 1, "Update ESP HTTPS OTA Begin (%d)failure\n", err);

            esp_app_desc_t app_desc;
            err = esp_https_ota_get_img_desc(https_ota_handle, &app_desc);
            ERRP(err != ESP_OK, goto ERR1, 1, "Update esp_https_ota_get_img_desc failure\n");

            esp_app_desc_t *new_app_info = &app_desc;
            esp_app_desc_t running_app_info;
            const esp_partition_t *running = esp_ota_get_running_partition();
            err = esp_ota_get_partition_description(running, &running_app_info);
            ERRP(err != ESP_OK, goto ERR1, 1, "Update esp_ota_get_partition_description failure\n");

            LogPrintf(LogUpdate_Info, "Update Running firmware version: %s", running_app_info.version);

            err = memcmp(new_app_info->version, running_app_info.version, sizeof(new_app_info->version));
            ERRP(err != ESP_OK, goto ERR1, 1, 
                    "Update Current running version is the same as a new. We will not continue the update.\n");

#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
            /**
             * Secure version check from firmware image header prevents subsequent download and flash write of
             * entire firmware image. However this is optional because it is also taken care in API
             * esp_https_ota_finish at the end of OTA update procedure.
             */
            const uint32_t hw_sec_version = esp_efuse_read_secure_version();
            if (new_app_info->secure_version < hw_sec_version) {
                ESP_LOGW(TAG, "New firmware security version is less than eFuse programmed, %"PRIu32" < %"PRIu32 \n, new_app_info->secure_version, hw_sec_version);
                return ESP_FAIL;
            }
#endif

            while (1) {
                err = esp_https_ota_perform(https_ota_handle);
                if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
                    break;
                }
                // esp_https_ota_perform returns after every read operation which gives user the ability to
                // monitor the status of OTA upgrade by calling esp_https_ota_get_image_len_read, which gives length of image
                // data read so far.
                LogPrintf(LogUpdate_Info, "Update Image bytes read: %d\n", esp_https_ota_get_image_len_read(https_ota_handle));
            }

            esp_err_t status = esp_https_ota_is_complete_data_received(https_ota_handle); 
            ERRP(status != true, goto ERR1, 1, "Update Complete data was not received.\n");

            ota_finish_err = esp_https_ota_finish(https_ota_handle);
            if ((err == ESP_OK) && (ota_finish_err == ESP_OK)) {
                LogPrintf(LogUpdate_Info, "Update ESP_HTTPS_OTA upgrade successful. Rebooting ...\n");
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                esp_restart();
            } else {
                if (ota_finish_err == ESP_ERR_OTA_VALIDATE_FAILED) {
                    LogPrintf(LogUpdate_Info, "Update Image validation failed, image is corrupted\n");
                }
                LogPrintf(LogUpdate_Info, "Update ESP_HTTPS_OTA upgrade failed 0x%x\n", ota_finish_err);
                goto ERR0;
            }

ERR1:
            esp_https_ota_abort(https_ota_handle);
ERR0:
            LogPrintf(LogUpdate_Info, "ESP_HTTPS_OTA upgrade failed\n");

        }
    }
}

typedef enum {
    MYUPDATEERNET_EVENT_START,
} myupdate_event_t;
ESP_EVENT_DEFINE_BASE(MYUPDATE_EVENT);
#define MYUPDATE_EVENT_ETH_ANY_ID (-1)

int32_t UpdateEventRecvHandler(Update *update, ModuleMessage *message) {
    switch (message->attr) {
        case ModuleDataAttr_TriggerRecv:
            {
                if (update->recv) {
                    ModuleMessage message;
                    int32_t length = sizeof(message);
                    int status = update->recv(gPriv, DataAttr_MqttToUpdate, &message, &length, 0);
                    if (!status) {
                        LogPrintf(LogUpdate_Info, "start test recv ModuleDataAttr_Update\n");
                        /*主要是设置Update信号*/
                        switch (message.attr) {
                            case ModuleDataAttr_Update:
                                {
                                    strcpy(update->url, message.update.url);
                                    //发信号到升级线程处理升级流程
                                    BaseType_t xReturn = xSemaphoreGive(update->binarySemHandle);
                                    if (pdTRUE != xReturn) {
                                        LogPrintf(LogUpdate_Info, "Update xSemaphoreGive failure\n");
                                    }

                                    break;
                                }
                            default:break;
                        }
                    }
                }
                break;
            }
        default:break;
    }

    return 0;
}

#define EXAMPLE_ESP_MAXIMUM_RETRY  5
void UpdateEventHandler(void* arg, esp_event_base_t event_base,
        int32_t event_id, void* event_data) {
    Update *update = (Update *)arg;

    LogPrintf(LogUpdate_Info, "eventBase:%s eventId:%d\n", 
            event_base, event_id);

    if (event_base == ESP_HTTPS_OTA_EVENT) {
        switch (event_id) {
            case ESP_HTTPS_OTA_START:
                LogPrintf (LogUpdate_Info, "OTA started");
                break;
            case ESP_HTTPS_OTA_CONNECTED:
                LogPrintf (LogUpdate_Info, "Connected to server");
                break;
            case ESP_HTTPS_OTA_GET_IMG_DESC:
                LogPrintf (LogUpdate_Info, "Reading Image Description");
                break;
            case ESP_HTTPS_OTA_VERIFY_CHIP_ID:
                LogPrintf (LogUpdate_Info, "Verifying chip id of new image: %d", *(esp_chip_id_t *)event_data);
                break;
            case ESP_HTTPS_OTA_DECRYPT_CB:
                LogPrintf (LogUpdate_Info, "Callback to decrypt function");
                break;
            case ESP_HTTPS_OTA_WRITE_FLASH:
                LogPrintf(LogUpdate_Info, "Writing to flash: %d written", *(int *)event_data);
                break;
            case ESP_HTTPS_OTA_UPDATE_BOOT_PARTITION:
                LogPrintf (LogUpdate_Info, "Boot partition updated. Next Partition: %d", *(esp_partition_subtype_t *)event_data);
                break;
            case ESP_HTTPS_OTA_FINISH:
                LogPrintf (LogUpdate_Info, "OTA finish");
                break;
            case ESP_HTTPS_OTA_ABORT:
                LogPrintf (LogUpdate_Info, "OTA abort");
                break;
            case ESP_HTTPS_OTA_MAX:
                {
                    UpdateEventRecvHandler(update, (ModuleMessage *)event_data);
                    break;
                }
            default:break;
        }
    }
    else if (event_base == MYUPDATE_EVENT) {
        switch (event_id) {
            case MYUPDATEERNET_EVENT_START:
                UpdateEventRecvHandler(update, (ModuleMessage *)event_data);
                break;
            default:break;
        }
    }
}

int32_t UpdateTriggerRecv(void *arg) {
    Update *update = (Update *)arg;
    ModuleMessage message;
    message.attr = ModuleDataAttr_TriggerRecv;
    esp_event_post_to(update->event, MYUPDATE_EVENT, 
            MYUPDATEERNET_EVENT_START, &message, sizeof(message), 0);

    return 0;
}

void *UpdateInit(UpdateConfig *config) {
    esp_err_t status        = ESP_FAIL;        
    /* esp_ota_img_states_t ota_state; */
    esp_event_loop_args_t loop_args = {
        .queue_size = CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE,
        .task_name = "update",
        .task_stack_size = ESP_TASKD_EVENT_STACK,
        .task_priority = ESP_TASKD_EVENT_PRIO,
        .task_core_id = 0
    };

    Update *update = (Update *) malloc (sizeof(*update));
    ERRP(NULL == update, return NULL, 1, "malloc Update Instance failure\n");
    memset(update, 0x0, sizeof(*update));

    update->send  = config->send;
    update->recv  = config->recv;

    status = esp_event_loop_create(&loop_args, &update->event);
    ERRP(ESP_OK != status, goto ERR000, 1, 
            "update esp_event_loop_create failure\n");

    status = esp_event_handler_instance_register_with(
            update->event,
            ESP_HTTPS_OTA_EVENT, 
            ESP_EVENT_ANY_ID, UpdateEventHandler, update, NULL);
    ERRP(ESP_OK != status, goto ERR0, 1, 
            "update esp_event_handler_register failure\n");

    status = esp_event_handler_instance_register_with(
            update->event,
            MYUPDATE_EVENT, 
            MYUPDATE_EVENT_ETH_ANY_ID, UpdateEventHandler, update, NULL);
    ERRP(ESP_OK != status, goto ERR00, 1, 
            "update esp_event_handler_register failure\n");

    const esp_partition_t *running = esp_ota_get_running_partition();
    ERRP(NULL == running, goto ERR1, 1, 
            "update esp_ota_get_running_partition failure\n");

    status = esp_ota_get_state_partition(running, &update->otaState);
    if (ESP_OK == status) {
        if (update->otaState == ESP_OTA_IMG_PENDING_VERIFY) {
            if (esp_ota_mark_app_valid_cancel_rollback() == ESP_OK) {
                LogPrintf(LogUpdate_Info, "Update App is valid, rollback cancelled successfully");
            } else {
                LogPrintf(LogUpdate_Info, "Update Failed to cancel rollback");
            }
        }
    }

    //蓝牙使能
    status = esp_wifi_set_ps(WIFI_PS_NONE);
    ERRP(ESP_OK != status, goto ERR3, 1, 
            "Update esp_wifi_set_ps WIFI_PS_NONE failure\n");

    update->binarySemHandle = xSemaphoreCreateBinary();
    ERRP(NULL == update->binarySemHandle, goto ERR4, 1, 
            "Update xSemaphoreCreateBinary failure\n");

    xTaskCreate(&UpdateAdvancedOtaTask, 
            "advanced_ota_example_task", 1024 * 8, update, 5, &update->pTask);

    LogPrintf(LogUpdate_Info, "Update INIT Over\n");

    return update;
ERR4:
ERR3:
/* ERR2: */
ERR1:
ERR0:
ERR00:
ERR000:
    free(update);

    return NULL;
}


