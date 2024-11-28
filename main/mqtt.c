#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

/* #define USE_BSON */

#include "lwip/err.h"
#include "lwip/sys.h"

#include <mqtt.h>
#include "mqtt_client.h"
#include <message.h>
#include "esp_timer.h"
#ifdef USE_BSON
#include "bson/bson.h"
#endif
#include "cJSON.h"
#include "re.h"

static void *gPriv = NULL;
static MQTTPrint gPrint;
static LogMQTT gLevel = LogMQTT_Info;


/*************************************************/
/**************LogPrintf**************************/
/*************************************************/

extern int cpp_regexFunction(const char *inStr, const char *matchStr);


static int MQTTLogPrintf(LogMQTT level,
        const char *file, const char *func,
        int line, const char *format, ...) {
    char logBuf[256];
    va_list args;
    int funcLine        = 0;

    if (level > gLevel) return -1;

    snprintf (logBuf, sizeof(logBuf), "[%s][%s][%s][%d]", file, func, esp_log_system_timestamp(), line);
    funcLine = strlen(logBuf);

    /*va_list*/
    va_start(args, format);
    vsnprintf(&logBuf[funcLine], sizeof(logBuf) - funcLine - 1, format, args);
    va_end(args);
    /*va_list*/
    logBuf[255] = '\0';

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

typedef enum {
    MYMQTTERNET_EVENT_START,
    MYMQTTERNET_EVENT_STOPMQTT,
    MYMQTTERNET_EVENT_STARTMQTT,
} myeth_event_t;
ESP_EVENT_DEFINE_BASE(MYMQTT_EVENT);
#define MYMQTT_EVENT_MQTT_ANY_ID (-1)

typedef struct {
    int32_t isRunning;
    int32_t connect;

    esp_event_loop_handle_t event;

    esp_mqtt_client_handle_t client;
    esp_timer_handle_t timer;

    MQTTSigSend send;
    MQTTSigRecv recv;
    MQTTSigPeek peek;
    MQTTSigRequest request;
    MQTTSigRelease release;

    int32_t msgQos;
    int32_t dataQos;

    char name[32];

    char version[32];
    char info[32];

    PowerSupplyMode mode;
    int32_t level;

    char url[128];
    char clientid[128];
    char username[32];
    char password[32];



    char mac[128];
    char topic[128];
    char topicack[128];

    int32_t isRegex; /*正则表达式是否有效*/
    char devType[128];  /*上报的设备类型，内部、外部 和所有  0 1 2*/
    char regex[128];

    int checkStatus;

    int netFirstConnect;

    ModuleDataAttr attr;

    esp_mqtt_client_config_t mqttConfig;
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

int32_t MQTTMessageRecvGetWifiConfig(MQTT *mqtt, ModuleMessage *message) {
    cJSON *root = NULL;
    cJSON *sub  = NULL;

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetWifiCfg));
            cJSON_AddStringToObject(sub, "ssid", message->getWifiCfg.ssid);
            cJSON_AddStringToObject(sub, "passwd", message->getWifiCfg.password);
            cJSON_AddStringToObject(sub, "ip", message->getWifiCfg.address); 
            cJSON_AddStringToObject(sub, "netmask", message->getWifiCfg.netmask); 
            cJSON_AddStringToObject(sub, "gateway", message->getWifiCfg.gateway); 
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                mqtt->topicack, //toEnumString(message->attr), 
                                (const char *)json,
                                strlen(json),
                                mqtt->msgQos, 0);
#else 
                bson_error_t error;
                bson_t *bson = bson_new_from_json((const uint8_t *)json, strlen(json), &error);
                if (bson) {
                    const uint8_t *bsons = bson_get_data(bson);
                    if (mqtt->connect)
                        if (mqtt->connect)
                            esp_mqtt_client_publish(mqtt->client, 
                                    mqtt->topicack, //toEnumString(message->attr), 
                                    (const char *)bsons,
                                    bson->len,
                                    mqtt->msgQos, 0);
                }
#endif
                free(json);
            }
        }
        cJSON_Delete(root);
    }


    return 0;
}

int32_t MQTTMessageRecvWifiHandler(MQTT *mqtt) {
    if (mqtt->recv) {
        while (1) {
            ModuleMessage message;
            int32_t length = sizeof(message);
            int status = mqtt->recv(gPriv, DataAttr_WifiToMqtt, &message, &length, 0);
            if (!status) {
                if (!mqtt->netFirstConnect) {
                    mqtt->netFirstConnect = 1;
                    esp_mqtt_client_start(mqtt->client);
                    LogPrintf(LogMQTT_Info, "NetConnect, Start Mqtt\n");
                }
                /*
                 * else if (message.ack.attr == ModuleDataAttr_Ack) {
                 *     switch (message.ack.srcAttr) {
                 *         case ModuleDataAttr_SetWifiCfg:
                 *             {
                 *                 return message.ack.status;
                 *             }
                 *         default:break;
                 *     }
                 * }
                 */
                else {

                }
            }
            else {
                break;
            }
        }
    }

    return -1;
}

int32_t MQTTMessageRecvUartGetModuleVersionHandler(MQTT *mqtt, char *version) {
    cJSON *root = NULL;
    cJSON *sub  = NULL;
    char *module = NULL;
    char *firmwareVer = NULL;
    char *hardwareVer = NULL;
    char *mac = NULL;

    char *sptr = version;//strchr(info, '<');
    if (sptr) {
        //sptr = module,hardwareVer,firmwareVer,mac
        char *ptr = strchr(sptr, ',');
        if (ptr) {
            //ptr = ,hardwareVer,firmwareVer,mac
            *ptr = '\0';
            module = sptr;
            sptr = ptr + 1;
            //sptr = hardwareVer,firmwareVer,mac
            if (sptr) {
                ptr = strchr(sptr, ',');
                if (ptr) {
                    *ptr = '\0';
                    //ptr = ,firmwareVer,mac
                    hardwareVer = sptr;
                    sptr = ptr + 1;
                    //sptr firmwareVer,mac
                    if (sptr) {
                        ptr = strchr(sptr, ',');
                        if (ptr) {
                            *ptr = '\0';
                            //ptr = ,mac
                            firmwareVer = sptr;
                            sptr = ptr + 1;
                            if (sptr) {
                                mac = sptr;
                            }
                        }
                    }
                }
            }
        }
    }

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetModuleVersion));
            cJSON_AddStringToObject(sub, "Module", module);
            cJSON_AddStringToObject(sub, "HardwareVer", hardwareVer);
            cJSON_AddStringToObject(sub, "FirmwareVer", firmwareVer);
            cJSON_AddStringToObject(sub, "mac", mac);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                mqtt->topicack, //toEnumString(ModuleDataAttr_GetModuleVersion), 
                                (const char *)json,
                                strlen(json),
                                mqtt->msgQos, 0);
#else 
                bson_error_t error;
                bson_t *bson = bson_new_from_json((const uint8_t *)json, strlen(json), &error);
                if (bson) {
                    const uint8_t *bsons = bson_get_data(bson);
                    if (mqtt->connect)
                        if (mqtt->connect)
                            esp_mqtt_client_publish(mqtt->client, 
                                    mqtt->topicack, //toEnumString(ModuleDataAttr_GetModuleVersion), 
                                    (const char *)bsons,
                                    bson->len,
                                    mqtt->msgQos, 0);
                }
#endif
                free(json);
            }
        }
        cJSON_Delete(root);
    }

    return 0;
}

int32_t MQTTMessageRecvUartCommonOkHandler(MQTT *mqtt, const char *isOk, ModuleDataAttr attr) {
    cJSON *root = NULL;
    cJSON *sub  = NULL;

    if ((attr == ModuleDataAttr_SetWifiCfg) 
            || (attr == ModuleDataAttr_SetEthCfg)) {
        esp_mqtt_client_unsubscribe(mqtt->client, mqtt->topic);
        esp_mqtt_client_reconnect(mqtt->client);
        esp_mqtt_client_subscribe(mqtt->client, mqtt->topic, mqtt->msgQos);
    }

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            cJSON_AddStringToObject(sub, "htype", toEnumString(attr));
            cJSON_AddStringToObject(sub, "status", isOk);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                mqtt->topicack, //toEnumString(attr), 
                                (const char *)json,
                                strlen(json),
                                mqtt->msgQos, 0);
#else 
                bson_error_t error;
                bson_t *bson = bson_new_from_json((const uint8_t *)json, strlen(json), &error);
                if (bson) {
                    const uint8_t *bsons = bson_get_data(bson);
                    if (mqtt->connect)
                        if (mqtt->connect)
                            esp_mqtt_client_publish(mqtt->client, 
                                    mqtt->topicack, //toEnumString(attr), 
                                    (const char *)bsons,
                                    bson->len,
                                    mqtt->msgQos, 0);
                }
#endif
                free(json);
            }
        }
        cJSON_Delete(root);
    }

    return 0;
}

int32_t MQTTMessageRecvUartRebootHandler(MQTT *mqtt, char *info) {
    LogPrintf(LogMQTT_Info, "start reboot...esp_restart()\n");
    /*同步一些数据，如果没有数据要同步，不处理*/

    /*send OK to module*/
    return 0;
}

int32_t MQTTMessageStopMqtt(MQTT *mqtt) {
    ModuleDataAttr attr;
    ModuleMessage message;
    message.attr = ModuleDataAttr_SetMqttCfg;
    esp_event_post_to(mqtt->event, MYMQTT_EVENT, 
            MYMQTTERNET_EVENT_STOPMQTT, &message, sizeof(message), 0);
    return 0;
}
int32_t MQTTMessageStartMqtt(MQTT *mqtt) {
    ModuleDataAttr attr;
    ModuleMessage message;
    message.attr = ModuleDataAttr_SetMqttCfg;
    esp_event_post_to(mqtt->event, MYMQTT_EVENT, 
            MYMQTTERNET_EVENT_STARTMQTT, &message, sizeof(message), 0);
    return 0;
}

