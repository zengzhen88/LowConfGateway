#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include <mqtt.h>
#include "mqtt_client.h"
#include <message.h>
#include "esp_timer.h"

static void *gPriv = NULL;
static MQTTPrint gPrint;
static LogMQTT gLevel = LogMQTT_Info;


/*************************************************/
/**************LogPrintf**************************/
/*************************************************/



static int MQTTLogPrintf(LogMQTT level,
        const char *file, const char *func,
        int line, const char *format, ...) {
    char logBuf[128];
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
    MQTTLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogMQTT_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogMQTT_Debug, __VA_ARGS__);
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
    char *name;
    int32_t isRunning;

    esp_mqtt_client_handle_t client;
    esp_timer_handle_t timer;

    MQTTSigSend send;
    MQTTSigRecv recv;

    char *clientId;
    char *url;
    char *hostname;
    char *username;
    char *password;

    int port;
} MQTT;

//设置日志输出对象，主要看你想输出到哪里，就对应填充回调
int32_t MQTTInitLog(void *priv, MQTTPrint print) {
    gPriv   = priv;
    gPrint  = print;
    return 0;
}

//设置日志输出等级，看需要，调试、错误、宕机等
int32_t MQTTSetLogLevel(LogMQTT level) {
    gLevel  = level;
    return 0;
}

int32_t MQTTMessageRecvHandler(MQTT *mqtt) {
    if (mqtt->recv) {
        char recv[128];
        int32_t length = sizeof(recv);
        int status = mqtt->recv(gPriv, DataAttr_UartToMqtt, recv, &length, 0);
        if (!status) {
            /*AT命令检测*/
        }

        ModuleMessage message;
        length = sizeof(message);
        status = mqtt->recv(gPriv, DataAttr_WifiToMqtt, &message, &length, 0);
        if (!status) {
            /*测试*/
            switch (message.attr) {
                case ModuleDataAttr_helloworld:
                    {
                        status = esp_mqtt_client_subscribe(mqtt->client, 
                                toEnumString(message.attr), 
                                1);
                        if (status) {
                            LogPrintf(LogMQTT_Info, "failurellllllllllll status:%d\n", status);
                        }
                        esp_mqtt_client_publish(mqtt->client, 
                                toEnumString(message.attr), 
                                message.helloworld.helloworld, 
                                strlen(message.helloworld.helloworld), 
                                1, 0);
                        break;
                    }
                default:break;
            }
        }
    }

    return 0;
}

int32_t MQTTMessageSendHandler(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    if (!strncmp(event->topic, "zengzhen", strlen("zengzhen"))) {
        LogPrintf(LogMQTT_Info, "<DATA>=%.*s\n", event->data_len, event->data);
        if (mqtt->send) 
            mqtt->send(gPriv, DataAttr_MqttToWifi, event->data, 
                    event->data_len, DataTimeStatus_BLOCK);
    }
    /* else if (!strcmp(event->topic, "xxx")) { */

    /* } */

    return 0;
}

