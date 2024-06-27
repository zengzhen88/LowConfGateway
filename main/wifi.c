#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/ip_addr.h"

#include <wifi.h>
#include <message.h>
#include "esp_timer.h"

static void *gPriv = NULL;
static WifiPrint gPrint;
static LogWifi gLevel = LogWifi_Info;


/*************************************************/
/**************LogPrintf**************************/
/*************************************************/



static int WifiLogPrintf(LogWifi level,
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
    WifiLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogWifi_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogWifi_Debug, __VA_ARGS__);
#endif

#ifndef ERRP
#define ERRP(con, ret, flag, ...)               \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif

#define WIFI_UPDATE_USERPASSWORD BIT0

typedef struct {
    int32_t isRunning;
    int32_t retryNum;
    int32_t wifiSok;
    esp_event_handler_instance_t instanceAnyId;
    esp_event_handler_instance_t instanceGotIp;
    esp_event_handler_instance_t instanceGotIpV6;
    wifi_config_t config;
    esp_netif_t *staNetif;
    ModuleMessage message; //保存配置

    esp_timer_handle_t timer;
    /* TaskHandle_t wifiTask; */

    WifiSigSend send;
    WifiSigRecv recv;
} Wifi;

//设置日志输出对象，主要看你想输出到哪里，就对应填充回调
int32_t WifiInitLog(void *priv, WifiPrint print) {
    gPriv   = priv;
    gPrint  = print;

    return 0;
}

//设置日志输出等级，看需要，调试、错误、宕机等
int32_t WifiSetLogLevel(LogWifi level) {
    gLevel  = level;

    return 0;
}