int32_t MQTTMessageRecvUartDEBUGHandle(MQTT *mqtt, char *info, int32_t report) {
    char *ssid              = NULL;
    char *url               = NULL;
    char *password          = NULL;
    char *address           = NULL;
    char *netmask           = NULL;
    char *gateway           = NULL;
    char *user              = NULL;
    char *clientid          = NULL;

    /*checketh:address,netmask,gateway*/
    /*checkwifi:ssid,password,address,netmask,gateway*/
    /*checkmqtt:url,user,password,clientid*/

    printf ("info:%s\n", info);
    if (strstr(info, "checketh")) {
        /*eth:address,netmask,gateway*/
        char *sptr = (strchr(info, ':')) + 1;
        if (sptr) {
            char *ptr = strchr(sptr, ',');
            if (ptr) {
                //ptr = ,netmask,gateway
                *ptr = '\0';
                address = sptr;
                sptr = ptr + 1;
                //sptr = netmask,gateway
                if (sptr) {
                    ptr = strchr(sptr, ',');
                    if (ptr) {
                        //ptr = ,gateway
                        *ptr = '\0';
                        netmask = sptr;
                        sptr = ptr + 1;
                        //sptr = gateway
                        if (sptr) {
                            gateway = sptr;
                            ModuleMessage message;
                            message.attr = ModuleDataAttr_SetEthCfg;
                            strcpy(message.setWifiCfg.address, address);
                            strcpy(message.setWifiCfg.netmask, netmask);
                            strcpy(message.setWifiCfg.gateway, gateway);
                            if (mqtt->send) {
                                mqtt->send(gPriv, DataAttr_MqttToEth, &message, sizeof(message), 0);
                            }
                        }
                    }
                }
            }
        }
    }
    else if (strstr(info, "checkwifi")) {
        /*wifi:ssid,password,address,netmask,gateway*/
        char *sptr = (strchr(info, ':')) + 1;
        if (sptr) {
            char *ptr = strchr(sptr, ',');
            if (ptr) {
                //ptr = ,password,address,netmask,gateway
                *ptr = '\0';
                ssid = sptr;
                sptr = ptr + 1;
                //sptr = password,address,netmask,gateway
                if (sptr) {
                    ptr = strchr(sptr, ',');
                    if (ptr) {
                        //ptr = ,address,netmask,gateway
                        *ptr = '\0';
                        password = sptr;
                        sptr = ptr + 1;
                        //sptr = address,netmask,gateway
                        if (sptr) {
                            ptr = strchr(sptr, ',');
                            if (ptr) {
                                //ptr = ,netmask,gateway
                                *ptr = '\0';
                                address = sptr;
                                sptr = ptr + 1;
                                //netmask,gateway 
                                if (sptr) {
                                    ptr = strchr(sptr, ',');
                                    if (ptr) {
                                        //,gateway
                                        *ptr = '\0';
                                        netmask = sptr;
                                        sptr = ptr + 1;
                                        //gateway
                                        if (sptr) {
                                            gateway = sptr;
                                            ModuleMessage message;
                                            message.attr = ModuleDataAttr_SetWifiCfg;
                                            strcpy(message.setWifiCfg.ssid, ssid);
                                            strcpy(message.setWifiCfg.password, password);
                                            strcpy(message.setWifiCfg.address, address);
                                            strcpy(message.setWifiCfg.netmask, netmask);
                                            strcpy(message.setWifiCfg.gateway, gateway);
                                            if (mqtt->send) {
                                                mqtt->send(gPriv, DataAttr_MqttToWifi, &message, sizeof(message), 0);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else if (strstr(info, "checkmqtt")) {
        /*checkmqtt:url,user,password,clientid*/
        char *sptr = (strchr(info, ':')) + 1;
        if (sptr) {
            char *ptr = strchr(sptr, ',');
            if (ptr) {
                //ptr = ,user,password,clientid
                *ptr = '\0';
                url = sptr;
                sptr = ptr + 1;
                //sptr = user,password,clientid
                if (sptr) {
                    ptr = strchr(sptr, ',');
                    if (ptr) {
                        //ptr = ,password,clientid
                        *ptr = '\0';
                        user = sptr;
                        sptr = ptr + 1;
                        //sptr = password,clinetid
                        if (sptr) {
                            ptr = strchr(sptr, ',');
                            if (ptr) {
                                //ptr = ,clinetid
                                *ptr = '\0';
                                password = sptr;
                                sptr = ptr + 1;
                                //sptr = clientid
                                if (sptr) {
                                    clientid = sptr;
                                    /* esp_mqtt_client_config_t mqttConfig; */
                                    /* memset(&mqttConfig, 0x0, sizeof(mqttConfig)); */
                                    strcpy(mqtt->mqttConfig.broker.address.uri, url);
                                    strcpy(mqtt->mqttConfig.credentials.username, user);
                                    strcpy(mqtt->mqttConfig.credentials.authentication.password, password);
                                    strcpy(mqtt->mqttConfig.credentials.client_id, clientid);
                                    esp_mqtt_client_unsubscribe(mqtt->client, mqtt->topic);
                                    mqtt->checkStatus = 1;
                                    snprintf (mqtt->topic, sizeof(mqtt->topic) - 1, 
                                            "GZ248_%02x%02x%02x%02x%02x%02x_%s", 
                                            mqtt->mac[0], mqtt->mac[1], mqtt->mac[2], 
                                            mqtt->mac[3], mqtt->mac[4], mqtt->mac[5], clientid);
                                    snprintf (mqtt->topicack, sizeof(mqtt->topicack) - 1, 
                                            "GZ248_%02x%02x%02x%02x%02x%02x_%sAck", 
                                            mqtt->mac[0], mqtt->mac[1], mqtt->mac[2], 
                                            mqtt->mac[3], mqtt->mac[4], mqtt->mac[5], clientid);
                                    MQTTMessageStopMqtt(mqtt);
                                    MQTTMessageStartMqtt(mqtt);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}

int32_t MQTTMessageRecvUartSetREGEXHandle(MQTT *mqtt, char *info, int32_t report) {
    cJSON *root             = NULL;
    cJSON *sub              = NULL;
    char *regex             = NULL;

    printf ("info:%s\n", info);
    char *sptr = info;//strchr(info, '<');
    if (sptr) {
        regex = (sptr);
    }

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            if (report)
                cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_ReportREGEX));
            else
                cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetREGEX));
            cJSON_AddStringToObject(sub, "data", regex);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                mqtt->topicack, //toEnumString(ModuleDataAttr_SetScanTimeout), 
                                (const char *)json,
                                strlen(json),
                                mqtt->msgQos, 0);
#else 
                bson_error_t error;
                bson_t *bson = bson_new_from_json((const uint8_t *)json, strlen(json), &error);
                if (bson) {
                    const uint8_t *bsons = bson_get_data(bson);
                    if (mqtt->connect)
                        if (mqtt->connect)
                            esp_mqtt_client_publish(mqtt->client, 
                                    mqtt->topicack, //toEnumString(ModuleDataAttr_SetScanTimeout), 
                                    (const char *)bsons,
                                    bson->len,
                                    mqtt->msgQos, 0);
                }
#endif
                free(json);
            }
        }
        cJSON_Delete(root);
    }

    return 0;
}

int32_t MQTTMessageRecvUartSetScanTimeoutHandle(MQTT *mqtt, char *info, int32_t report) {
    cJSON *root             = NULL;
    cJSON *sub              = NULL;
    int sec                 = 0;

    printf ("info:%s\n", info);
    char *sptr = info;//strchr(info, '<');
    if (sptr) {
        sec = atoi(sptr);
    }

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            if (report)
                cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_ReportScanTimeout));
            else
                cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetScanTimeout));
            cJSON_AddNumberToObject(sub, "sec", sec);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                mqtt->topicack, //toEnumString(ModuleDataAttr_SetScanTimeout), 
                                (const char *)json,
                                strlen(json),
                                mqtt->msgQos, 0);
#else 
                bson_error_t error;
                bson_t *bson = bson_new_from_json((const uint8_t *)json, strlen(json), &error);
                if (bson) {
                    const uint8_t *bsons = bson_get_data(bson);
                    if (mqtt->connect)
                        if (mqtt->connect)
                            esp_mqtt_client_publish(mqtt->client, 
                                    mqtt->topicack, //toEnumString(ModuleDataAttr_SetScanTimeout), 
                                    (const char *)bsons,
                                    bson->len,
                                    mqtt->msgQos, 0);
                }
#endif
                free(json);
            }
        }
        cJSON_Delete(root);
    }

    return 0;
}

int32_t MQTTMessageRecvUartPtRecvHandler(MQTT *mqtt, char *info, ModuleDataAttr attr) {
    cJSON *root             = NULL;
    cJSON *sub              = NULL;
    char *mac               = NULL;
    uint32_t seq            = 0;
    char *state             = NULL;

    printf ("info:%s\n", info);
    //mac,seq,data
    char *sptr = info;//strchr(info, '<');
    if (sptr) {
        char *ptr = strchr(sptr, ',');
        if (ptr) {
            //ptr = ,seq,data
            *ptr = '\0';
            mac = sptr;
            sptr = ptr + 1;
            //sptr = seq,data
            if (sptr) {
                ptr = strchr(sptr, ',');
                if (ptr) {
                    //ptr = ,data
                    *ptr = '\0';
                    seq = atoi(sptr);
                    sptr = ptr + 1;
                    //sptr = data
                    if (sptr) {
                        state = sptr;
                    }
                }
            }
        }
    }

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_PtRecv));
            cJSON_AddStringToObject(sub, "mac", mac);
            cJSON_AddNumberToObject(sub, "seq", (float)seq);
            cJSON_AddStringToObject(sub, "state", state);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                mqtt->topicack, //toEnumString(ModuleDataAttr_GetMqttCfg), 
                                (const char *)json,
                                strlen(json),
                                mqtt->msgQos, 0);
#else 
                bson_error_t error;
                bson_t *bson = bson_new_from_json((const uint8_t *)json, strlen(json), &error);
                if (bson) {
                    const uint8_t *bsons = bson_get_data(bson);
                    if (mqtt->connect)
                        if (mqtt->connect)
                            esp_mqtt_client_publish(mqtt->client, 
                                    mqtt->topicack, //toEnumString(ModuleDataAttr_GetMqttCfg), 
                                    (const char *)bsons,
                                    bson->len,
                                    mqtt->msgQos, 0);
                }
#endif
                free(json);
            }
        }
        cJSON_Delete(root);
    }

    return 0;
}

int32_t MQTTMessageRecvUartPTUpdateHandle(MQTT *mqtt, char *info) {
    cJSON *root             = NULL;
    cJSON *sub              = NULL;
    char *mac               = NULL;
    char *data             = NULL;

    printf ("info:%s\n", info);
    //mac,data
    char *sptr = info;//strchr(info, '<');
    if (sptr) {
        char *ptr = strchr(sptr, ',');
        if (ptr) {
            //ptr = ,data
            *ptr = '\0';
            mac = sptr;
            sptr = ptr + 1;
            //sptr = data
            if (sptr) {
                data = sptr;
            }
        }
    }

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_ReportTransmitData));
            cJSON_AddStringToObject(sub, "mac", mac);
            cJSON_AddStringToObject(sub, "data", data);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                mqtt->topicack, //toEnumString(ModuleDataAttr_GetMqttCfg), 
                                (const char *)json,
                                strlen(json),
                                mqtt->msgQos, 0);
#else 
                bson_error_t error;
                bson_t *bson = bson_new_from_json((const uint8_t *)json, strlen(json), &error);
                if (bson) {
                    const uint8_t *bsons = bson_get_data(bson);
                    if (mqtt->connect)
                        if (mqtt->connect)
                            esp_mqtt_client_publish(mqtt->client, 
                                    mqtt->topicack, //toEnumString(ModuleDataAttr_GetMqttCfg), 
                                    (const char *)bsons,
                                    bson->len,
                                    mqtt->msgQos, 0);
                }
#endif
                free(json);
            }
        }
        cJSON_Delete(root);
    }

    return 0;
}