void MQTTEventHandler(void *handler_args, esp_event_base_t base,
        int32_t event_id, void *event_data) {
    MQTT *mqtt = (MQTT *)handler_args;

    LogPrintf(LogMQTT_Info, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "\n", base, event_id);

    /* int msg_id; */
    esp_mqtt_event_handle_t event   = event_data;

    switch (/* (esp_mqtt_event_id_t) */event_id) {
        case MQTT_EVENT_CONNECTED:
            {
                LogPrintf(LogMQTT_Info, "[%s]MQTT_EVENT_CONNECTED\n", mqtt->name);
                break;
            }
        case MQTT_EVENT_DISCONNECTED:
            {
                LogPrintf(LogMQTT_Info, "[%s]MQTT_EVENT_DISCONNECTED\n", mqtt->name);
                break;
            }
        case MQTT_EVENT_SUBSCRIBED:
            {
                LogPrintf(LogMQTT_Info, "[%s]MQTT_EVENT_SUBSCRIBED\n", mqtt->name);
                break;
            }
        case MQTT_EVENT_UNSUBSCRIBED:
            {
                LogPrintf(LogMQTT_Info, "[%s]MQTT_EVENT_UNSUBSCRIBED\n", mqtt->name);
                break;
            }
        case MQTT_EVENT_PUBLISHED:
            {
                LogPrintf(LogMQTT_Info, "[%s]MQTT_EVENT_PUBLISHED\n", mqtt->name);
                break;
            }
        case MQTT_EVENT_DATA:
            {
                LogPrintf(LogMQTT_Info, "[%s]MQTT_EVENT_DATA\n", mqtt->name);
                LogPrintf(LogMQTT_Info, "TOPIC=%.*s\r\n", event->topic_len, event->topic);
                LogPrintf(LogMQTT_Info, "DATA=%.*s\r\n", event->data_len, event->data);

                MQTTMessageSendHandler(mqtt, event);
                /* event中会包含current_data_offset和total_data_len请用户自行拼接碎片消息 */
                break;
            }
        case MQTT_EVENT_ERROR:
            {
                LogPrintf(LogMQTT_Info, "[%s]MQTT_EVENT_ERROR\n", mqtt->name);
                if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                    LogPrintf(LogMQTT_Info, "reported from esp-tls : 0x%x\n", event->error_handle->esp_tls_last_esp_err);
                    LogPrintf(LogMQTT_Info, "reported from tls stack : 0x%x \n", event->error_handle->esp_tls_stack_err);
                    LogPrintf(LogMQTT_Info, "captured as transport's socket errno : 0x%x \n",  event->error_handle->esp_transport_sock_errno);
                    LogPrintf(LogMQTT_Info, "Last errno string (%s)\n", strerror(event->error_handle->esp_transport_sock_errno));
                }
                break;
            }
        case MQTT_USER_EVENT:
            {
                MQTTMessageRecvHandler(mqtt);
                break;
            }
        default:
            {
                LogPrintf(LogMQTT_Info, "[%s]Other event id:%d\n", mqtt->name, event->event_id);
                break;
            }
    }
}

static void timer_cb(void *arg) {
    MQTT *mqtt = (MQTT *)arg;

    esp_mqtt_event_t event;
    event.event_id  = MQTT_USER_EVENT;
    event.topic     = (char *) malloc (strlen("zengtimer") + 1);
    if (event.topic) {
        strcpy(event.topic, "zengtimer");
        event.topic_len = strlen("zengtimer");
    }
    else {
        event.topic_len = 0;
    }

    esp_mqtt_dispatch_custom_event(mqtt->client, &event);
}

int32_t MQTTPushlish(void *oObj, const char *data, int32_t len, 
        const char *topic, int qos, int retain) {
    MQTT *pObj = (MQTT *)oObj;

    esp_mqtt_event_t event;
    event.event_id  = MQTT_USER_EVENT;
    event.qos       = qos;
    event.topic     = (char *) malloc (strlen(topic) + 1);
    if (event.topic) {
        strcpy(event.topic, topic);
        event.topic_len = strlen(topic);
    }
    else {
        event.topic_len = 0;
    }
    event.data      = (char *) malloc (len + 1);
    if (event.data) {
        memcpy(event.data, data, len);
        event.data_len = len;
    }
    else {
        event.data_len = 0;
    }

    //暂时先这样设计
    /* return esp_mqtt_client_publish( */
            /* pObj->client, topic, data, len, qos, retain); */
    return esp_mqtt_dispatch_custom_event(pObj->client, &event);
}

/*
 * static int32_t MQTTSubscribe(void *oObj, const char *topic, int qos) {
 *     MQTT *pObj = (MQTT *)oObj;
 * 
 *     esp_mqtt_event_t event;
 *     event.event_id  = MQTT_USER_EVENT;
 *     event.qos       = qos;
 *     event.topic     = (char *) malloc (strlen(topic) + 1);
 *     if (event.topic) {
 *         strcpy(event.topic, topic);
 *         event.topic_len = strlen(topic);
 *     }
 *     else {
 *         event.topic_len = 0;
 *     }
 *     return esp_mqtt_dispatch_custom_event(pObj->client, &event);
 * }
 */

