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

static void *gPriv = NULL;
static MQTTPrint gPrint;
static LogMQTT gLevel = LogMQTT_Info;


/*************************************************/
/**************LogPrintf**************************/
/*************************************************/



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
    int32_t isRunning;
    int32_t connect;

    esp_mqtt_client_handle_t client;
    esp_timer_handle_t timer;

    MQTTSigSend send;
    MQTTSigRecv recv;
    MQTTSigRequest request;
    MQTTSigRelease release;

    int32_t msgQos;
    int32_t dataQos;

    char name[32];

    char version[32];
    char info[32];

    PowerSupplyMode mode;
    int32_t level;

    char username[32];
    char password[32];

    char url[128];

    ModuleDataAttr attr;
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
            cJSON_AddStringToObject(sub, "htype", toAckEnumString(ModuleDataAttr_GetWifiCfg));
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
                                toAckEnumString(message->attr), 
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
                                    toAckEnumString(message->attr), 
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
                if (message.ack.attr == ModuleDataAttr_Ack) {
                    switch (message.ack.srcAttr) {
                        case ModuleDataAttr_SetWifiCfg:
                            {
                                return message.ack.status;
                            }
                        default:break;
                    }
                }
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

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            cJSON_AddStringToObject(sub, "htype", toAckEnumString(ModuleDataAttr_GetModuleVersion));
            cJSON_AddStringToObject(sub, "ver", version);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                toAckEnumString(ModuleDataAttr_GetModuleVersion), 
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
                                    toAckEnumString(ModuleDataAttr_GetModuleVersion), 
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
    int32_t status = ESP_FAIL;
    cJSON *root = NULL;
    cJSON *sub  = NULL;

    if ((attr == ModuleDataAttr_SetWifiCfg) 
            || (attr == ModuleDataAttr_SetEthCfg)) {
        status = esp_mqtt_client_reconnect(mqtt->client);
    }

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            cJSON_AddStringToObject(sub, "htype", toAckEnumString(attr));
            cJSON_AddStringToObject(sub, "status", isOk);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                toAckEnumString(attr), 
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
                                    toAckEnumString(attr), 
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

    /*send OK to module*/
    return 0;
}