int32_t MQTTMessageRecvUartMqttCfgHandler(MQTT *mqtt, char *info, int32_t isUp, int32_t report) {
    cJSON *root             = NULL;
    cJSON *sub              = NULL;
    char *user              = NULL;
    char *password          = NULL;
    char *url               = NULL;
    char *clientid          = NULL;
    short port              = 0;

    printf ("info:%s\n", info);
    //url,port,clientid,user,password
    char *sptr = info;//strchr(info, '<');
    if (sptr) {
        char *ptr = strchr(sptr, ',');
        if (ptr) {
            //,port,clientid,user,password
            *ptr = '\0';
            url = sptr;
            sptr = ptr + 1;
            //port,clientid,user,password
            if (sptr) {
                ptr = strchr(sptr, ',');
                if (ptr) {
                    //,clientid,user,password
                    *ptr = '\0';
                    port = atoi(sptr);
                    sptr = ptr + 1;
                    //clientid,user,password
                    if (sptr) {
                        ptr = strchr(sptr, ',');
                        if (ptr) {
                            //,user,password
                            *ptr = '\0';
                            clientid = sptr;
                            sptr = ptr + 1;
                            //user,password
                            if (sptr) {
                                ptr = strchr(sptr, ',');
                                if (ptr) {
                                    //,password
                                    *ptr = '\0';
                                    user = sptr;
                                    sptr = ptr + 1;
                                    //password
                                    if (sptr) {
                                        password = sptr;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    LogPrintf(LogMQTT_Info, "url:%s\n", url);
    LogPrintf(LogMQTT_Info, "port:%d\n", port);
    LogPrintf(LogMQTT_Info, "clientid:%s\n", clientid);
    LogPrintf(LogMQTT_Info, "user:%s\n", user);
    LogPrintf(LogMQTT_Info, "password:%s\n", password);

    if (isUp) {
        char combineUrl[256];
        snprintf (combineUrl, sizeof(combineUrl) - 1, "%s:%d", url, port);
        combineUrl[sizeof(combineUrl) - 1] = '\0';
        /* esp_mqtt_client_config_t mqttConfig; */
        /* memset(&mqttConfig, 0x0, sizeof(mqttConfig)); */
        strcpy(mqtt->mqttConfig.broker.address.uri, combineUrl);
        strcpy(mqtt->mqttConfig.credentials.username, user);
        strcpy(mqtt->mqttConfig.credentials.authentication.password, password);
        strcpy(mqtt->mqttConfig.credentials.client_id, clientid);
        esp_mqtt_client_unsubscribe(mqtt->client, mqtt->topic);
        MQTTMessageStopMqtt(mqtt);
        MQTTMessageStartMqtt(mqtt);
    }

    if (0) {
        root = cJSON_CreateArray();
        if (root) {
            sub = cJSON_CreateObject();
            if (sub) {
                if (report)
                    cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_ReportMqttCfg));
                else
                    cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetMqttCfg));
                cJSON_AddStringToObject(sub, "user", user);
                cJSON_AddStringToObject(sub, "password", password);
                cJSON_AddStringToObject(sub, "url", url);
                cJSON_AddNumberToObject(sub, "port", port);
                cJSON_AddStringToObject(sub, "clientid", clientid);
                cJSON_AddItemToArray(root, sub);
                char *json = cJSON_Print(root);
                if (json) {
                    LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                    if (mqtt->connect)
                        if (mqtt->connect)
                            esp_mqtt_client_publish(mqtt->client, 
                                    mqtt->topicack, //toEnumString(ModuleDataAttr_GetMqttCfg), 
                                    (const char *)json,
                                    strlen(json),
                                    mqtt->msgQos, 0);
#else 
                    bson_error_t error;
                    bson_t *bson = bson_new_from_json((const uint8_t *)json, strlen(json), &error);
                    if (bson) {
                        const uint8_t *bsons = bson_get_data(bson);
                        if (mqtt->connect)
                            if (mqtt->connect)
                                esp_mqtt_client_publish(mqtt->client, 
                                        mqtt->topicack, //toEnumString(ModuleDataAttr_GetMqttCfg), 
                                        (const char *)bsons,
                                        bson->len,
                                        mqtt->msgQos, 0);
                    }
#endif
                    free(json);
                }
            }
            cJSON_Delete(root);
        }
    }

    return 0;
}

int32_t MQTTMessageRecvUartWifiCfgHandler(MQTT *mqtt, char *info, int32_t isUp, int32_t report) {
    cJSON *root             = NULL;
    cJSON *sub              = NULL;
    /* PowerSupplyMode mode    = PowerCnt; */
    char *ssid              = NULL;
    char *password          = NULL;
    char *address           = NULL;
    char *netmask           = NULL;
    char *gateway           = NULL;

    printf ("info:%s\n", info);
    //ssid,password,address,netmask,gateway
    char *sptr = info;//strchr(info, '<');
    if (sptr) {
        char *ptr = strchr(sptr, ',');
        if (ptr) {
            //ptr = ,password,address,netmask,gateway
            *ptr = '\0';
            ssid = sptr;
            sptr = ptr + 1;
            //sptr = password,address,netmask,gateway
            if (sptr) {
                ptr = strchr(sptr, ',');
                if (ptr) {
                    //ptr = ,address,netmask,gateway
                    *ptr = '\0';
                    password = sptr;
                    sptr = ptr + 1;
                    //sptr = address,netmask,gateway
                    if (sptr) {
                        ptr = strchr(sptr, ',');
                        if (ptr) {
                            //ptr = ,netmask,gateway
                            *ptr = '\0';
                            address = sptr;
                            sptr = ptr + 1;
                            //netmask,gateway 
                            if (sptr) {
                                ptr = strchr(sptr, ',');
                                if (ptr) {
                                    //,gateway
                                    *ptr = '\0';
                                    netmask = sptr;
                                    sptr = ptr + 1;
                                    //gateway
                                    if (sptr) {
                                        gateway = sptr;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (isUp) {
        ModuleMessage message;
        message.attr = ModuleDataAttr_SetWifiCfg;
        strcpy(message.setWifiCfg.ssid, ssid);
        strcpy(message.setWifiCfg.password, password);
        strcpy(message.setWifiCfg.address, address);
        strcpy(message.setWifiCfg.netmask, netmask);
        strcpy(message.setWifiCfg.gateway, gateway);
        if (mqtt->send) {
            mqtt->send(gPriv, DataAttr_MqttToWifi, &message, sizeof(message), 0);
        }
    }

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            if (report)
                cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_ReportWifiCfg));
            else
                cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetWifiCfg));
            cJSON_AddStringToObject(sub, "ssid", ssid);
            cJSON_AddStringToObject(sub, "password", password);
            cJSON_AddStringToObject(sub, "address", address);
            cJSON_AddStringToObject(sub, "netmask", netmask);
            cJSON_AddStringToObject(sub, "gateway", gateway);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                mqtt->topicack, //toEnumString(ModuleDataAttr_GetWifiCfg), 
                                (const char *)json,
                                strlen(json),
                                mqtt->msgQos, 0);
#else 
                bson_error_t error;
                bson_t *bson = bson_new_from_json((const uint8_t *)json, strlen(json), &error);
                if (bson) {
                    const uint8_t *bsons = bson_get_data(bson);
                    if (mqtt->connect)
                        if (mqtt->connect)
                            esp_mqtt_client_publish(mqtt->client, 
                                    mqtt->topicack, //toEnumString(ModuleDataAttr_GetWifiCfg), 
                                    (const char *)bsons,
                                    bson->len,
                                    mqtt->msgQos, 0);
                }
#endif
                free(json);
            }
        }
        cJSON_Delete(root);
    }

    return 0;
}

int32_t MQTTMessageRecvUartEthCfgHandler(MQTT *mqtt, char *info, int isUp, int report) {
    cJSON *root             = NULL;
    cJSON *sub              = NULL;
    /* PowerSupplyMode mode    = PowerCnt; */
    /* char *sptr              = NULL; */
    char *address           = NULL;
    char *netmask           = NULL;
    char *gateway           = NULL;

    printf ("info:%s\n", info);
    //address,netmask,gateway
    char *sptr = info;//strchr(info, '<');
    if (sptr) {
        char *ptr = strchr(sptr, ',');
        if (ptr) {
            //ptr = ,netmask,gateway
            *ptr = '\0';
            address = sptr;
            sptr = ptr + 1;
            //sptr = netmask,gateway
            if (sptr) {
                ptr = strchr(sptr, ',');
                if (ptr) {
                    //ptr = ,gateway
                    *ptr = '\0';
                    netmask = sptr;
                    sptr = ptr + 1;
                    //sptr = gateway
                    if (sptr) {
                        gateway = sptr;
                    }
                }
            }
        }
    }

    if (isUp) {
        ModuleMessage message;
        message.attr = ModuleDataAttr_SetEthCfg;
        strcpy(message.setEthCfg.address, address);
        strcpy(message.setEthCfg.netmask, netmask);
        strcpy(message.setEthCfg.gateway, gateway);
        if (mqtt->send) {
            /* int32_t status =  */mqtt->send(gPriv, DataAttr_MqttToEth, &message, sizeof(message), 0);
            /* if (!status) { */
                /* status = MQTTMessageRecvWifiHandler(mqtt); */
            /* } */
        }
    }

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            if (report)
                cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_ReportEthCfg));
            else 
                cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetEthCfg));
            cJSON_AddStringToObject(sub, "address", address);
            cJSON_AddStringToObject(sub, "netmask", netmask);
            cJSON_AddStringToObject(sub, "gateway", gateway);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                mqtt->topicack, //toEnumString(ModuleDataAttr_GetEthCfg), 
                                (const char *)json,
                                strlen(json),
                                mqtt->msgQos, 0);
#else 
                bson_error_t error;
                bson_t *bson = bson_new_from_json((const uint8_t *)json, strlen(json), &error);
                if (bson) {
                    const uint8_t *bsons = bson_get_data(bson);
                    if (mqtt->connect)
                        if (mqtt->connect)
                            esp_mqtt_client_publish(mqtt->client, 
                                    mqtt->topicack, //toEnumString(ModuleDataAttr_GetEthCfg), 
                                    (const char *)bsons,
                                    bson->len,
                                    mqtt->msgQos, 0);
                }
#endif
                free(json);
            }
        }
        cJSON_Delete(root);
    }

    return 0;
}

int32_t MQTTMessageRecvUartGetPowerHandler(MQTT *mqtt, char *info, int report) {
    cJSON *root             = NULL;
    cJSON *sub              = NULL;
    int32_t power           = 0;;
    char *mode              = NULL;

    printf ("GetPower:%s\n", info);
    /* GetPower:<DC>,<100> */

    char *sptr = info;//strchr(info, '<');
    if (sptr) {
        char *ptr = strchr(sptr, ',');
        if (ptr) {
            //ptr = ,100
            *ptr = '\0';
            mode = sptr;
            sptr = ptr + 1;
            //sptr = 100
            if (sptr) {
                power = atoi(sptr);
            }
        }
    }

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            if (report)
                cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_ReportPower));
            else
                cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetPower));
            cJSON_AddStringToObject(sub, "mode", mode);
            cJSON_AddNumberToObject(sub, "battery_level", power);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                mqtt->topicack, //toEnumString(ModuleDataAttr_GetPower), 
                                (const char *)json,
                                strlen(json),
                                mqtt->msgQos, 0);
#else 
                bson_error_t error;
                bson_t *bson = bson_new_from_json((const uint8_t *)json, strlen(json), &error);
                if (bson) {
                    const uint8_t *bsons = bson_get_data(bson);
                    if (mqtt->connect)
                        if (mqtt->connect)
                            esp_mqtt_client_publish(mqtt->client, 
                                    mqtt->topicack, //toEnumString(ModuleDataAttr_GetPower), 
                                    (const char *)bsons,
                                    bson->len,
                                    mqtt->msgQos, 0);
                }
#endif
                free(json);
            }
        }
        cJSON_Delete(root);
    }

    return 0;
}