void *MQTTInit(MQTTConfig *config) {
    /* esp_err_t status                = ESP_FAIL;         */
    esp_mqtt_client_config_t mqttConfig;

    MQTT *mqtt = (MQTT *) malloc (sizeof(*mqtt));
    ERRP(NULL == mqtt, return NULL, 1, "malloc MQTT Instance failure\n");
    memset(mqtt, 0x0, sizeof(*mqtt));

    mqtt->port      = config->port;
    mqtt->send      = config->send;
    mqtt->recv      = config->recv;

    LogPrintf(LogMQTT_Info, "url     :%s\n", config->url);
    LogPrintf(LogMQTT_Info, "hostname:%s\n", config->hostname);
    LogPrintf(LogMQTT_Info, "username:%s\n", config->username);
    LogPrintf(LogMQTT_Info, "password:%s\n", config->password);
    LogPrintf(LogMQTT_Info, "port    :%" PRIu32 "\n", config->port);

    mqtt->name = (char *) malloc (strlen(config->name) + 1);
    ERRP(NULL == mqtt->name, goto ERR00, 1, "mqtt malloc name mem failure\n");
    strcpy(mqtt->name, config->name);

    mqtt->url = (char *) malloc (strlen(config->url) + 1);
    ERRP(NULL == mqtt->url, goto ERR0, 1, "mqtt malloc url mem failure\n");
    strcpy(mqtt->url, config->url);

    mqtt->hostname = (char *) malloc (strlen(config->hostname) + 1);
    ERRP(NULL == mqtt->hostname, goto ERR1, 1, "mqtt malloc hostname mem failure\n");
    strcpy(mqtt->hostname, config->hostname);

    mqtt->username = (char *) malloc (strlen(config->username) + 1);
    ERRP(NULL == mqtt->username, goto ERR2, 1, "mqtt malloc username mem failure\n");
    strcpy(mqtt->username, config->username);

    mqtt->password = (char *) malloc (strlen(config->password) + 1);
    ERRP(NULL == mqtt->password, goto ERR3, 1, "mqtt malloc password mem failure\n");
    strcpy(mqtt->password, config->password);

    mqtt->clientId = (char *) malloc (10);
    ERRP(NULL == mqtt->clientId, goto ERR5, 1, "mqtt malloc clientId failure\n");
    snprintf (mqtt->clientId, 10, "ESP32_888");/*后面调整*/

    memset(&mqttConfig, 0x0, sizeof(mqttConfig));
    mqttConfig.broker.address.uri                   = mqtt->url;
    mqttConfig.broker.address.port                  = mqtt->port;
    mqttConfig.broker.address.hostname              = mqtt->hostname;
    mqttConfig.credentials.username                 = mqtt->username;
    mqttConfig.credentials.authentication.password  = mqtt->password;
    mqttConfig.credentials.client_id                = mqtt->clientId;

    LogPrintf(LogMQTT_Info, "uri:%s \n",
            mqttConfig.broker.address.uri);
    LogPrintf(LogMQTT_Info, "hostname:%s \n",
            mqttConfig.broker.address.hostname);
    LogPrintf(LogMQTT_Info, "port:%d \n",
            mqttConfig.broker.address.port);
    LogPrintf(LogMQTT_Info, "username:%s \n",
            mqttConfig.credentials.username);
    LogPrintf(LogMQTT_Info, "password:%s \n",
            mqttConfig.credentials.authentication.password);
    LogPrintf(LogMQTT_Info, "client_id:%s \n",
            mqttConfig.credentials.client_id);

    const esp_timer_create_args_t timer_args = {
        timer_cb,
        mqtt,
        ESP_TIMER_TASK,
        "mqtt_timer",
        true,
    };
    esp_timer_create(&timer_args, &mqtt->timer);

    mqtt->client = esp_mqtt_client_init(&mqttConfig);
    ERRP(NULL == mqtt->client, goto ERR5, 1, "mqtt esp_mqtt_client_init failure\n");

    esp_mqtt_client_register_event(mqtt->client, 
            ESP_EVENT_ANY_ID, MQTTEventHandler, mqtt);

    esp_mqtt_client_start(mqtt->client);

    esp_timer_start_periodic(mqtt->timer, 10000);//10ms

    return mqtt;
ERR5:
    free(mqtt->password);
ERR3:
    free(mqtt->username);
ERR2:
    free(mqtt->hostname);
ERR1:
    free(mqtt->url);
ERR0:
    free(mqtt->name);
ERR00:
    free(mqtt);
    return NULL;
}