int32_t MQTTMessageRecvUartMqttCfgHandler(MQTT *mqtt, char *info, int32_t isUp) {
    cJSON *root             = NULL;
    cJSON *sub              = NULL;
    char *user              = NULL;
    char *password          = NULL;
    char *url               = NULL;

    printf ("info:%s\n", info);
    char *sptr = strchr(info, '<');
    if (sptr) {
        char *ptr = strchr(sptr + 1, '>');
        if (ptr) {
            *ptr = '\0';
            user = sptr + 1;

            sptr = strchr(ptr + 1, '<');
            if (ptr) {
                ptr = strchr(sptr + 1, '>');
                if (ptr) {
                    *ptr = '\0';
                    password = sptr + 1;

                    sptr = strchr(ptr + 1, '<');
                    if (ptr) {
                        ptr = strchr(sptr + 1, '>');
                        if (ptr) {
                            *ptr = '\0';
                            url = sptr + 1;
                        }
                    }
                }
            }
        }
    }

    if (isUp) {
        esp_mqtt_client_config_t mqttConfig;
        memset(&mqttConfig, 0x0, sizeof(mqttConfig));
        mqttConfig.broker.address.uri                   = url;
        mqttConfig.credentials.username                 = user;
        mqttConfig.credentials.authentication.password  = password;
        int32_t status = esp_mqtt_set_config(mqtt->client, &mqttConfig);
        if (!status) {
            status = esp_mqtt_client_reconnect(mqtt->client);
        }
    }

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            cJSON_AddStringToObject(sub, "htype", toAckEnumString(ModuleDataAttr_GetMqttCfg));
            cJSON_AddStringToObject(sub, "user", user);
            cJSON_AddStringToObject(sub, "password", password);
            cJSON_AddStringToObject(sub, "url", url);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                toAckEnumString(ModuleDataAttr_GetMqttCfg), 
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
                                    toAckEnumString(ModuleDataAttr_GetMqttCfg), 
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

int32_t MQTTMessageRecvUartWifiCfgHandler(MQTT *mqtt, char *info, int32_t isUp) {
    cJSON *root             = NULL;
    cJSON *sub              = NULL;
    /* PowerSupplyMode mode    = PowerCnt; */
    char *ssid              = NULL;
    char *password          = NULL;
    char *address           = NULL;
    char *netmask           = NULL;
    char *gateway           = NULL;

    char *sptr = strchr(info, '<');
    if (sptr) {
        char *ptr = strchr(sptr + 1, '>');
        if (ptr) {
            *ptr = '\0';
            ssid = sptr + 1;

            sptr = strchr(ptr + 1, '<');
            if (ptr) {
                ptr = strchr(sptr + 1, '>');
                if (ptr) {
                    *ptr = '\0';
                    password = sptr + 1;

                    sptr = strchr(ptr + 1, '<');
                    if (ptr) {
                        ptr = strchr(sptr + 1, '>');
                        if (ptr) {
                            *ptr = '\0';
                            address = sptr + 1;

                            sptr = strchr(ptr + 1, '<');
                            if (ptr) {
                                ptr = strchr(sptr + 1, '>');
                                if (ptr) {
                                    *ptr = '\0';
                                    netmask = sptr + 1;

                                    sptr = strchr(ptr + 1, '<');
                                    if (ptr) {
                                        ptr = strchr(sptr + 1, '>');
                                        if (ptr) {
                                            *ptr = '\0';
                                            gateway = sptr + 1;
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

    if (isUp) {
        ModuleMessage message;
        message.attr = ModuleDataAttr_SetWifiCfg;
        strcpy(message.setWifiCfg.ssid, ssid);
        strcpy(message.setWifiCfg.password, password);
        strcpy(message.setWifiCfg.address, address);
        strcpy(message.setWifiCfg.netmask, netmask);
        strcpy(message.setWifiCfg.gateway, gateway);
        if (mqtt->send) {
            /* int32_t status =  */mqtt->send(gPriv, DataAttr_MqttToWifi, &message, sizeof(message), 0);
            /* if (!status) { */
                /* status = MQTTMessageRecvWifiHandler(mqtt); */
            /* } */
        }
    }

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            cJSON_AddStringToObject(sub, "htype", toAckEnumString(ModuleDataAttr_GetWifiCfg));
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
                                toAckEnumString(ModuleDataAttr_GetWifiCfg), 
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
                                    toAckEnumString(ModuleDataAttr_GetWifiCfg), 
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

int32_t MQTTMessageRecvUartEthCfgHandler(MQTT *mqtt, char *info) {
    cJSON *root             = NULL;
    cJSON *sub              = NULL;
    /* PowerSupplyMode mode    = PowerCnt; */
    /* char *sptr              = NULL; */
    char *address           = NULL;
    char *netmask           = NULL;
    char *gateway           = NULL;

    char *sptr = strchr(info, '<');
    if (sptr) {
        char *ptr = strchr(sptr + 1, '>');
        if (ptr) {
            *ptr = '\0';
            address = sptr + 1;

            sptr = strchr(ptr + 1, '<');
            if (ptr) {
                ptr = strchr(sptr + 1, '>');
                if (ptr) {
                    *ptr = '\0';
                    netmask = sptr + 1;

                    sptr = strchr(ptr + 1, '<');
                    if (ptr) {
                        ptr = strchr(sptr + 1, '>');
                        if (ptr) {
                            *ptr = '\0';
                            gateway = sptr + 1;
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
            cJSON_AddStringToObject(sub, "htype", toAckEnumString(ModuleDataAttr_GetEthCfg));
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
                                toAckEnumString(ModuleDataAttr_GetEthCfg), 
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
                                    toAckEnumString(ModuleDataAttr_GetEthCfg), 
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

int32_t MQTTMessageRecvUartGetPowerHandler(MQTT *mqtt, char *info) {
    cJSON *root             = NULL;
    cJSON *sub              = NULL;
    int32_t power           = 0;;
    PowerSupplyMode mode    = PowerCnt;

    printf ("GetPower:%s\n", info);
    /* GetPower:<DC>,<100> */

    char *ptr = strchr(info, '<');
    if (ptr) {
        info = ptr + 1;
        ptr = strchr(ptr + 1, '>');
        if (ptr) {
            *ptr = '\0';
            printf ("info:%s\n", info);
            if (!strcmp(info, "DC")) {
                mode = DC;
            }
            else if (!strcmp(info, "BAT")) {
                mode = BAT;
            }
            else {
                mode = PowerCnt;
            }

            printf ("ptr:%s\n", ptr);
            ptr = strchr(ptr + 1, '<');
            if (ptr) {
                power = atoi(ptr + 1);
            }
        }
    }

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            cJSON_AddStringToObject(sub, "htype", toAckEnumString(ModuleDataAttr_GetPower));
            cJSON_AddNumberToObject(sub, "mode", mode);
            cJSON_AddNumberToObject(sub, "battery_level", power);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                toAckEnumString(ModuleDataAttr_GetPower), 
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
                                    toAckEnumString(ModuleDataAttr_GetPower), 
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

int32_t MQTTMessageRecvUartSetModuleInfoHandler(MQTT *mqtt, char *info) {
    cJSON *root = NULL;
    cJSON *sub  = NULL;

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            cJSON_AddStringToObject(sub, "htype", toAckEnumString(ModuleDataAttr_SetModuleInfo));
            cJSON_AddStringToObject(sub, "info", info);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                toAckEnumString(ModuleDataAttr_SetModuleInfo), 
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
                                    toAckEnumString(ModuleDataAttr_SetModuleInfo), 
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

int32_t MQTTMessageRecvUartGetModuleInfoHandler(MQTT *mqtt, char *info) {
    cJSON *root = NULL;
    cJSON *sub  = NULL;

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            cJSON_AddStringToObject(sub, "htype", toAckEnumString(ModuleDataAttr_GetModuleInfo));
            cJSON_AddStringToObject(sub, "info", info);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect)
                        esp_mqtt_client_publish(mqtt->client, 
                                toAckEnumString(ModuleDataAttr_GetModuleInfo), 
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
                                    toAckEnumString(ModuleDataAttr_GetModuleInfo), 
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

int32_t MQTTMessageRecvUartGetTemperatureHandler(MQTT *mqtt, float temperature) {
    cJSON *root = NULL;
    cJSON *sub  = NULL;

    root = cJSON_CreateArray();
    if (root) {
        sub = cJSON_CreateObject();
        if (sub) {
            cJSON_AddStringToObject(sub, "htype", toAckEnumString(ModuleDataAttr_GetTemperature));
            cJSON_AddNumberToObject(sub, "temperature", temperature);
            cJSON_AddItemToArray(root, sub);
            char *json = cJSON_Print(root);
            if (json) {
                LogPrintf(LogMQTT_Info, "json:\r\n%s\n", json);
#ifndef USE_BSON
                if (mqtt->connect)
                    if (mqtt->connect) {
                        LogPrintf(LogMQTT_Info, "...............................")
                            esp_mqtt_client_publish(mqtt->client, 
                                    toAckEnumString(ModuleDataAttr_GetTemperature), 
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
                                    toAckEnumString(ModuleDataAttr_GetTemperature), 
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
    return 0;
}

#include <esp_heap_trace.h>
#define NUM_RECORDS 100
static heap_trace_record_t trace_record[NUM_RECORDS]; // This buffer must be in internal RAM
int32_t MQTTMessageRecvSpiHandler(MQTT *mqtt) {
#if 1
    int status = -1;
    int num = 0;
    static int allNum = 0;
    /* LogPrintf(LogMQTT_Info, "%s %d\n", __func__, __LINE__); */

    /* heap_trace_start(HEAP_TRACE_LEAKS); */

    if (mqtt->recv) {
        while (1) {
            char recv[128];
            int32_t length = 0;
            Message *message = NULL;
            status = mqtt->recv(gPriv, DataAttr_SpiToMqtt, &message, &length, 0);
            ERRP(status, break, 0);

            /* num++; */
            /* allNum++; */

            /*
             * int32_t jindex = 0;
             * printf("Received: ");
             * for (jindex = 0; jindex < 41; jindex++) {
             *     printf("%02x ", ((char *)message->data)[jindex]);//recvbuf);
             * }
             * printf ("\n");
             */
            /*send to MQTT SubScri*/
            cJSON *root = NULL;
            cJSON *sub  = NULL;

            root = cJSON_CreateArray();
            if (root) {
                sub = cJSON_CreateObject();
                if (sub) {
                    cJSON_AddStringToObject(sub, "htype", toAckEnumString(ModuleDataAttr_ReportData));
                    /* cJSON_AddStringToObject(sub, "data", (char *)"1234567890abcdefghijkopkalsdf"); */
                    cJSON_AddStringToObject(sub, "data", (char *)message->data);
                    cJSON_AddItemToArray(root, sub);
                    char *json = cJSON_Print(root);
                    if (json) {
                        LogPrintf(LogMQTT_Debug, "json:\r\n%s\n", json);
#ifndef USE_BSON
                        if (mqtt->connect)
                            esp_mqtt_client_publish(mqtt->client, 
                                    toAckEnumString(ModuleDataAttr_ReportData), 
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
                                        toAckEnumString(ModuleDataAttr_ReportData), 
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

            if (mqtt->release) {
                status = mqtt->release(gPriv, DataAttr_SpiToMqtt, message);
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
                LogPrintf(LogMQTT_Info, "uarttomqtt recv:%s\n", recv);
                char *valid = strchr(recv, ':');
                if (valid) {
                    valid++;
                    recv[length] = '\0';
                    if (strstr(recv, "+TEMPERATUREUPDATE:")) {
                        //+TEMPERATURE:20?
                        float temperature = atof(valid);/*获取温度*/
                        status = MQTTMessageRecvUartGetTemperatureHandler(mqtt, temperature);
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+TEMPERATURE:")) {
                        float temperature = atof(valid);/*获取温度*/
                        status = MQTTMessageRecvUartGetTemperatureHandler(mqtt, temperature);
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
                    else if (strstr(recv, "+VER:")) {
                        char *version = valid;
                        status = MQTTMessageRecvUartGetModuleVersionHandler(mqtt, version);
                        mqtt->attr = ModuleDataAttr_Cnt;

                    }
                    else if (strstr(recv, "+INFO:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartGetModuleInfoHandler(mqtt, info);
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+INFOUPDATE:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartSetModuleInfoHandler(mqtt, info);
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
                    else if (strstr(recv, "+POWER:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartGetPowerHandler(mqtt, info);
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+POWERUPDATE:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartGetPowerHandler(mqtt, info);
                        if (!status) {
                            Message message;
                            message.attr = ModuleDataAttr_Cnt;
                            if (mqtt->send) {
                                status = mqtt->send(gPriv, DataAttr_MqttToUart,
                                        &message, sizeof(message), 0);
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
                                status = mqtt->send(gPriv, DataAttr_MqttToUart,
                                        &message, sizeof(message), 0);
                            }
                        }
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+ETHCFG:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartEthCfgHandler(mqtt, info);
                        mqtt->attr = ModuleDataAttr_Cnt;
                    }
                    else if (strstr(recv, "+ETHCFGUPDATE:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartEthCfgHandler(mqtt, info);
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
                    else if (strstr(recv, "+WIFICFG:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartWifiCfgHandler(mqtt, info, 0);
                    }
                    else if (strstr(recv, "+WIFICFGUPDATE:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartWifiCfgHandler(mqtt, info, 1);
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
                    else if (strstr(recv, "+MQTTCFG:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartMqttCfgHandler(mqtt, info, 0);
                    }
                    else if (strstr(recv, "+MQTTCFGUPDATE:")) {
                        char *info = valid;
                        status = MQTTMessageRecvUartMqttCfgHandler(mqtt, info, 1);
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
                }
                else {
                    if (strstr(recv, "+OK")) {
                        /* char *info = valid; */
                        if (mqtt->attr == ModuleDataAttr_SetModuleInfo) {
                            //后面调整
                            MQTTMessageRecvUartCommonOkHandler(mqtt, "OK", ModuleDataAttr_SetModuleInfo);
                            mqtt->attr = ModuleDataAttr_Cnt;
                        }
                        else if (mqtt->attr == ModuleDataAttr_Reboot) {
                            //后面调整
                            MQTTMessageRecvUartRebootHandler(mqtt, "OK");
                            mqtt->attr = ModuleDataAttr_Cnt;
                        }
                        else if (mqtt->attr == ModuleDataAttr_NetState) {
                            //后面调整
                            MQTTMessageRecvUartCommonOkHandler(mqtt, "OK", ModuleDataAttr_NetState);
                            mqtt->attr = ModuleDataAttr_Cnt;
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
                        /* else { */

                        /* } */
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

int32_t MQTTMessageSendHandlerTTUpdate(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    const char *response = NULL;
#ifdef USE_BSON
    bson_t *bson = bson_new_from_data((uint8_t *)event->data, event->data_len);
    if (bson) {
        size_t length = 0;
        response =  bson_as_json(bson, &length);
        LogPrintf(LogMQTT_Info, "message(%d) bson(%d)->json(%d):%s\n",
                sizeof(ModuleMessageUpdate), bson->len, strlen(response), response);
    }
#else
    response = (char *)event->data; /*json*/
#endif
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    if (!strcmp(strings, "Update")) {
                        const char *url        = MQTTcJsonGetString(json, index, "url");
                        LogPrintf(LogMQTT_Info, "htype:%s\n", strings);
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
            }
            cJSON_Delete(json);
        }
    }
    return 0;
}

int32_t MQTTMessageSendHandlerPTSend(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    int status              = -1;
    const char *response    = NULL;
#ifdef USE_BSON
    bson_t *bson = bson_new_from_data((uint8_t *)event->data, event->data_len);
    if (bson) {
        size_t length = 0;
        response =  bson_as_json(bson, &length);
        LogPrintf(LogMQTT_Info, "message(%d) bson(%d)->json(%d):%s\n",
                sizeof(ModuleMessageUpdate), bson->len, strlen(response), response);
    }
#else
    response = (char *)event->data; /*json*/
#endif
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    if (!strcmp(strings, "PtSend")) {
                        const float timeOut = MQTTcJsonGetNumber(json, index, "timeOut");
                        const char *mac = MQTTcJsonGetString(json, index, "mac");
                        const char *address = MQTTcJsonGetString(json, index, "data");
                        /* LogPrintf(LogMQTT_Info, "htype:%s\n", strings); */
                        /* LogPrintf(LogMQTT_Info, "info:%f\n", info); */
                        LogPrintf(LogMQTT_Info, "htype:%s\n", strings);
                        LogPrintf(LogMQTT_Info, "timeOut:%f\n", timeOut);
                        LogPrintf(LogMQTT_Info, "mac:%s\n", mac);
                        LogPrintf(LogMQTT_Info, "data:%s\n", address);
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_PtSend;
                        message.ptSend.timeOut = (int)timeOut;
                        strcpy(message.ptSend.mac, mac);
                        strcpy(message.ptSend.data, address);
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_PtSend;
                                /* MQTTMessageRecvUartHandler(mqtt); */
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

int32_t MQTTMessageSendHandlerSetMqttCfg(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    int status              = 0;
    const char *response    = NULL;
#ifdef USE_BSON
    bson_t *bson = bson_new_from_data((uint8_t *)event->data, event->data_len);
    if (bson) {
        size_t length = 0;
        response =  bson_as_json(bson, &length);
        LogPrintf(LogMQTT_Info, "message(%d) bson(%d)->json(%d):%s\n",
                sizeof(ModuleMessageUpdate), bson->len, strlen(response), response);
    }
#else
    response = (char *)event->data; /*json*/
#endif
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    if (!strcmp(strings, "SetMqttCfg")) {
                        const char *user = MQTTcJsonGetString(json, index, "user");
                        const char *password = MQTTcJsonGetString(json, index, "password");
                        const char *url = MQTTcJsonGetString(json, index, "url");
                        /* LogPrintf(LogMQTT_Info, "htype:%s\n", strings); */
                        /* LogPrintf(LogMQTT_Info, "info:%f\n", info); */
                        LogPrintf(LogMQTT_Info, "htype:%s\n", strings);
                        LogPrintf(LogMQTT_Info, "user:%s\n", user);
                        LogPrintf(LogMQTT_Info, "passwd:%s\n", password);
                        LogPrintf(LogMQTT_Info, "url:%s\n", url);
                        ModuleMessage message;
                        message.attr = ModuleDataAttr_SetMqttCfg;
                        strcpy(message.setMqttCfg.user, user);
                        strcpy(message.setMqttCfg.password, password);
                        strcpy(message.setMqttCfg.url, url);
                        if (mqtt->send) {
                            esp_mqtt_client_config_t mqttConfig;
                            memset(&mqttConfig, 0x0, sizeof(mqttConfig));
                            mqttConfig.broker.address.uri                   = message.setMqttCfg.url;
                            mqttConfig.credentials.username                 = message.setMqttCfg.user;
                            mqttConfig.credentials.authentication.password  = message.setMqttCfg.password;
                            status = esp_mqtt_set_config(mqtt->client, &mqttConfig);
                            if (!status) {
                                status = esp_mqtt_client_reconnect(mqtt->client);
                                if (!status) {
                                    status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                                    if (!status) {
                                        mqtt->attr = ModuleDataAttr_SetMqttCfg;
                                        /* MQTTMessageRecvUartHandler(mqtt); */
                                    }
                                    else {
                                        MQTTMessageRecvUartCommonOkHandler(mqtt, "NO", ModuleDataAttr_SetMqttCfg);
                                    }
                                }
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


int32_t MQTTMessageSendHandlerGetMqttCfg(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    int status              = 0;
    const char *response    = NULL;
#ifdef USE_BSON
    bson_t *bson = bson_new_from_data((uint8_t *)event->data, event->data_len);
    if (bson) {
        size_t length = 0;
        response =  bson_as_json(bson, &length);
        LogPrintf(LogMQTT_Info, "message(%d) bson(%d)->json(%d):%s\n",
                sizeof(ModuleMessageUpdate), bson->len, strlen(response), response);
    }
#else
    response = (char *)event->data; /*json*/
#endif
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    if (!strcmp(strings, "GetMqttCfg")) {
                        /* const char *info = MQTTcJsonGetString(json, index, ""); */
                        /* LogPrintf(LogMQTT_Info, "htype:%s\n", strings); */
                        /* LogPrintf(LogMQTT_Info, "info:%f\n", info); */

                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetMqttCfg;
                        /* message.netState.state = NetState_CONNET;//后面修改 */
                        /* strcpy(message.setmoduleInfo.info, info); */
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_GetMqttCfg;
                                /* MQTTMessageRecvUartHandler(mqtt); */
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

int32_t MQTTMessageSendHandlerSetWifiCfg(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    int status              = 0;
    const char *response    = NULL;
#ifdef USE_BSON
    bson_t *bson = bson_new_from_data((uint8_t *)event->data, event->data_len);
    if (bson) {
        size_t length = 0;
        response =  bson_as_json(bson, &length);
        LogPrintf(LogMQTT_Info, "message(%d) bson(%d)->json(%d):%s\n",
                sizeof(ModuleMessageUpdate), bson->len, strlen(response), response);
    }
#else
    response = (char *)event->data; /*json*/
#endif
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    if (!strcmp(strings, "SetWifiCfg")) {
                        const char *ssid = MQTTcJsonGetString(json, index, "ssid");
                        const char *password = MQTTcJsonGetString(json, index, "password");
                        const char *address = MQTTcJsonGetString(json, index, "address");
                        const char *netmask = MQTTcJsonGetString(json, index, "netmask");
                        const char *gateway = MQTTcJsonGetString(json, index, "gateway");
                        /* LogPrintf(LogMQTT_Info, "htype:%s\n", strings); */
                        /* LogPrintf(LogMQTT_Info, "info:%f\n", info); */
                        LogPrintf(LogMQTT_Info, "htype:%s\n", strings);
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
                                /*
                                 * status = MQTTMessageRecvWifiHandler(mqtt);
                                 * if (!status) {
                                 *     status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                                 *     if (!status) {
                                 *         [> MQTTMessageRecvUartHandler(mqtt); <]
                                 *     }
                                 *     else {
                                 *         MQTTMessageRecvUartCommonOkHandler(mqtt, "NO", ModuleDataAttr_SetWifiCfg);
                                 *     }
                                 * }
                                 * else {
                                 *     MQTTMessageRecvUartCommonOkHandler(mqtt, "NO", ModuleDataAttr_SetWifiCfg);
                                 * }
                                 */
                            }
                            else {
                                MQTTMessageRecvUartCommonOkHandler(mqtt, "NO", ModuleDataAttr_SetWifiCfg);
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


int32_t MQTTMessageSendHandlerGetWifiCfg(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    int status              = 0;
    const char *response    = NULL;
#ifdef USE_BSON
    bson_t *bson = bson_new_from_data((uint8_t *)event->data, event->data_len);
    if (bson) {
        size_t length = 0;
        response =  bson_as_json(bson, &length);
        LogPrintf(LogMQTT_Info, "message(%d) bson(%d)->json(%d):%s\n",
                sizeof(ModuleMessageUpdate), bson->len, strlen(response), response);
    }
#else
    response = (char *)event->data; /*json*/
#endif
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    if (!strcmp(strings, "GetWifiCfg")) {
                        /* const char *info = MQTTcJsonGetString(json, index, ""); */
                        /* LogPrintf(LogMQTT_Info, "htype:%s\n", strings); */
                        /* LogPrintf(LogMQTT_Info, "info:%f\n", info); */

                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetWifiCfg;
                        /* message.netState.state = NetState_CONNET;//后面修改 */
                        /* strcpy(message.setmoduleInfo.info, info); */
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_GetWifiCfg;
                                /* MQTTMessageRecvUartHandler(mqtt); */
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

int32_t MQTTMessageSendHandlerSetEthCfg(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    int status              = 0;
    const char *response    = NULL;
#ifdef USE_BSON
    bson_t *bson = bson_new_from_data((uint8_t *)event->data, event->data_len);
    if (bson) {
        size_t length = 0;
        response =  bson_as_json(bson, &length);
        LogPrintf(LogMQTT_Info, "message(%d) bson(%d)->json(%d):%s\n",
                sizeof(ModuleMessageUpdate), bson->len, strlen(response), response);
    }
#else
    response = (char *)event->data; /*json*/
#endif
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    if (!strcmp(strings, "SetEthCfg")) {
                        const char *address = MQTTcJsonGetString(json, index, "address");
                        const char *netmask = MQTTcJsonGetString(json, index, "netmask");
                        const char *gateway = MQTTcJsonGetString(json, index, "gateway");
                        /* LogPrintf(LogMQTT_Info, "htype:%s\n", strings); */
                        /* LogPrintf(LogMQTT_Info, "info:%f\n", info); */
                        LogPrintf(LogMQTT_Info, "htype:%s\n", strings);
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
                                /* MQTTMessageRecvUartHandler(mqtt); */
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

int32_t MQTTMessageSendHandlerGetEthCfg(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    int status              = 0;
    const char *response    = NULL;
#ifdef USE_BSON
    bson_t *bson = bson_new_from_data((uint8_t *)event->data, event->data_len);
    if (bson) {
        size_t length = 0;
        response =  bson_as_json(bson, &length);
        LogPrintf(LogMQTT_Info, "message(%d) bson(%d)->json(%d):%s\n",
                sizeof(ModuleMessageUpdate), bson->len, strlen(response), response);
    }
#else
    response = (char *)event->data; /*json*/
#endif
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    if (!strcmp(strings, "GetEthCfg")) {
                        /* const char *info = MQTTcJsonGetString(json, index, ""); */
                        /* LogPrintf(LogMQTT_Info, "htype:%s\n", strings); */
                        /* LogPrintf(LogMQTT_Info, "info:%f\n", info); */

                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetEthCfg;
                        /* message.netState.state = NetState_CONNET;//后面修改 */
                        /* strcpy(message.setmoduleInfo.info, info); */
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_GetEthCfg;
                                /* MQTTMessageRecvUartHandler(mqtt); */
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

int32_t MQTTMessageSendHandlerNetState(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    int status              = 0;
    const char *response    = NULL;
#ifdef USE_BSON
    bson_t *bson = bson_new_from_data((uint8_t *)event->data, event->data_len);
    if (bson) {
        size_t length = 0;
        response =  bson_as_json(bson, &length);
        LogPrintf(LogMQTT_Info, "message(%d) bson(%d)->json(%d):%s\n",
                sizeof(ModuleMessageUpdate), bson->len, strlen(response), response);
    }
#else
    response = (char *)event->data; /*json*/
#endif
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    if (!strcmp(strings, "NetState")) {
                        /* const char *info = MQTTcJsonGetString(json, index, ""); */
                        /* LogPrintf(LogMQTT_Info, "htype:%s\n", strings); */
                        /* LogPrintf(LogMQTT_Info, "info:%f\n", info); */

                        ModuleMessage message;
                        message.attr = ModuleDataAttr_NetState;
                        message.netState.state = NetState_CONNET;//后面修改
                        /* strcpy(message.setmoduleInfo.info, info); */
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_NetState;
                                /* MQTTMessageRecvUartHandler(mqtt); */
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

int32_t MQTTMessageSendHandlerReboot(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    int status              = 0;
    const char *response    = NULL;
#ifdef USE_BSON
    bson_t *bson = bson_new_from_data((uint8_t *)event->data, event->data_len);
    if (bson) {
        size_t length = 0;
        response =  bson_as_json(bson, &length);
        LogPrintf(LogMQTT_Info, "message(%d) bson(%d)->json(%d):%s\n",
                sizeof(ModuleMessageUpdate), bson->len, strlen(response), response);
    }
#else
    response = (char *)event->data; /*json*/
#endif
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    if (!strcmp(strings, "Reboot")) {
                        /* const char *info = MQTTcJsonGetString(json, index, ""); */
                        /* LogPrintf(LogMQTT_Info, "htype:%s\n", strings); */
                        /* LogPrintf(LogMQTT_Info, "info:%f\n", info); */

                        ModuleMessage message;
                        message.attr = ModuleDataAttr_Reboot;
                        /* strcpy(message.setmoduleInfo.info, info); */
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_Reboot;
                                /* MQTTMessageRecvUartHandler(mqtt); */
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

int32_t MQTTMessageSendHandlerGetPower(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    int status              = 0;
    const char *response    = NULL;
#ifdef USE_BSON
    bson_t *bson = bson_new_from_data((uint8_t *)event->data, event->data_len);
    if (bson) {
        size_t length = 0;
        response =  bson_as_json(bson, &length);
        LogPrintf(LogMQTT_Info, "message(%d) bson(%d)->json(%d):%s\n",
                sizeof(ModuleMessageUpdate), bson->len, strlen(response), response);
    }
#else
    response = (char *)event->data; /*json*/
#endif
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    if (!strcmp(strings, "GetPower")) {
                        /* const char *info = MQTTcJsonGetString(json, index, ""); */
                        /* LogPrintf(LogMQTT_Info, "htype:%s\n", strings); */
                        /* LogPrintf(LogMQTT_Info, "info:%f\n", info); */

                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetPower;
                        /* strcpy(message.setmoduleInfo.info, info); */
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_GetPower;
                                /* MQTTMessageRecvUartHandler(mqtt); */
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

int32_t MQTTMessageSendHandlerSetModuleInfo(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    int status              = 0;
    const char *response    = NULL;
#ifdef USE_BSON
    bson_t *bson = bson_new_from_data((uint8_t *)event->data, event->data_len);
    if (bson) {
        size_t length = 0;
        response =  bson_as_json(bson, &length);
        LogPrintf(LogMQTT_Info, "message(%d) bson(%d)->json(%d):%s\n",
                sizeof(ModuleMessageUpdate), bson->len, strlen(response), response);
    }
#else
    response = (char *)event->data; /*json*/
#endif
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    if (!strcmp(strings, "SetModuleInfo")) {
                        const char *info = MQTTcJsonGetString(json, index, "info");
                        LogPrintf(LogMQTT_Info, "htype:%s\n", strings);
                        LogPrintf(LogMQTT_Info, "info:%s\n", info);

                        ModuleMessage message;
                        message.attr = ModuleDataAttr_SetModuleInfo;
                        strcpy(message.setModuleInfo.info, info);
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_SetModuleInfo;
                                /* MQTTMessageRecvUartHandler(mqtt); */
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

int32_t MQTTMessageSendHandlerGetModuleInfo(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    int status              = 0;
    const char *response    = NULL;
#ifdef USE_BSON
    bson_t *bson = bson_new_from_data((uint8_t *)event->data, event->data_len);
    if (bson) {
        size_t length = 0;
        response =  bson_as_json(bson, &length);
        LogPrintf(LogMQTT_Info, "message(%d) bson(%d)->json(%d):%s\n",
                sizeof(ModuleMessageUpdate), bson->len, strlen(response), response);
    }
#else
    response = (char *)event->data; /*json*/
#endif
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    if (!strcmp(strings, "GetModuleInfo")) {
                        /* const float temperature = MQTTcJsonGetNumber(json, index, "temperature"); */
                        LogPrintf(LogMQTT_Info, "htype:%s\n", strings);
                        /* LogPrintf(LogMQTT_Info, "temperature:%f\n", temperature); */

                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetModuleInfo;
                        /* message.getTemperature.temperature = temperature; */
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_GetModuleInfo;
                                /* MQTTMessageRecvUartHandler(mqtt); */
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

int32_t MQTTMessageSendHandlerGetModuleVersion(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    int status              = 0;
    const char *response    = NULL;
#ifdef USE_BSON
    bson_t *bson = bson_new_from_data((uint8_t *)event->data, event->data_len);
    if (bson) {
        size_t length = 0;
        response =  bson_as_json(bson, &length);
        LogPrintf(LogMQTT_Info, "message(%d) bson(%d)->json(%d):%s\n",
                sizeof(ModuleMessageUpdate), bson->len, strlen(response), response);
    }
#else
    response = (char *)event->data; /*json*/
#endif
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    if (!strcmp(strings, "GetModuleVersion")) {
                        /* const float temperature = MQTTcJsonGetNumber(json, index, "temperature"); */
                        LogPrintf(LogMQTT_Info, "htype:%s\n", strings);
                        /* LogPrintf(LogMQTT_Info, "temperature:%f\n", temperature); */

                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetModuleVersion;
                        /* message.getTemperature.temperature = temperature; */
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                mqtt->attr = ModuleDataAttr_GetModuleVersion;
                                /* MQTTMessageRecvUartHandler(mqtt); */
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

int32_t MQTTMessageSendHandlerGetTemperature(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    int status              = 0;
    const char *response    = NULL;
#ifdef USE_BSON
    bson_t *bson = bson_new_from_data((uint8_t *)event->data, event->data_len);
    if (bson) {
        size_t length = 0;
        response =  bson_as_json(bson, &length);
        LogPrintf(LogMQTT_Info, "message(%d) bson(%d)->json(%d):%s\n",
                sizeof(ModuleMessageUpdate), bson->len, strlen(response), response);
    }
#else
    response = (char *)event->data; /*json*/
#endif
    if (response) {
        cJSON *json = cJSON_Parse(response);
        if (json) {
            int arraysize = cJSON_GetArraySize(json);                        
            if (arraysize > 0) {
                int index = 0;
                for (index = 0; index < arraysize; index++) {
                    const char *strings = MQTTcJsonGetString(json, index, "htype");
                    if (!strcmp(strings, "GetTemperature")) {
                        const float temperature = MQTTcJsonGetNumber(json, index, "temperature");
                        LogPrintf(LogMQTT_Info, "htype:%s\n", strings);
                        /* LogPrintf(LogMQTT_Info, "temperature:%f\n", temperature); */

                        ModuleMessage message;
                        message.attr = ModuleDataAttr_GetTemperature;
                        message.getTemperature.temperature = temperature;
                        if (mqtt->send) {
                            status = mqtt->send(gPriv, DataAttr_MqttToUart, &message, sizeof(message), 0);
                            if (!status) {
                                /* mqtt->attr = ModuleDataAttr_GetTemperature; */
                                MQTTMessageRecvUartHandler(mqtt);
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

int32_t MQTTMessageSendHandler(MQTT *mqtt, esp_mqtt_event_handle_t event) {
    ModuleDataAttr attr = toStringEnum(event->topic);
    switch (attr) {
        case ModuleDataAttr_Update:
            {
                MQTTMessageSendHandlerTTUpdate(mqtt, event);
                break;
            }
        case ModuleDataAttr_GetTemperature:
            {
                MQTTMessageSendHandlerGetTemperature(mqtt, event);
                break;
            }
        case ModuleDataAttr_GetModuleVersion:
            {
                MQTTMessageSendHandlerGetModuleVersion(mqtt, event);
                break;
            }
        case ModuleDataAttr_GetModuleInfo:
            {
                MQTTMessageSendHandlerGetModuleInfo(mqtt, event);
                break;
            }
        case ModuleDataAttr_SetModuleInfo:
            {
                MQTTMessageSendHandlerSetModuleInfo(mqtt, event);
                break;
            }
        case ModuleDataAttr_GetPower:
            {
                MQTTMessageSendHandlerGetPower(mqtt, event);
                break;
            }
        case ModuleDataAttr_Reboot:
            {
                MQTTMessageSendHandlerReboot(mqtt, event);
                break;
            }
        case ModuleDataAttr_NetState:
            {
                MQTTMessageSendHandlerNetState(mqtt, event);
                break;
            }
        case ModuleDataAttr_GetEthCfg:
            {
                MQTTMessageSendHandlerGetEthCfg(mqtt, event);
                break;
            }
        case ModuleDataAttr_SetEthCfg:
            {
                MQTTMessageSendHandlerSetEthCfg(mqtt, event);
                break;
            }
        case ModuleDataAttr_GetWifiCfg:
            {
                MQTTMessageSendHandlerGetWifiCfg(mqtt, event);
                break;
            }
        case ModuleDataAttr_SetWifiCfg:
            {
                MQTTMessageSendHandlerSetWifiCfg(mqtt, event);
                break;
            }
        case ModuleDataAttr_GetMqttCfg:
            {
                MQTTMessageSendHandlerGetMqttCfg(mqtt, event);
                break;
            }
        case ModuleDataAttr_SetMqttCfg:
            {
                MQTTMessageSendHandlerSetMqttCfg(mqtt, event);
                break;
            }
        case ModuleDataAttr_PtSend:
            {
                MQTTMessageSendHandlerPTSend(mqtt, event);
                break;
            }
        default:break;
    }

    return 0;
}

int32_t MQTTMessageSubscribeList(MQTT *mqtt) {
    int32_t index = 0; 

    for (index = 0; index < ModuleDataAttr_Cnt; index++) {
        esp_mqtt_client_subscribe(
                mqtt->client, 
                toEnumString((ModuleDataAttr)index),
                mqtt->msgQos);//消息必需成功???
    }

    return 0;
}

static void timer_cb(void *arg) {
    MQTT *mqtt = (MQTT *)arg;

    esp_mqtt_event_t event;
    memset(&event, 0x0, sizeof(event));
    event.event_id  = MQTT_USER_EVENT;

    esp_mqtt_dispatch_custom_event(mqtt->client, &event);
}


void MQTTEventHandler(void *handler_args, esp_event_base_t base,
        int32_t event_id, void *event_data) {
    MQTT *mqtt = (MQTT *)handler_args;

    LogPrintf(LogMQTT_Info, "Event dispatched from event loop base=%s, event_id=%" PRIi32 " CPUID:%d\n", base, event_id, xPortGetCoreID());

    /* int msg_id; */
    esp_mqtt_event_handle_t event   = event_data;

    /*任何消息前先处理消息数据*/
    MQTTMessageRecvWifiHandler(mqtt);
    MQTTMessageRecvUartHandler(mqtt);
    MQTTMessageRecvEthHandler(mqtt);
    MQTTMessageRecvSpiHandler(mqtt);

    switch (/* (esp_mqtt_event_id_t) */event_id) {
        case MQTT_EVENT_CONNECTED:
            {
                mqtt->connect = 1;
                LogPrintf(LogMQTT_Info, "[%s]MQTT_EVENT_CONNECTED\n", mqtt->name);
                MQTTMessageSubscribeList(mqtt);
                break;
            }
        case MQTT_EVENT_DISCONNECTED:
            {
                mqtt->connect = 0;
                esp_mqtt_client_reconnect(mqtt->client);
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

    esp_mqtt_event_t event;
    memset(&event, 0x0, sizeof(event));
    event.event_id  = MQTT_USER_EVENT;

    return esp_mqtt_dispatch_custom_event(mqtt->client, &event);
    /* printf ("MQTTETHEVnetID:%d status:%d\n", event.event_id, status); */

    return 0;
}

int32_t MQTTUartTriggerRecv(void *arg) {
    MQTT *mqtt = (MQTT *)arg;

    esp_mqtt_event_t event;
    memset(&event, 0x0, sizeof(event));
    event.event_id  = MQTT_USER_EVENT;

    return esp_mqtt_dispatch_custom_event(mqtt->client, &event);
    /* printf ("MQTTUARTEVnetID:%d status:%d\n", event.event_id, status); */

    return 0;
}

int32_t MQTTSpiTriggerRecv(void *arg) {
    MQTT *mqtt = (MQTT *)arg;

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

    esp_mqtt_event_t event;
    memset(&event, 0x0, sizeof(event));
    event.event_id  = MQTT_USER_EVENT;

    return esp_mqtt_dispatch_custom_event(mqtt->client, &event);
    /* printf ("MQTTWIFIEVnetID:%d status:%d\n", event.event_id, status); */

    return 0;
}

void *MQTTInit(MQTTConfig *config) {
    esp_mqtt_client_config_t mqttConfig  = { };

    MQTT *mqtt = (MQTT *) malloc (sizeof(*mqtt));
    ERRP(NULL == mqtt, return NULL, 1, "malloc MQTT Instance failure\n");
    memset(mqtt, 0x0, sizeof(*mqtt));

    mqtt->send      = config->send;
    mqtt->recv      = config->recv;
    mqtt->request   = config->request;
    mqtt->release   = config->release;
    mqtt->msgQos    = 0;
    mqtt->dataQos   = 0;

    snprintf (mqtt->name, sizeof(mqtt->name) - 1, "mqtt");

    mqttConfig.credentials.username = config->user;
    mqttConfig.credentials.authentication.password = config->password;
    mqttConfig.broker.address.uri = config->url;
    /* mqtt://192.168.0.102:1883 */
    mqttConfig.network.reconnect_timeout_ms = 1000;
    mqttConfig.network.timeout_ms = 1000;
    LogPrintf(LogMQTT_Info, "user:%s\n", mqttConfig.credentials.username);
    LogPrintf(LogMQTT_Info, "password:%s\n", mqttConfig.credentials.authentication.password);
    LogPrintf(LogMQTT_Info, "url:%s\n", mqttConfig.broker.address.uri);
    /* mqttConfig.network.refresh_connection_after_ms = 1000; */
    mqtt->client = esp_mqtt_client_init(&mqttConfig);
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

    esp_mqtt_client_start(mqtt->client);

    /* heap_trace_init_standalone(trace_record, NUM_RECORDS); */
    
    /* mqtt->connect = 1; */

    return mqtt;
ERR5:
    free(mqtt);
    return NULL;
}