int32_t MQTTMessageRecvUartGetModuleInfoHandler(MQTT *mqtt, char *info, int report) {
    cJSON *root = NULL;
    cJSON *sub  = NULL;

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            if (report)
                cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_ReportModuleInfo));
            else
                cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetModuleInfo));
            cJSON_AddStringToObject(sub, "info", info);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                mqtt->topicack, 
                                (const char *)json,
                                strlen(json),
                                mqtt->msgQos, 0);
#else 
                bson_error_t error;
                bson_t *bson = bson_new_from_json((const uint8_t *)json, strlen(json), &error);
                if (bson) {
                    const uint8_t *bsons = bson_get_data(bson);
                    if (mqtt->connect)
                        if (mqtt->connect)
                            esp_mqtt_client_publish(mqtt->client, 
                                    mqtt->topicack,
                                    (const char *)bsons,
                                    bson->len,
                                    mqtt->msgQos, 0);
                }
#endif
                free(json);
            }
        }
        cJSON_Delete(root);
    }

    return 0;
}

int32_t MQTTMessageRecvUartGetTemperatureHandler(MQTT *mqtt, float temperature, int report) {
    cJSON *root = NULL;
    cJSON *sub  = NULL;

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            if (report)
                cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_ReportTemperature));
            else
                cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetTemperature));
            cJSON_AddNumberToObject(sub, "temperature", temperature);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect) {
                        esp_mqtt_client_publish(mqtt->client, 
                                mqtt->topicack,
                                (const char *)json,
                                strlen(json),
                                mqtt->msgQos, 0);
                    }
#else 
                bson_error_t error;
                bson_t *bson = bson_new_from_json((const uint8_t *)json, strlen(json), &error);
                if (bson) {
                    const uint8_t *bsons = bson_get_data(bson);
                    if (mqtt->connect)
                        if (mqtt->connect)
                            esp_mqtt_client_publish(mqtt->client, 
                                    mqtt->topicack,
                                    (const char *)bsons,
                                    bson->len,
                                    mqtt->msgQos, 0);
                }
#endif
                free(json);
            }
        }
        cJSON_Delete(root);
    }

    return 0;
}

int32_t MQTTMessageRecvEthHandler(MQTT *mqtt) {
    if (mqtt->recv) {
        while (1) {
            ModuleMessage message;
            int32_t length = sizeof(message);
            int status = mqtt->recv(gPriv, DataAttr_EthToMqtt, &message, &length, 0);
            if (!status) {
                if (message.attr == ModuleDataAttr_NetConnect) {
                    if (!mqtt->netFirstConnect) {
                        mqtt->netFirstConnect = 1;
                        esp_mqtt_client_start(mqtt->client);
                        LogPrintf(LogMQTT_Info, "NetConnect, Start Mqtt\n");
                    }
                }
                /*
                 * else if (message.ack.attr == ModuleDataAttr_Ack) {
                 *     switch (message.ack.srcAttr) {
                 *         case ModuleDataAttr_SetWifiCfg:
                 *             {
                 *                 return message.ack.status;
                 *             }
                 *         default:break;
                 *     }
                 * }
                 */
                else {

                }
            }
            else {
                break;
            }
        }
    }

    return 0;
}


unsigned char IntToHexChar(unsigned char c) {
    if (c > 9) return (c + 55);
    else return (c + 0x30);
}

static int c_regexFunction(const char *text, const char *pattern) {
    int match_length;

    /* LogPrintf(LogMQTT_Info, "text(%d):%s pattern(%d):%s", strlen(text), text, strlen(pattern), pattern); */
    if (re_match(pattern, text, &match_length) >= 0) {
        return 0;
    } else {
        return -1;
    }
}

#include <esp_heap_trace.h>
#define NUM_RECORDS 100
static heap_trace_record_t trace_record[NUM_RECORDS]; // This buffer must be in internal RAM
int32_t MQTTMessageRecvSpiHandler(MQTT *mqtt) {
#if 1
    int status = -1;
    int index = 0;
    char *sString = (char *)alloca(81);
    /* int num = 0; */
    /* static int allNum = 0; */
    /* LogPrintf(LogMQTT_Info, "%s %d\n", __func__, __LINE__); */

    /* heap_trace_start(HEAP_TRACE_LEAKS); */

    if (mqtt->recv) {
        if (mqtt->peek) {
            int32_t length = 0;
            Message *message = NULL;
            /* Message **pMessage = NULL; */
            /* char **pChar = NULL; */
            int messLen = 0;
            int messNum = 0;
            int allMessNum = 0;

            status = mqtt->peek(gPriv, DataAttr_SpiToMqtt, &message, &length, 0);
            ERRP(status, return -1, 0);

#define MessageNum (10)  //最大只发40个数据包，一次
            messLen = MessageNum;
            while (1) {
                status = mqtt->peek(gPriv, DataAttr_SpiToMqtt, &message, &length, 0);
                ERRP(status, return -1, 0);

                cJSON *root = NULL;
                cJSON *sub  = NULL;
                cJSON *array = NULL;
                allMessNum += messNum;
                ERRP(allMessNum >= MessageNum, return -1, 0);
                messNum = 0;


                root = cJSON_CreateArray();
                if (root) {
                    sub = cJSON_CreateObject();
                    if (sub) {
                        cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_TransmitData));
                        cJSON *psJSON = cJSON_CreateArray();
                        if (psJSON) {
                            while (1) {
                                //如果实际的数据单位大于messLen
                                ERRP (messNum >= messLen, break, 0);

                                status = mqtt->recv(gPriv, DataAttr_SpiToMqtt, &message, &length, 0);
                                ERRP(status, break, 0);

                                char devType[16];
                                snprintf (devType, sizeof(devType) - 1, "%d", ((char *)message->data)[8]);
                                /*0x88 88 88 88*/
                                /* memcpy(&(((char *)message->data)[8]), */
                                        /* &(((char *)message->data)[9]), 31);//数据为31字节，2个校验数据不要 */
                                /* (((char *)message->data)[39]) = '\0'; */
                                {
                                    unsigned char combine;
                                    unsigned char *inStr = (unsigned char *)message->data;
                                    int combineLen = 40;
                                    for (index = 0; index < combineLen; index++) {
                                        combine = inStr[index] & 0xf0;
                                        sString[2 * index] = IntToHexChar(combine >> 4);
                                        combine = inStr[index] & 0x0f;
                                        sString[2 * index + 1] = IntToHexChar(combine);
                                    }
                                    sString[combineLen * 2] = '\0';
                                    LogPrintf(LogMQTT_Detail, "before:%s\n", sString);
                                    memcpy(&sString[8 * 2], &sString[9 * 2], 31 * 2);
                                    sString[(combineLen - 1) * 2] = '\0';
                                    LogPrintf(LogMQTT_Detail, "after:%s\n", sString);
                                }

                                if (mqtt->isRegex) {
                                    //正则表达式生效中
                                    status = c_regexFunction(devType, mqtt->devType);
                                    if (status) {
                                        LogPrintf(LogMQTT_Warning, "DevType:%s Match:%s failure\n", devType, mqtt->devType);
                                        //设备类型没有匹配不成功
                                        if (mqtt->release) {
                                            status = mqtt->release(gPriv, DataAttr_SpiToMqtt, message);
                                        }
                                        continue;
                                    }
                                    status = c_regexFunction(sString, mqtt->regex);
                                    if (status) {
                                        LogPrintf(LogMQTT_Warning, "Message Match:%s failure\n", mqtt->regex);
                                        //数据相关信息匹配不成功
                                        if (mqtt->release) {
                                            status = mqtt->release(gPriv, DataAttr_SpiToMqtt, message);
                                        }
                                        continue;
                                    }
                                }

                                cJSON *psQJSON = cJSON_CreateObject();
                                if (psQJSON) {
                                    cJSON_AddNumberToObject(psQJSON, "timestamp", esp_log_early_timestamp());
                                    cJSON_AddStringToObject(psQJSON, "data", sString);
                                }
                                cJSON_AddItemToArray(psJSON, psQJSON);
                                messNum++;
                                if (mqtt->release) {
                                    status = mqtt->release(gPriv, DataAttr_SpiToMqtt, message);
                                }
                            }
                            cJSON_AddItemToObject(sub, "dataAdv", psJSON);
                            /*  [{
                             *      "htype" : "reportAdv",
                             *      "dataAdv" : [
                             *          {
                             *              "timestamp": 0
                             *              "data":"alsdfkjasdf"
                             *          },
                             *          {
                             *              "timestamp": 1
                             *              "data":"alsdfslkjasdf"
                             *          }
                             *      ]
                             *  }]
                             *
                             */
                            cJSON_AddItemToArray(root, sub);
                            char *json = cJSON_Print(root);
                            if (json) {
                                /* LogPrintf(LogMQTT_Info, "json(size:%d):\r\n%s\n", strlen(json), json); */
#ifndef USE_BSON
                                if (mqtt->connect)
                                    esp_mqtt_client_publish(mqtt->client, 
                                            mqtt->topicack,
                                            (const char *)json,
                                            strlen(json),
                                            mqtt->msgQos, 0);
#else 
                                bson_error_t error;
                                bson_t *bson = bson_new_from_json((const uint8_t *)json, strlen(json), &error);
                                if (bson) {
                                    const uint8_t *bsons = bson_get_data(bson);
                                    if (mqtt->connect)
                                        esp_mqtt_client_publish(mqtt->client, 
                                                mqtt->topicack,
                                                (const char *)bsons,
                                                bson->len,
                                                mqtt->msgQos, 0);
                                }
#endif
                                free(json);
                            }
                        }
                    }
                    cJSON_Delete(root);
                }
            }
        }
    }
    /* LogPrintf(LogMQTT_Info, "%s %d allNum:%d num:%d\n", __func__, __LINE__, allNum, num); */
    /* heap_trace_stop(); */
    /* heap_trace_dump(); */

    return status;
#else
    return 0;
#endif
}