int32_t WifiEventRecvHandler(Wifi *wifi, ModuleMessage *message) {
    /* int32_t status = -1; */

    switch (message->attr) {
        case ModuleDataAttr_TriggerRecv:
            {
                if (wifi->recv) {
                    ModuleMessage message;
                    int32_t length = sizeof(message);
                    int status = wifi->recv(gPriv, DataAttr_MqttToWifi, &message, &length, 0);
                    if (!status) {
                        LogPrintf(LogWifi_Info, "start test recv setWifiConfig\n");
                        /*主要是设置Wifi信号*/
                        switch (message.attr) {
                            case ModuleDataAttr_SetWifiCfg:
                                {
                                    status = 0;
                                    if (strcmp((const char *)wifi->config.sta.ssid, message.setWifiCfg.ssid)
                                            || strcmp((const char *)wifi->config.sta.password, message.setWifiCfg.password)) {
                                        strcpy((char *)wifi->config.sta.ssid, (const char *)message.setWifiCfg.ssid);
                                        strcpy((char *)wifi->config.sta.password, (const char *)message.setWifiCfg.password);
                                        esp_wifi_set_config(WIFI_IF_STA, &wifi->config);
                                    }
                                    if (strcmp(message.setWifiCfg.address, "0.0.0.0")) {
                                        esp_netif_dhcp_status_t dhcpcStatus;
                                        if (ESP_OK == esp_netif_dhcpc_get_status(wifi->staNetif, &dhcpcStatus)) {
                                            if (dhcpcStatus == ESP_NETIF_DHCP_STARTED) {
                                                if (ESP_OK == esp_netif_dhcpc_stop(wifi->staNetif)) {
                                                    esp_netif_ip_info_t info;
                                                    memset(&info, 0x0, sizeof(info));
                                                    ipaddr_aton((const char *)message.setWifiCfg.address, (ip_addr_t *)&info.ip);
                                                    ipaddr_aton((const char *)message.setWifiCfg.netmask, (ip_addr_t *)&info.netmask);
                                                    ipaddr_aton((const char *)message.setWifiCfg.gateway, (ip_addr_t *)&info.gw);
                                                    message.setWifiCfg.address, 
                                                        message.setWifiCfg.netmask, 
                                                        message.setWifiCfg.gateway);
                                                    status = esp_netif_set_ip_info(wifi->staNetif, &info);
                                                }
                                                else {
                                                    LogPrintf(LogWifi_Error, "wifi esp_netif_dhcps_stop failure\n");
                                                    status = -1;
                                                }
                                            }
                                            else {
                                                esp_netif_ip_info_t info;
                                                memset(&info, 0x0, sizeof(info));
                                                ipaddr_aton((const char *)message.setWifiCfg.address, (ip_addr_t *)&info.ip);
                                                ipaddr_aton((const char *)message.setWifiCfg.netmask, (ip_addr_t *)&info.netmask);
                                                ipaddr_aton((const char *)message.setWifiCfg.gateway, (ip_addr_t *)&info.gw);
                                                message.setWifiCfg.address, 
                                                    message.setWifiCfg.netmask, 
                                                    message.setWifiCfg.gateway);
                                                status = esp_netif_set_ip_info(wifi->staNetif, &info);
                                            }
                                        }
                                    }
                                    else {
                                        esp_netif_dhcp_status_t dhcpcStatus;
                                        if (ESP_OK == esp_netif_dhcpc_get_status(wifi->staNetif, &dhcpcStatus)) {
                                            if (dhcpcStatus == ESP_NETIF_DHCP_STOPPED) {
                                                if (ESP_OK != esp_netif_dhcpc_start(wifi->staNetif)) {
                                                    LogPrintf(LogWifi_Error, "wifi esp_netif_dhcps_start failure\n");
                                                    status = -1;
                                                }
                                            }
                                        }
                                    }

                                    if (wifi->send) {
                                        ModuleMessage message;
                                        message.attr        = ModuleDataAttr_Ack;
                                        message.ack.srcAttr = ModuleDataAttr_SetWifiCfg;
                                        message.ack.status  = status;
                                        wifi->send(gPriv, DataAttr_WifiToMqtt, &message, sizeof(message), 0);
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
void WifiEventHandler(void* arg, esp_event_base_t event_base,
        int32_t event_id, void* event_data) {
    Wifi *wifi = (Wifi *)arg;

    LogPrintf(LogWifi_Info, "eventBase:%s eventId:%d\n", 
            event_base, event_id);

    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                {
                    esp_err_t status = esp_wifi_connect();
                    LogPrintf(LogWifi_Info, "wifi esp_wifi_connect retryNum:%" PRIu32 " status:%d\n", wifi->retryNum, status);
                    wifi->wifiSok = 1; //应该Start就表示可以了
                    break;
                }
            case WIFI_EVENT_STA_DISCONNECTED:
                {
                    wifi->wifiSok = 0;
                    if (wifi->retryNum < EXAMPLE_ESP_MAXIMUM_RETRY) {
                        esp_wifi_connect();
                        wifi->retryNum++;
                        LogPrintf(LogWifi_Warning, "retry to connect to "
                                "the AP retryNum:% " PRIu32 "\n", wifi->retryNum);
                    }

                    LogPrintf(LogWifi_Error ,"connect to the AP fail retryNum:%" PRIu32 "\n", wifi->retryNum);
                    break;
                }
            case WIFI_EVENT_MAX:
                {
                    WifiEventRecvHandler(wifi, (ModuleMessage *)event_data);
                    break;
                }
            default:break;
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        LogPrintf(LogWifi_Info, "got ip:" IPSTR "\n", IP2STR(&event->ip_info.ip));
        if (wifi->send) {
            //report
            /*
             * wifi->message.wifiConfig.attr      = ModuleDataAttr_GetWifiConfig;
             * wifi->message.wifiConfig.ip        = event->ip_info.ip.addr;
             * wifi->message.wifiConfig.netmask   = event->ip_info.netmask.addr;
             * wifi->message.wifiConfig.gateway   = event->ip_info.gw.addr;
             */
            /* wifi->send(gPriv, DataAttr_Wifi, &wifi->message, 40); */
        }
        wifi->retryNum = 0;
    }
    //暂不考虑IPV6
}

/*
 * static void timer_cb(void *arg) {
 *     ModuleMessage message;
 *     message.attr = ModuleDataAttr_TriggerRecv;
 *     esp_event_post(WIFI_EVENT, WIFI_EVENT_MAX, &message, sizeof(message), 0);
 * }
 */

int32_t WifiTriggerRecv(void *arg) {
    ModuleMessage message;
    message.attr = ModuleDataAttr_TriggerRecv;
    esp_event_post(WIFI_EVENT, WIFI_EVENT_MAX, &message, sizeof(message), 0);

    return 0;
}

void *WifiInit(WifiConfig *config) {
    esp_err_t status                = ESP_FAIL;        
    wifi_init_config_t initConfig   = WIFI_INIT_CONFIG_DEFAULT();
    esp_netif_inherent_config_t netifConfig = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();

    netifConfig.route_prio = 128;
    {
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
    }

    Wifi *wifi = (Wifi *) malloc (sizeof(*wifi));
    ERRP(NULL == wifi, return NULL, 1, "malloc Wifi Instance failure\n");
    memset(wifi, 0x0, sizeof(*wifi));

    wifi->send  = config->send;
    wifi->recv  = config->recv;

    /* strcpy(wifi->message.wifiConfig.ssid, config->ssid); */
    /* strcpy(wifi->message.wifiConfig.passwd, config->password); */

    // Initialize TCP/IP network interface
    status = esp_netif_init();
    ERRP(ESP_OK != status, goto ERR1, 1, "wifi esp_netif_init failure\n");

    // Create default event loop that running in background
    status = esp_event_loop_create_default();
    ERRP(ESP_OK != status, 
            goto ERR2, 1, "wifi esp_event_loop_create_default failure\n");

    status = esp_wifi_init(&initConfig);
    ERRP(ESP_OK != status, goto ERR3, 1, "wifi esp_wifi_init failure\n");

    status = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    ERRP(ESP_OK != status, goto ERR4, 1, "wifi esp_wifi_set_storage failure\n");

    strcpy((char *)wifi->config.sta.ssid, (const char *)config->ssid);
    strcpy((char *)wifi->config.sta.password, (const char *)config->password);
    wifi->config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi->config.sta.sae_pwe_h2e        = WPA3_SAE_PWE_BOTH;
    strcpy((char *)wifi->config.sta.sae_h2e_identifier, "");

    status = esp_wifi_set_mode(WIFI_MODE_STA);
    ERRP(ESP_OK != status, goto ERR8, 1, "wifi esp_wifi_set_mode failure\n");

    status = esp_wifi_set_config(WIFI_IF_STA, &wifi->config);
    ERRP(ESP_OK != status, goto ERR8, 1, "wifi esp_wifi_set_config sta failure\n");

    wifi->staNetif = esp_netif_create_wifi(WIFI_IF_STA, &netifConfig);
    ERRP(NULL == wifi->staNetif, goto ERR9, 1, "wifi esp_netif_create_wifi failure\n");

    esp_wifi_set_default_wifi_sta_handlers();

    status = esp_event_handler_instance_register(WIFI_EVENT,
            ESP_EVENT_ANY_ID, &WifiEventHandler,
            wifi, &wifi->instanceAnyId);
    ERRP(ESP_OK != status, goto ERR5, 1, 
            "esp_event_handler_instance_register STA_DISCONNECTED\n");

    status = esp_event_handler_instance_register(IP_EVENT,
            IP_EVENT_STA_GOT_IP, &WifiEventHandler, wifi, &wifi->instanceGotIp);
    ERRP(ESP_OK != status, goto ERR6, 1, 
            "esp_event_handler_instance_register IP_EVENT_STA_GOT_IP\n");

    status = esp_event_handler_instance_register(IP_EVENT,
            IP_EVENT_GOT_IP6, &WifiEventHandler, wifi, &wifi->instanceGotIpV6);
    ERRP(ESP_OK != status, goto ERR7, 1, 
            "esp_event_handler_instance_register WIFI_EVENT_STA_CONNECTED\n");

    status = esp_wifi_start();
    ERRP(ESP_OK != status, goto ERR10, 1, "wifi esp_wifi_start sta failure\n");

    /*
     * const esp_timer_create_args_t timer_args = {
     *     timer_cb,
     *     wifi,
     *     ESP_TIMER_TASK,
     *     "wifi_timer",
     *     true,
     * };
     * esp_timer_create(&timer_args, &wifi->timer);
     * esp_timer_start_periodic(wifi->timer, 1000000);//10ms
     */

    while (!wifi->wifiSok) {
        vTaskDelay(pdMS_TO_TICKS(200));
        LogPrintf(LogWifi_Info, "wifi->wifiSok:%d\n", wifi->wifiSok);
    }
    LogPrintf(LogWifi_Info, "<<<wifi->wifiSok:%d>>>\n", wifi->wifiSok);

    return wifi;
ERR10:
ERR9:
    esp_netif_destroy(wifi->staNetif);
ERR8:
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_GOT_IP6, &WifiEventHandler);
ERR7:
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &WifiEventHandler);
ERR6:
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiEventHandler);
ERR5:
    esp_netif_destroy(wifi->staNetif);
ERR4:
    esp_wifi_deinit();
ERR3:
    esp_event_loop_delete_default();
ERR2:
    esp_netif_deinit();
ERR1:
    free(wifi);

    return NULL;
}