int32_t MQTTMessageRecvUartHandler(MQTT *mqtt) {
    int status = -1;
    if (mqtt->recv) {
        while (1) {
            char recv[128];
            int32_t length = sizeof(recv);
            status = mqtt->recv(gPriv, DataAttr_UartToMqtt, recv, &length, 0);
            if (!status) {
                /*AT命令检测*/
                /* +TEMPERATURE:20 */
                recv[length] = '\0';
                LogPrintf(LogMQTT_Info, "recv:%s\n", recv);
                char *valid = strchr(recv, ':');
                if (valid) {
                    valid++;
                    recv[length] = '\0';
                    if (strstr(recv, "+TEMPERATUREUPDATE:")) {
                        /* LogPrintf(LogMQTT_Info, "uarttomqtt recv:%s\n", recv); */
                        //+TEMPERATURE:20?
                        float temperature = atof(valid);/*获取温度*/
                        status = MQTTMessageRecvUartGetTemperatureHandler(mqtt, temperature, 1);
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+TEMPERATURE:")) {
                        LogPrintf(LogMQTT_Info, "uarttomqtt recv:%s\n", recv);
                        float temperature = atof(valid);/*获取温度*/
                        status = MQTTMessageRecvUartGetTemperatureHandler(mqtt, temperature, 0);
                        if (!status) {
                            Message message;
                            message.attr = ModuleDataAttr_Cnt;
                            if (mqtt->send) {
                                /* status = mqtt->send(gPriv, DataAttr_MqttToUart, */
                                        /* &message, sizeof(message), 0); */
                            }
                        }
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+DEVINFO:")) {
                        char *version = valid;
                        status = MQTTMessageRecvUartGetModuleVersionHandler(mqtt, version);
                        mqtt->attr = ModuleDataAttr_Cnt;

                    }
                    else if (strstr(recv, "+USERINFO:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartGetModuleInfoHandler(mqtt, info, 0);
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+USERINFOUPDATE:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartGetModuleInfoHandler(mqtt, info, 1);
                        if (!status) {
                            Message message;
                            message.attr = ModuleDataAttr_Cnt;
                            if (mqtt->send) {
                                /* status = mqtt->send(gPriv, DataAttr_MqttToUart, */
                                        /* &message, sizeof(message), 0); */
                            }
                        }
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+POWER:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartGetPowerHandler(mqtt, info, 0);
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+POWERUPDATE:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartGetPowerHandler(mqtt, info, 1);
                        if (!status) {
                            Message message;
                            message.attr = ModuleDataAttr_Cnt;
                            if (mqtt->send) {
                                /* status = mqtt->send(gPriv, DataAttr_MqttToUart, */
                                        /* &message, sizeof(message), 0); */
                            }
                        }
                    }
                    else if (strstr(recv, "+OFFNOWUPDATE:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartRebootHandler(mqtt, info);
                        if (!status) {
                            Message message;
                            message.attr = ModuleDataAttr_Cnt;
                            if (mqtt->send) {
                                /* status = mqtt->send(gPriv, DataAttr_MqttToUart, */
                                        /* &message, sizeof(message), 0); */
                            }
                        }
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+ETHCFG:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartEthCfgHandler(mqtt, info, 0, 0);
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+ETHCFGUPDATE:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartEthCfgHandler(mqtt, info, 1, 1);
                        if (!status) {
                            Message message;
                            message.attr = ModuleDataAttr_Cnt;
                            if (mqtt->send) {
                                /* status = mqtt->send(gPriv, DataAttr_MqttToUart, */
                                        /* &message, sizeof(message), 0); */
                            }
                        }
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+WIFICFG:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartWifiCfgHandler(mqtt, info, 0, 0);
                    }
                    else if (strstr(recv, "+WIFICFGUPDATE:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartWifiCfgHandler(mqtt, info, 1, 1);
                        if (!status) {
                            Message message;
                            message.attr = ModuleDataAttr_Cnt;
                            if (mqtt->send) {
                                /* status = mqtt->send(gPriv, DataAttr_MqttToUart, */
                                        /* &message, sizeof(message), 0); */
                            }
                        }
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+MQTTCFG:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartMqttCfgHandler(mqtt, info, 0, 0);
                    }
                    else if (strstr(recv, "+MQTTCFGUPDATE:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartMqttCfgHandler(mqtt, info, 1, 1);
                        if (!status) {
                            Message message;
                            message.attr = ModuleDataAttr_Cnt;
                            if (mqtt->send) {
                                LogPrintf(LogMQTT_Info, "Mqtt Send Ack\n");
                                /* status = mqtt->send(gPriv, DataAttr_MqttToUart, */
                                        /* &message, sizeof(message), 0); */
                            }
                        }
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+PTSEND:")) {
                        status = MQTTMessageRecvUartCommonOkHandler(mqtt, "OK", ModuleDataAttr_PtSend);
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+PTRECV:")) {
                        status = MQTTMessageRecvUartPtRecvHandler(mqtt, valid, ModuleDataAttr_PtRecv);
                        if (!status) {
                            Message message;
                            message.attr = ModuleDataAttr_Cnt;
                            if (mqtt->send) {
                                status = mqtt->send(gPriv, DataAttr_MqttToUart,
                                        &message, sizeof(message), 0);
                            }
                        }
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+PTUPDATE")) {
                        status = MQTTMessageRecvUartPTUpdateHandle(mqtt, valid);
                        if (!status) {
                            Message message;
                            message.attr = ModuleDataAttr_Cnt;
                            if (mqtt->send) {
                                /* status = mqtt->send(gPriv, DataAttr_MqttToUart, */
                                        /* &message, sizeof(message), 0); */
                            }
                        }
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+SCANTIMEOUTUPDATE:")) {
                        status = MQTTMessageRecvUartSetScanTimeoutHandle(mqtt, valid, 1);
                        if (!status) {
                            Message message;
                            message.attr = ModuleDataAttr_Cnt;
                            if (mqtt->send) {
                                /* status = mqtt->send(gPriv, DataAttr_MqttToUart, */
                                        /* &message, sizeof(message), 0); */
                            }
                        }
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+SCANTIMEOUT:")) {
                        status = MQTTMessageRecvUartSetScanTimeoutHandle(mqtt, valid, 0);
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+REGEX:")) {
                        status = MQTTMessageRecvUartSetREGEXHandle(mqtt, valid, 0);
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+REGEXUPDATE:")) {
                        status = MQTTMessageRecvUartSetREGEXHandle(mqtt, valid, 1);
                        if (!status) {
                            Message message;
                            message.attr = ModuleDataAttr_Cnt;
                            if (mqtt->send) {
                                /* status = mqtt->send(gPriv, DataAttr_MqttToUart, */
                                        /* &message, sizeof(message), 0); */
                            }
                        }
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+DEBUGUPDATE:")) {
                        status = MQTTMessageRecvUartDEBUGHandle(mqtt, valid, 1);
                        if (!status) {
                            Message message;
                            message.attr = ModuleDataAttr_Cnt;
                            if (mqtt->send) {
                                /* status = mqtt->send(gPriv, DataAttr_MqttToUart, */
                                        /* &message, sizeof(message), 0); */
                            }
                        }
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                }
                else {
                    if (strstr(recv, "+OK")) {
                        /* char *info = valid; */
                        if (mqtt->attr == ModuleDataAttr_SetModuleInfo) {
                            //后面调整
                            MQTTMessageRecvUartCommonOkHandler(mqtt, "OK", ModuleDataAttr_SetModuleInfo);
                            mqtt->attr = ModuleDataAttr_Cnt;
                        }
                        else if (mqtt->attr == ModuleDataAttr_SetREGEX) {
                            MQTTMessageRecvUartCommonOkHandler(mqtt, "OK", ModuleDataAttr_SetREGEX);
                            mqtt->attr = ModuleDataAttr_Cnt;
                        }
                        else if (mqtt->attr == ModuleDataAttr_Reboot) {
                            //后面调整
                            MQTTMessageRecvUartRebootHandler(mqtt, "OK");
                            mqtt->attr = ModuleDataAttr_Cnt;
                        }
                        else if (mqtt->attr == ModuleDataAttr_NetState) {
                            //不用上报到上层，内部消息
                            /* MQTTMessageRecvUartCommonOkHandler(mqtt, "OK", ModuleDataAttr_NetState); */
                            /* mqtt->attr = ModuleDataAttr_Cnt; */
                        }
                        else if (mqtt->attr == ModuleDataAttr_SetEthCfg) {
                            //后面调整
                            MQTTMessageRecvUartCommonOkHandler(mqtt, "OK", ModuleDataAttr_SetEthCfg);
                            mqtt->attr = ModuleDataAttr_Cnt;
                        }
                        else if (mqtt->attr == ModuleDataAttr_SetWifiCfg) {
                            //后面调整
                            MQTTMessageRecvUartCommonOkHandler(mqtt, "OK", ModuleDataAttr_SetWifiCfg);
                            mqtt->attr = ModuleDataAttr_Cnt;
                        }
                        else if (mqtt->attr == ModuleDataAttr_SetMqttCfg) {
                            //后面调整
                            MQTTMessageRecvUartCommonOkHandler(mqtt, "OK", ModuleDataAttr_SetMqttCfg);
                            mqtt->attr = ModuleDataAttr_Cnt;
                        }
                        else if (mqtt->attr == ModuleDataAttr_SetScanTimeout) {
                            //后面调整
                            MQTTMessageRecvUartCommonOkHandler(mqtt, "OK", ModuleDataAttr_SetScanTimeout);
                            mqtt->attr = ModuleDataAttr_Cnt;
                        }
                    }
                    else if (strstr(recv, "+<OK>")) {
                        /* char *info = valid; */
                    }
                    else if (strstr(recv, "+<FULL>")) {
                        /* char *info = valid; */
                    }
                    /* else if (!strstr(recv, "")) */
                }
            }
            else {
                break;
            }
        }
    }

    return status;
}

double MQTTcJsonGetNumber(cJSON *root, int index, const char *htype) {
    cJSON *sub = cJSON_GetArrayItem(root, index);
    if (sub) {
        cJSON *elem = cJSON_GetObjectItem(sub, htype);
        if (elem) {
            if (elem->type == cJSON_Number) {
                return elem->valuedouble;
            }
            else return 0.0;
        }

    }

    return 0.0;
}

char *MQTTcJsonGetString(cJSON *root, int index, const char *htype) {
    cJSON *sub = cJSON_GetArrayItem(root, index);
    if (sub) {
        cJSON *elem = cJSON_GetObjectItem(sub, htype);
        if (elem) {
            if (elem->type == cJSON_String) {
                return elem->valuestring;
            }
            else return NULL;
        }

    }

    return NULL;
}

int32_t MQTTMessageSendHandlerTTUpdate(MQTT *mqtt, cJSON *root, int arraysize) {
    cJSON *json = root;
    if (json) {
        int index = 0;
        for (index = 1; index < arraysize; index++) {
            const char *url        = MQTTcJsonGetString(json, index, "url");
            LogPrintf(LogMQTT_Info, "url:%s\n", url);

            ModuleMessage message;
            message.attr = ModuleDataAttr_Update;
            strcpy(message.update.url, url);
            if (mqtt->send) {
                mqtt->send(gPriv, DataAttr_MqttToUpdate, &message, sizeof(message), 0);
                mqtt->attr = ModuleDataAttr_Update;
            }
        }
    }

    return 0;
}

int32_t MQTTMessageSendHandlerPTSend(MQTT *mqtt, cJSON *root, int arraysize) {
    int status = -1;
    cJSON *json = root;
    if (json) {
        int index = 0;
        for (index = 1; index < arraysize; index++) {
            const char *mac = MQTTcJsonGetString(json, index, "mac");
            const float seq = MQTTcJsonGetNumber(json, index, "seq");
            const char *address = MQTTcJsonGetString(json, index, "data");
            LogPrintf(LogMQTT_Info, "mac:%s\n", mac);
            LogPrintf(LogMQTT_Info, "seq:%f\n", seq);
            LogPrintf(LogMQTT_Info, "data:%s\n", address);
            ModuleMessage message;
            message.attr = ModuleDataAttr_PtSend;
            message.ptSend.seq = (int)seq;
            strcpy(message.ptSend.mac, mac);
            strcpy(message.ptSend.data, address);
            if (mqtt->send) {
                status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                if (!status) {
                    mqtt->attr = ModuleDataAttr_PtSend;
                }
            }
        }
    }
    return 0;
}

int32_t MQTTMessageSendHandlerGetScanTimeout(MQTT *mqtt, cJSON *root, int arraysize) {
    int status = -1;
    ModuleMessage message;
    message.attr = ModuleDataAttr_GetScanTimeout;
    if (mqtt->send) {
        status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
        if (!status) {
            mqtt->attr = ModuleDataAttr_GetScanTimeout;
        }
    }
    return 0;
}

int32_t isValidRegex(MQTT *mqtt, char *data) {
    /*GZ248:[0,2];23*/
    char *ptr = (char *)data;
    char *devType = NULL;
    char *regex = NULL;
    /*GZ248:[0,2];23*/
    char *sptr = strchr(ptr, ':');
    if (sptr) {
        /* :[0,2];23 */
        *sptr = '\0';
        /*\0[0,2];23*/
        sptr = sptr + 1;
        /*[0,2];23*/
        if (!strcmp(ptr, "GZ248")) {
            /*GZ248有效*/
            ptr = strchr(sptr, ';');
            if (ptr) {
                *ptr = '\0';
                ptr = ptr + 1;
                devType = sptr;
                regex = ptr;
                strcpy(mqtt->devType, devType);
                strcpy(mqtt->regex, regex);
                mqtt->isRegex = 1;
                LogPrintf(LogMQTT_Info, "regex isvalid(%d), initRegex:%s "
                        "devType:%s regex:%s\n", 
                        mqtt->isRegex, data, mqtt->devType, mqtt->regex);
                return 0;
            }
        }
    }

    mqtt->isRegex = 0;
    LogPrintf(LogMQTT_Info, "regex isvalid(%d), initRegex:%s\n", 
            mqtt->isRegex, data);

    return -1;
}

int32_t MQTTMessageSendHandlerSetREGEX(MQTT *mqtt, const char *data) {
    int32_t status = -1;
    ModuleMessage message;
    char *sString = alloca(strlen(data) + 1);
    if (sString) {
        strcpy(sString, data);

        isValidRegex(mqtt, sString);
        message.attr = ModuleDataAttr_SetREGEX;
        strcpy(message.setREGEX.data, (char *)data);
        if (mqtt->send) {
            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
            if (!status) {
                mqtt->attr = ModuleDataAttr_SetREGEX;
                return 0;
            }
        }
    }

    return -1;
}

int32_t MQTTMessageSendHandlerSetScanTimeout(MQTT *mqtt, cJSON *root, int arraysize) {
    int status = -1;
    cJSON *json = root;
    if (json) {
        int index = 0;
        for (index = 1; index < arraysize; index++) {
            const float sec = MQTTcJsonGetNumber(json, index, "sec");
            LogPrintf(LogMQTT_Info, "sec:%f\n", sec);
            ModuleMessage message;
            message.attr = ModuleDataAttr_SetScanTimeout;
            message.setScanTimeout.sec = (int)sec;
            if (mqtt->send) {
                status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                if (!status) {
                    mqtt->attr = ModuleDataAttr_SetScanTimeout;
                }
            }
        }
    }
    return 0;
}

int32_t MQTTMessageSendHandlerSetMqttCfg(MQTT *mqtt, cJSON *root, int arraysize) {
    return 0;
}


int32_t MQTTMessageSendHandlerGetMqttCfg(MQTT *mqtt, cJSON *root, int arraysize) {
    int status = -1;
    ModuleMessage message;
    message.attr = ModuleDataAttr_GetMqttCfg;
    if (mqtt->send) {
        status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
        if (!status) {
            mqtt->attr = ModuleDataAttr_GetMqttCfg;
        }
    }
    return 0;
}

int32_t MQTTMessageSendHandlerSetWifiCfg(MQTT *mqtt, cJSON *root, int arraysize) {
    int status = -1;
    cJSON *json = root;
    if (json) {
        int index = 0;
        for (index = 1; index < arraysize; index++) {
            const char *ssid = MQTTcJsonGetString(json, index, "ssid");
            const char *password = MQTTcJsonGetString(json, index, "password");
            const char *address = MQTTcJsonGetString(json, index, "address");
            const char *netmask = MQTTcJsonGetString(json, index, "netmask");
            const char *gateway = MQTTcJsonGetString(json, index, "gateway");
            LogPrintf(LogMQTT_Info, "ssid:%s\n", ssid);
            LogPrintf(LogMQTT_Info, "passwd:%s\n", password);
            LogPrintf(LogMQTT_Info, "address:%s\n", address);
            LogPrintf(LogMQTT_Info, "netmask:%s\n", netmask);
            LogPrintf(LogMQTT_Info, "gateway:%s\n", gateway);
            ModuleMessage message;
            message.attr = ModuleDataAttr_SetWifiCfg;
            strcpy(message.setWifiCfg.ssid, ssid);
            strcpy(message.setWifiCfg.password, password);
            strcpy(message.setWifiCfg.address, address);
            strcpy(message.setWifiCfg.netmask, netmask);
            strcpy(message.setWifiCfg.gateway, gateway);
            if (mqtt->send) {
                status = mqtt->send(gPriv, DataAttr_MqttToWifi, &message, sizeof(message), 0);
                if (!status) {
                    mqtt->attr = ModuleDataAttr_SetWifiCfg;
                }
                else {
                    MQTTMessageRecvUartCommonOkHandler(mqtt, "NO", ModuleDataAttr_SetWifiCfg);
                }
            }
        }
    }
    return 0;
}


int32_t MQTTMessageSendHandlerGetWifiCfg(MQTT *mqtt, cJSON *root, int arraysize) {
    int status = -1;
    ModuleMessage message;
    message.attr = ModuleDataAttr_GetWifiCfg;
    if (mqtt->send) {
        status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
        if (!status) {
            mqtt->attr = ModuleDataAttr_GetWifiCfg;
        }
    }
    return 0;
}

int32_t MQTTMessageSendHandlerSetEthCfg(MQTT *mqtt, cJSON *root, int arraysize) {
    int status = -1;
    cJSON *json = root;
    if (json) {
        int index = 0;
        for (index = 1; index < arraysize; index++) {
            const char *address = MQTTcJsonGetString(json, index, "address");
            const char *netmask = MQTTcJsonGetString(json, index, "netmask");
            const char *gateway = MQTTcJsonGetString(json, index, "gateway");
            LogPrintf(LogMQTT_Info, "address:%s\n", address);
            LogPrintf(LogMQTT_Info, "netmask:%s\n", netmask);
            LogPrintf(LogMQTT_Info, "gateway:%s\n", gateway);

            ModuleMessage message;
            message.attr = ModuleDataAttr_SetEthCfg;
            strcpy(message.setEthCfg.address, address);
            strcpy(message.setEthCfg.netmask, netmask);
            strcpy(message.setEthCfg.gateway, gateway);
            if (mqtt->send) {
                status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                if (!status) {
                    mqtt->attr = ModuleDataAttr_SetEthCfg;
                }
            }
        }
    }
    return 0;
}

int32_t MQTTMessageSendHandlerGetEthCfg(MQTT *mqtt, cJSON *root, int arraysize) {
    int status = -1;
    ModuleMessage message;
    message.attr = ModuleDataAttr_GetEthCfg;
    if (mqtt->send) {
        status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
        if (!status) {
            mqtt->attr = ModuleDataAttr_GetEthCfg;
        }
    }
    return 0;
}

int32_t MQTTMessageSendHandlerNetState(MQTT *mqtt, cJSON *root, int arraysize) {
    /*
     * int status = -1;
     * cJSON *json = root;
     * if (json) {
     *     int index = 0;
     *     for (index = 1; index < arraysize; index++) {
     *         ModuleMessage message;
     *         message.attr = ModuleDataAttr_NetState;
     *         message.netState.state = NetState_CONNET;//后面修改
     *         if (mqtt->send) {
     *             status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
     *             if (!status) {
     *                 mqtt->attr = ModuleDataAttr_NetState;
     *             }
     *         }
     *     }
     * }
     */
    return 0;
}

int32_t MQTTMessageSendHandlerReboot(MQTT *mqtt, cJSON *root, int arraysize) {
    int status = -1;
    ModuleMessage message;
    message.attr = ModuleDataAttr_Reboot;
    if (mqtt->send) {
        status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
        if (!status) {
            mqtt->attr = ModuleDataAttr_Reboot;
        }
    }
    return 0;
}

int32_t MQTTMessageSendHandlerGetPower(MQTT *mqtt, cJSON *root, int arraysize) {
    int status = -1;
    ModuleMessage message;
    message.attr = ModuleDataAttr_GetPower;
    if (mqtt->send) {
        status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
        if (!status) {
            mqtt->attr = ModuleDataAttr_GetPower;
        }
    }
    return 0;
}

int32_t MQTTMessageSendHandlerSetModuleInfo(MQTT *mqtt, cJSON *root, int arraysize) {
    int status = -1;
    cJSON *json = root;
    if (json) {
        int index = 0;
        for (index = 1; index < arraysize; index++) {
            const char *info = MQTTcJsonGetString(json, index, "info");
            LogPrintf(LogMQTT_Info, "info:%s\n", info);

            ModuleMessage message;
            message.attr = ModuleDataAttr_SetModuleInfo;
            strcpy(message.setModuleInfo.info, info);
            if (mqtt->send) {
                status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                if (!status) {
                    mqtt->attr = ModuleDataAttr_SetModuleInfo;
                }
            }
        }
    }
    return 0;
}

int32_t MQTTMessageSendHandlerGetModuleInfo(MQTT *mqtt, cJSON *root, int arraysize) {
    int status = -1;
    ModuleMessage message;
    message.attr = ModuleDataAttr_GetModuleInfo;
    if (mqtt->send) {
        status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
        if (!status) {
            mqtt->attr = ModuleDataAttr_GetModuleInfo;
        }
    }
    return 0;
}

int32_t MQTTMessageSendHandlerGetModuleVersion(MQTT *mqtt, cJSON *root, int arraysize) {
    int status = -1;
    ModuleMessage message;
    message.attr = ModuleDataAttr_GetModuleVersion;
    if (mqtt->send) {
        status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
        if (!status) {
            mqtt->attr = ModuleDataAttr_GetModuleVersion;
        }
    }
    return 0;
}

int32_t MQTTMessageSendHandlerGetTemperature(MQTT *mqtt, cJSON *root, int arraysize) {
    int status = -1;
    LogPrintf(LogMQTT_Info, "arraysize:%d\n", arraysize);
    ModuleMessage message;
    message.attr = ModuleDataAttr_GetTemperature;
    if (mqtt->send) {
        status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
        if (!status) {
            /* MQTTMessageRecvUartHandler(mqtt); */
            mqtt->attr = ModuleDataAttr_GetTemperature;
        }
    }
    return 0;
}

int32_t MQTTMessageSendHandler(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    int32_t status = -1;

    const char *response = (char *)event->data;
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    LogPrintf(LogMQTT_Info, "htype:%s arraysize:%d\n", strings, arraysize);
                    if (!strcmp(strings, "CpuUpdate")) {
                        /* MQTTMessageSendHandlerTTUpdate(mqtt, json, arraysize); */
                        const char *url        = MQTTcJsonGetString(json, index, "url");
                        LogPrintf(LogMQTT_Info, "url:%s\n", url);
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_Update;
                        strcpy(message.update.url, url);
                        if (mqtt->send) {
                            mqtt->send(gPriv, DataAttr_MqttToUpdate, &message, sizeof(message), 0);
                            mqtt->attr = ModuleDataAttr_Update;
                        }
                    }
                    else if (!strcmp(strings, "PTSEND")) {
                        /* MQTTMessageSendHandlerPTSend(mqtt, json, arraysize); */
                        const char *mac = MQTTcJsonGetString(json, index, "mac");
                        const float seq = MQTTcJsonGetNumber(json, index, "seq");
                        const char *address = MQTTcJsonGetString(json, index, "data");
                        LogPrintf(LogMQTT_Info, "mac:%s\n", mac);
                        LogPrintf(LogMQTT_Info, "seq:%f\n", seq);
                        LogPrintf(LogMQTT_Info, "data:%s\n", address);
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_PtSend;
                        message.ptSend.seq = (int)seq;
                        strcpy(message.ptSend.mac, mac);
                        strcpy(message.ptSend.data, address);
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_PtSend;
                            }
                        }
                    }
                    else if (!strcmp(strings, "GetScanTimeout")) {
                        /* MQTTMessageSendHandlerGetScanTimeout(mqtt, json, arraysize); */
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetScanTimeout;
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_GetScanTimeout;
                            }
                        }
                    }
                    else if (!strcmp(strings, "SetScanTimeout")) {
                        /* MQTTMessageSendHandlerSetScanTimeout(mqtt, json, arraysize); */
                        const float sec = MQTTcJsonGetNumber(json, index, "sec");
                        LogPrintf(LogMQTT_Info, "sec:%f\n", sec);
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_SetScanTimeout;
                        message.setScanTimeout.sec = (int)sec;
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_SetScanTimeout;
                            }
                        }
                    }
                    else if (!strcmp(strings, "GetRegex")) {
                        /* MQTTMessageSendHandlerGetScanTimeout(mqtt, json, arraysize); */
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetREGEX;
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_GetREGEX;
                            }
                        }
                    }
                    else if (!strcmp(strings, "SetRegex")) {
                        const char *data = MQTTcJsonGetString(json, index, "data");
                        MQTTMessageSendHandlerSetREGEX(mqtt, data);
                        LogPrintf(LogMQTT_Info, "data:%s\n", data);
                        /*
                         * ModuleMessage message;
                         * message.attr = ModuleDataAttr_SetREGEX;
                         * strcpy(message.setREGEX.data, (char *)data);
                         * strcpy(mqtt->regex, (char *)data);
                         * if (mqtt->send) {
                         *     status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                         *     if (!status) {
                         *         mqtt->attr = ModuleDataAttr_SetREGEX;
                         *     }
                         * }
                         */
                    }
                    else if (!strcmp(strings, "SetMqttCfg")) {
                        /* MQTTMessageSendHandlerSetMqttCfg(mqtt, json, arraysize); */
                        const char *user = MQTTcJsonGetString(json, index, "user");
                        const char *password = MQTTcJsonGetString(json, index, "password");
                        const char *url = MQTTcJsonGetString(json, index, "url");
                        const char *clientid = MQTTcJsonGetString(json, index, "clientid");
                        const short port = MQTTcJsonGetNumber(json, index, "port");
                        LogPrintf(LogMQTT_Info, "user:%s\n", user);
                        LogPrintf(LogMQTT_Info, "passwd:%s\n", password);
                        LogPrintf(LogMQTT_Info, "url:%s\n", url);
                        LogPrintf(LogMQTT_Info, "port:%d\n", port);
                        LogPrintf(LogMQTT_Info, "clientid:%s\n", clientid);
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_SetMqttCfg;
                        strcpy(message.setMqttCfg.user, user);
                        strcpy(message.setMqttCfg.password, password);
                        strcpy(message.setMqttCfg.url, url);
                        strcpy(message.setMqttCfg.clientid, clientid);
                        message.setMqttCfg.port = port;
                        if (port) {
                            //如果端口不为0
                            snprintf (message.setMqttCfg.url, sizeof(message.setMqttCfg.url) - 1, 
                                    "%s:%d", url, port);
                            message.setMqttCfg.url[sizeof(message.setMqttCfg.url) - 1] = '\0';
                        }

                        if (mqtt->send) {
                            /* esp_mqtt_client_config_t mqttConfig; */
                            /* memset(&mqttConfig, 0x0, sizeof(mqttConfig)); */
                            strcpy(mqtt->mqttConfig.broker.address.uri, message.setMqttCfg.url);
                            strcpy(mqtt->mqttConfig.credentials.username, message.setMqttCfg.user);
                            strcpy(mqtt->mqttConfig.credentials.client_id, message.setMqttCfg.clientid);
                            strcpy(mqtt->mqttConfig.credentials.authentication.password, message.setMqttCfg.password);
                            esp_mqtt_client_unsubscribe(mqtt->client, mqtt->topic);
                            snprintf (mqtt->topic, sizeof(mqtt->topic) - 1, 
                                    "GZ248_%02x%02x%02x%02x%02x%02x_%s", 
                                    mqtt->mac[0], mqtt->mac[1], mqtt->mac[2], 
                                    mqtt->mac[3], mqtt->mac[4], mqtt->mac[5], message.setMqttCfg.clientid);
                            snprintf (mqtt->topicack, sizeof(mqtt->topicack) - 1, 
                                    "GZ248_%02x%02x%02x%02x%02x%02x_%sAck", 
                                    mqtt->mac[0], mqtt->mac[1], mqtt->mac[2], 
                                    mqtt->mac[3], mqtt->mac[4], mqtt->mac[5], message.setMqttCfg.clientid);
                            MQTTMessageStopMqtt(mqtt);
                            MQTTMessageStartMqtt(mqtt);

                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_SetMqttCfg;
                            }
                            else {
                                MQTTMessageRecvUartCommonOkHandler(mqtt, "NO", ModuleDataAttr_SetMqttCfg);
                            }
                        }
                    }
                    else if (!strcmp(strings, "GetMqttCfg")) {
                        /* MQTTMessageSendHandlerGetMqttCfg(mqtt, json, arraysize); */
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetMqttCfg;
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_GetMqttCfg;
                            }
                        }
                    }
                    else if (!strcmp(strings, "SetWifiCfg")) {
                        /* MQTTMessageSendHandlerSetWifiCfg(mqtt, json, arraysize); */
                        const char *ssid = MQTTcJsonGetString(json, index, "ssid");
                        const char *password = MQTTcJsonGetString(json, index, "password");
                        const char *address = MQTTcJsonGetString(json, index, "address");
                        const char *netmask = MQTTcJsonGetString(json, index, "netmask");
                        const char *gateway = MQTTcJsonGetString(json, index, "gateway");
                        LogPrintf(LogMQTT_Info, "ssid:%s\n", ssid);
                        LogPrintf(LogMQTT_Info, "passwd:%s\n", password);
                        LogPrintf(LogMQTT_Info, "address:%s\n", address);
                        LogPrintf(LogMQTT_Info, "netmask:%s\n", netmask);
                        LogPrintf(LogMQTT_Info, "gateway:%s\n", gateway);
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_SetWifiCfg;
                        strcpy(message.setWifiCfg.ssid, ssid);
                        strcpy(message.setWifiCfg.password, password);
                        strcpy(message.setWifiCfg.address, address);
                        strcpy(message.setWifiCfg.netmask, netmask);
                        strcpy(message.setWifiCfg.gateway, gateway);
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToWifi, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_SetWifiCfg;
                            }
                            else {
                                MQTTMessageRecvUartCommonOkHandler(mqtt, "NO", ModuleDataAttr_SetWifiCfg);
                            }
                        }
                    }
                    else if (!strcmp(strings, "GetWifiCfg")) {
                        /* MQTTMessageSendHandlerGetWifiCfg(mqtt, json, arraysize); */
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetWifiCfg;
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_GetWifiCfg;
                            }
                        }
                    }
                    else if (!strcmp(strings, "SetEthCfg")) {
                        /* MQTTMessageSendHandlerSetEthCfg(mqtt, json, arraysize); */
                        const char *address = MQTTcJsonGetString(json, index, "address");
                        const char *netmask = MQTTcJsonGetString(json, index, "netmask");
                        const char *gateway = MQTTcJsonGetString(json, index, "gateway");
                        LogPrintf(LogMQTT_Info, "address:%s\n", address);
                        LogPrintf(LogMQTT_Info, "netmask:%s\n", netmask);
                        LogPrintf(LogMQTT_Info, "gateway:%s\n", gateway);

                        ModuleMessage message;
                        message.attr = ModuleDataAttr_SetEthCfg;
                        strcpy(message.setEthCfg.address, address);
                        strcpy(message.setEthCfg.netmask, netmask);
                        strcpy(message.setEthCfg.gateway, gateway);
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_SetEthCfg;
                            }
                        }
                    }
                    else if (!strcmp(strings, "GetEthCfg")) {
                        /* MQTTMessageSendHandlerGetEthCfg(mqtt, json, arraysize); */
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetEthCfg;
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_GetEthCfg;
                            }
                        }
                    }
                    else if (!strcmp(strings, "NetState")) {
                        /* MQTTMessageSendHandlerNetState(mqtt, json, arraysize); */
                    }
                    else if (!strcmp(strings, "Reboot")) {
                        /* MQTTMessageSendHandlerReboot(mqtt, json, arraysize); */
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_Reboot;
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_Reboot;
                            }
                        }
                    }
                    else if (!strcmp(strings, "GetPower")) {
                        /* MQTTMessageSendHandlerGetPower(mqtt, json, arraysize); */
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetPower;
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_GetPower;
                            }
                        }
                    }
                    else if (!strcmp(strings, "SetUserInfo")) {
                        /* MQTTMessageSendHandlerSetModuleInfo(mqtt, json, arraysize); */
                        const char *info = MQTTcJsonGetString(json, index, "info");
                        LogPrintf(LogMQTT_Info, "info:%s\n", info);

                        ModuleMessage message;
                        message.attr = ModuleDataAttr_SetModuleInfo;
                        strcpy(message.setModuleInfo.info, info);
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_SetModuleInfo;
                            }
                        }
                    }
                    else if (!strcmp(strings, "GetUserInfo")) {
                        /* MQTTMessageSendHandlerGetModuleInfo(mqtt, json, arraysize); */
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetModuleInfo;
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_GetModuleInfo;
                            }
                        }
                    }
                    else if (!strcmp(strings, "GetModuleVersion")) {
                        /* MQTTMessageSendHandlerGetModuleVersion(mqtt, json, arraysize); */
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetModuleVersion;
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_GetModuleVersion;
                            }
                        }
                    }
                    else if (!strcmp(strings, "GetTemperature")) {
                        /* MQTTMessageSendHandlerGetTemperature(mqtt, json, arraysize); */
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetTemperature;
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                /* MQTTMessageRecvUartHandler(mqtt); */
                                mqtt->attr = ModuleDataAttr_GetTemperature;
                            }
                        }
                    }
                }
            }
            cJSON_Delete(json);
        }
    }

    return 0;
}

int32_t MQTTMessageSubscribeList(MQTT *mqtt) {
    esp_mqtt_client_subscribe(
            mqtt->client, 
            mqtt->topic, /* toEnumString((ModuleDataAttr)index), */
            mqtt->msgQos);//消息必需成功???

    return 0;
}

static void timer_cb(void *arg) {
    MQTT *mqtt = (MQTT *)arg;

/*
 *     esp_mqtt_event_t event;
 *     memset(&event, 0x0, sizeof(event));
 *     event.event_id  = MQTT_USER_EVENT;
 * 
 *     esp_mqtt_dispatch_custom_event(mqtt->client, &event);
 */


    ModuleMessage message;
    message.attr = ModuleInternalDataAttr_TriggerRecv;
    esp_event_post_to(mqtt->event, MYMQTT_EVENT, 
            MYMQTTERNET_EVENT_START, &message, sizeof(message), 0);
}

void MQTTNetEventHandler(void *handler_args, esp_event_base_t base,
        int32_t event_id, void *event_data) {
    esp_err_t status                = ESP_FAIL;        
    MQTT *mqtt = (MQTT *)handler_args;

    LogPrintf(LogMQTT_Debug, "Event dispatched from event loop base=%s, event_id=%" PRIi32 " CPUID:%d\n", base, event_id, xPortGetCoreID());

    /* int msg_id; */
    esp_mqtt_event_handle_t event   = event_data;

    switch (/* (esp_mqtt_event_id_t) */event_id) {
        case MYMQTTERNET_EVENT_START:
            {
                /*任何消息前先处理消息数据*/
                //只处理触发mqtt start
                MQTTMessageRecvWifiHandler(mqtt);
                MQTTMessageRecvEthHandler(mqtt);
                MQTTMessageRecvSpiHandler(mqtt);
                MQTTMessageRecvUartHandler(mqtt);
                break;
            }
        case MYMQTTERNET_EVENT_STOPMQTT:
            {
                LogPrintf(LogMQTT_Info, "StopMqtt\n");
                esp_mqtt_client_stop(mqtt->client);
                esp_mqtt_set_config(mqtt->client, &mqtt->mqttConfig);
                break;
            }
        case MYMQTTERNET_EVENT_STARTMQTT:
            {
                LogPrintf(LogMQTT_Info, "StartMqtt\n");
                esp_mqtt_client_start(mqtt->client);
                esp_mqtt_client_subscribe(mqtt->client, mqtt->topic, mqtt->msgQos);
                break;
            }
        default:
            {
                LogPrintf(LogMQTT_Info, "[%s]Other event id:%d\n", mqtt->name, event->event_id);
                break;
            }
    }
}

void MQTTEventHandler(void *handler_args, esp_event_base_t base,
        int32_t event_id, void *event_data) {
    esp_err_t status                = ESP_FAIL;        
    MQTT *mqtt = (MQTT *)handler_args;

    LogPrintf(LogMQTT_Debug, "Event dispatched from event loop base=%s, event_id=%" PRIi32 " CPUID:%d\n", base, event_id, xPortGetCoreID());

    /* int msg_id; */
    esp_mqtt_event_handle_t event   = event_data;

    switch (/* (esp_mqtt_event_id_t) */event_id) {
        case MQTT_EVENT_CONNECTED:
            {
                mqtt->connect = 1;
                LogPrintf(LogMQTT_Info, "[%s]MQTT_EVENT_CONNECTED\n", mqtt->name);
                MQTTMessageSubscribeList(mqtt);
                ModuleMessage message;
                message.attr = ModuleDataAttr_NetState;
                message.netState._state = _NetState_MqttConnect;//后面修改
                /* strcpy(message.setmoduleInfo.info, info); */
                if (mqtt->send) {
                    if (mqtt->checkStatus == 1) {
                        mqtt->checkStatus = 0;
                        ModuleMessage message;
                        message.reportDebug.attr = ModuleDataAttr_ReportDebug;
                        strcpy(message.reportDebug.data, "checkmqtt:OK");
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_EthToUart, &message, sizeof(message), 0);
                        }
                    }
                }
                if (mqtt->send) {
                    status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                    if (!status) {
                        mqtt->attr = ModuleDataAttr_NetState;
                        /* MQTTMessageRecvUartHandler(mqtt); */
                    }
                }
                break;
            }
        case MQTT_EVENT_DISCONNECTED:
            {
                mqtt->connect = 0;
                /* esp_mqtt_client_unsubscribe(mqtt->client, mqtt->topic); */
                /* esp_mqtt_client_reconnect(mqtt->client); */
                /* esp_mqtt_client_subscribe(mqtt->client, mqtt->topic, mqtt->msgQos); */
                LogPrintf(LogMQTT_Info, "[%s]MQTT_EVENT_DISCONNECTED\n", mqtt->name);
                ModuleMessage message;
                message.attr = ModuleDataAttr_NetState;
                message.netState._state = _NetState_MqttUnconnect;//后面修改
                /* strcpy(message.setmoduleInfo.info, info); */
                if (mqtt->send) {
                    status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                    if (!status) {
                        mqtt->attr = ModuleDataAttr_NetState;
                        /* MQTTMessageRecvUartHandler(mqtt); */
                    }
                }
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
                LogPrintf(LogMQTT_Info, "[%s]MQTT_EVENT_DATA mgs_id:%d\n", mqtt->name, event->msg_id);
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
                break;
            }
        default:
            {
                LogPrintf(LogMQTT_Info, "[%s]Other event id:%d\n", mqtt->name, event->event_id);
                break;
            }
    }
}

int32_t MQTTEthTriggerRecv(void *arg) {
    MQTT *mqtt = (MQTT *)arg;

    ModuleMessage message;
    message.attr = ModuleInternalDataAttr_TriggerRecv;
    esp_event_post_to(mqtt->event, MYMQTT_EVENT, 
            MYMQTTERNET_EVENT_START, &message, sizeof(message), 0);

    return 0;
}

int32_t MQTTUartTriggerRecv(void *arg) {
    MQTT *mqtt = (MQTT *)arg;

    ModuleMessage message;
    message.attr = ModuleInternalDataAttr_TriggerRecv;
    esp_event_post_to(mqtt->event, MYMQTT_EVENT, 
            MYMQTTERNET_EVENT_START, &message, sizeof(message), 0);

    return 0;
}

int32_t MQTTSpiTriggerRecv(void *arg) {
    /* MQTT *mqtt = (MQTT *)arg; */

/*
 *     esp_mqtt_event_t event;
 *     memset(&event, 0x0, sizeof(event));
 *     event.event_id  = MQTT_USER_EVENT;
 * 
 *     return esp_mqtt_dispatch_custom_event(mqtt->client, &event);
 *     [> printf ("MQTTUARTEVnetID:%d status:%d\n", event.event_id, status); <]
 */

    return 0;
}

int32_t MQTTWifiTriggerRecv(void *arg) {
    MQTT *mqtt = (MQTT *)arg;

    LogPrintf(LogMQTT_Info, "%s %d\n", __func__, __LINE__);
    ModuleMessage message;
    message.attr = ModuleInternalDataAttr_TriggerRecv;
    LogPrintf(LogMQTT_Info, "%s %d\n", __func__, __LINE__);
    esp_event_post_to(mqtt->event, MYMQTT_EVENT, 
            MYMQTTERNET_EVENT_START, &message, sizeof(message), 0);
    LogPrintf(LogMQTT_Info, "%s %d\n", __func__, __LINE__);

    return 0;
}

void *MQTTInit(MQTTConfig *config) {
    /* esp_mqtt_client_config_t mqttConfig  = { }; */

    MQTT *mqtt = (MQTT *) malloc (sizeof(*mqtt));
    ERRP(NULL == mqtt, return NULL, 1, "malloc MQTT Instance failure\n");
    memset(mqtt, 0x0, sizeof(*mqtt));

    mqtt->send      = config->send;
    mqtt->recv      = config->recv;
    mqtt->peek      = config->peek;
    mqtt->request   = config->request;
    mqtt->release   = config->release;
    mqtt->msgQos    = 0;
    mqtt->dataQos   = 0;
    //测试
    /* mqtt->isRegex   = 1; */
    /* strcpy(mqtt->devType, "9"); */
    /* strcpy(mqtt->regex, "*"); */
    //测试结束

    snprintf (mqtt->name, sizeof(mqtt->name) - 1, "mqtt");

    strcpy(mqtt->mac, config->mac);

    snprintf (mqtt->topic, sizeof(mqtt->topic) - 1, 
            "GZ248_%02x%02x%02x%02x%02x%02x_%s", 
            mqtt->mac[0], mqtt->mac[1], mqtt->mac[2], 
            mqtt->mac[3], mqtt->mac[4], mqtt->mac[5], config->clientId);
    snprintf (mqtt->topicack, sizeof(mqtt->topicack) - 1, 
            "GZ248_%02x%02x%02x%02x%02x%02x_%sAck", 
            mqtt->mac[0], mqtt->mac[1], mqtt->mac[2], 
            mqtt->mac[3], mqtt->mac[4], mqtt->mac[5], config->clientId); 
    /* snprintf (mqtt->topic, sizeof(mqtt->topic) - 1, "%s", config->topic); */
    /* snprintf (mqtt->topicack, sizeof(mqtt->topicack) - 1, "%sAck", config->topic); */

    strcpy(mqtt->url, config->url);
    strcpy(mqtt->clientid, config->clientId);
    strcpy(mqtt->username, config->user);
    strcpy(mqtt->password, config->password);

    mqtt->mqttConfig.credentials.username = mqtt->username;
    mqtt->mqttConfig.credentials.client_id = mqtt->clientid;
    mqtt->mqttConfig.credentials.authentication.password = mqtt->password;
    mqtt->mqttConfig.broker.address.uri = mqtt->url;
    /* mqtt://192.168.0.102:1883 */
    mqtt->mqttConfig.network.reconnect_timeout_ms = 1000;
    mqtt->mqttConfig.network.timeout_ms = 1000;
    LogPrintf(LogMQTT_Info, "clientId:%s\n", mqtt->mqttConfig.credentials.client_id);
    LogPrintf(LogMQTT_Info, "user:%s\n", mqtt->mqttConfig.credentials.username);
    LogPrintf(LogMQTT_Info, "password:%s\n", mqtt->mqttConfig.credentials.authentication.password);
    LogPrintf(LogMQTT_Info, "url:%s\n", mqtt->mqttConfig.broker.address.uri);
    LogPrintf(LogMQTT_Info, "topic:%s\n", mqtt->topic);
    LogPrintf(LogMQTT_Info, "topicack:%s\n", mqtt->topicack);
    /* mqtt->mqttConfig.network.refresh_connection_after_ms = 1000; */
    mqtt->client = esp_mqtt_client_init(&mqtt->mqttConfig);
    ERRP(NULL == mqtt->client, goto ERR5, 1, "mqtt esp_mqtt_client_init failure\n");

    esp_mqtt_client_register_event(mqtt->client, 
            MQTT_EVENT_ANY, MQTTEventHandler, mqtt);

    const esp_timer_create_args_t timer_args = {
        timer_cb,
        mqtt,
        ESP_TIMER_TASK,
        "wifi_timer",
        true,
    };
    esp_timer_create(&timer_args, &mqtt->timer);
    esp_timer_start_periodic(mqtt->timer, 40000);//10000);//10ms

    esp_event_loop_args_t loop_args = {
        .queue_size = CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE,
        .task_name = "mqttSub",
        .task_stack_size = ESP_TASKD_EVENT_STACK,
        .task_priority = ESP_TASKD_EVENT_PRIO,
        .task_core_id = 0
    };
    int status = esp_event_loop_create(&loop_args, &mqtt->event);
    ERRP(ESP_OK != status, goto ERR5, 1, 
            "update esp_event_loop_create failure\n");

    LogPrintf(LogMQTT_Info, "%s %d\n", __func__, __LINE__);
    esp_event_handler_register_with(mqtt->event, MYMQTT_EVENT, MYMQTT_EVENT_MQTT_ANY_ID, MQTTNetEventHandler, mqtt);
    LogPrintf(LogMQTT_Info, "%s %d\n", __func__, __LINE__);
    /* esp_mqtt_client_start(mqtt->client); */

    /* heap_trace_init_standalone(trace_record, NUM_RECORDS); */
    
    /* mqtt->connect = 1; */

    return mqtt;
ERR5:
    free(mqtt);
    return NULL;
}

