
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <gateway.h>
#include <tska_que.h>
#include <tska_thr.h>
#include <uart.h>
#include <http.h>
#include <mqtt.h>
#include <uart.h>
#include "cJSON.h"
extern "C" {
#include <message.h>
}

static void *gPriv = NULL;
static GatewayPrint gPrint;
static LogGateway gLevel = LogGateway_Info;

// #define DOUART
#define DOMQTT


/*************************************************/ 
/**************LogPrintf**************************/
/*************************************************/ 

static int GatewayLogPrintf(LogGateway level,
        const char *file, const char *func, 
        int line, const char *format, ...) {
    char logBuf[1024];
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
        GatewayLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)     
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogGateway_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogGateway_Debug, __VA_ARGS__);
#endif

#ifndef ERRP
#define ERRP(con, ret, flag, ...)               \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif

typedef struct {
    /*mqtt*/
    void *mqttpublish;
    void *mqttsubscribe;

    int8_t username[128];
    int8_t password[128];

    int8_t topic[128];  /*主题名*/
    int8_t host[128];   /*ip*/
    int16_t port;       /*port*/
    int32_t qos;        /*传输质量  0, 1, 2*/

    int32_t mqttthreading;
    TSKA_ThrHndl mqttthread;
    TSKA_QueHndl bufQue;

// #define MQTTTEST
#ifdef MQTTTEST
    void *mqttsubscribe;
    TSKA_ThrHndl mqtttestthread;
#endif

    int32_t ackValid;
    int32_t connect;

    /*uart*/
    void *uart;
} GatewayObj;

/*
 * MQTT MQTT MQTT MQTT
 * MQTT MQTT MQTT MQTT
 * MQTT MQTT MQTT MQTT
 * MQTT MQTT MQTT MQTT
 * MQTT MQTT MQTT MQTT
 */
#ifdef DOMQTT
//设置日志输出对象，主要看你想输出到哪里，就对应填充回调
int32_t GatewayInitLog(void *priv, GatewayPrint print) {
    gPriv   = priv;
    gPrint  = print;

    return 0;
}

//设置日志输出等级，看需要，调试、错误、宕机等
int32_t GatewaySetLogLevel(LogGateway level) {
    gLevel  = level;

    return 0;
}

static int32_t GatewayMqttPushlishConnect(void *oObj) {
    GatewayObj *pObj    = (GatewayObj *)oObj;

    pObj->connect = 1;
    LogPrintf(LogGateway_Info, "%s %d push connect\n", __func__, __LINE__);
    return 0;
}

static int32_t GatewayMqttPushlishDisconnect(void *oObj) {
    // GatewayObj *pObj    = (GatewayObj *)oObj;

    LogPrintf(LogGateway_Info, "%s %d\n", __func__, __LINE__);
    return 0;
}

static int32_t GatewayMqttPushlish(void *oObj) {
    // GatewayObj *pObj    = (GatewayObj *)oObj;

    LogPrintf(LogGateway_Info, "%s %d push data\n", __func__, __LINE__);
    return 0;
}

// static void *GatewayMqttMessagePushlish(void *oObj) {
    // Message *message    = NULL;
    // GatewayObj *pObj    = (GatewayObj *)oObj;

    // while (!pObj->mqttthreading) {
        // TSKA_queGet(&pObj->bufQue, (long *)&message, TSKA_TIMEOUT_FOREVER);
        // if (message) {
            // LogPrintf(LogGateway_Info, "messageType:%d mid:%d topic:%s "
                    // "payload:%s payloadlen:%d qos:%d retain:%d\n", 
                    // message->attr, 
                    // message->mqtt.mid, 
                    // message->mqtt.topic, 
                    // (char *)message->mqtt.payload, 
                    // message->mqtt.payloadlen, 
                    // message->mqtt.qos, 
                    // message->mqtt.retain); 
            // MQTTPush(pObj->mqttpublish,
                    // (const char *)message->mqtt.payload,
                    // message->mqtt.payloadlen);
            // ReleaseMessage(message);
        // }
    // }

    // pthread_exit((void *)"gatewayMqttMessage exit");

    // return NULL;
// }

/*
 * static int32_t GatewayMqttInit(GatewayObj *pObj) {
 *     int32_t status  = -1;
 * 
 *     strcpy((char *)pObj->topic, "amdin");       [>default<]
 *     strcpy((char *)pObj->host, "192.168.0.107");      [>default<]
 *     strcpy((char *)pObj->username, "admin");    [>default<]
 *     strcpy((char *)pObj->password, "123456");   [>default<]
 *     pObj->port  = 1883;                 [>default<]
 *     pObj->qos   = 1;                    [>default<]
 * 
 *     MQTTInitLog(pObj, gPrint);
 *     MQTTInit();
 * 
 *     pObj->mqttpublish = MQTTNewPublish(
 *             (const char *)pObj->username, 
 *             (const char *)pObj->password,
 *             (const char *)pObj->topic, 
 *             (const char *)pObj->host, 
 *             pObj->port, 
 *             pObj->qos, 
 *             GatewayMqttPushlishConnect, 
 *             GatewayMqttPushlishDisconnect,
 *             GatewayMqttPushlish, 
 *             pObj);
 *     ERRP(NULL == pObj->mqttpublish, return -1, 1, "MQTTNewPublish failure\n");
 * 
 *     // status = TSKA_queCreate(&pObj->bufQue, 1024);
 *     // ERRP(0 != status, goto ERR0, 1, "TSKA_queCreate bufQue failure\n");
 * 
 *     // status = TSKA_thrCreate(
 *             // &pObj->mqttthread, 
 *             // GatewayMqttMessagePushlish, 
 *             // 80, 0, pObj);
 *     // ERRP(0 != status, goto ERR1, 1, "TSKA_thrCreate pthread failure\n");
 * 
 *     return 0;
 * // ERR1:
 *     // TSKA_queDelete(&pObj->bufQue);
 * // ERR0:
 *     // MQTTDestory(pObj->mqttpublish);
 *     // return -1;
 * }
 */

static int32_t GatewayMqttSubscribeConnect(void *oObj) {
    // GatewayObj *pObj    = (GatewayObj *)oObj;

    LogPrintf(LogGateway_Info, "%s %d\n", __func__, __LINE__);
    return 0;
}

static int32_t GatewayMqttSubscribeDisconnect(void *oObj) {
    // GatewayObj *pObj    = (GatewayObj *)oObj;

    LogPrintf(LogGateway_Info, "%s %d\n", __func__, __LINE__);
    return 0;
}

static int32_t GatewayMqttSubscribeMessage(void *oObj) {
    GatewayObj *pObj    = (GatewayObj *)oObj;

    pObj->ackValid = 1;

    // LogPrintf(LogGateway_Info, "%s %d\n", __func__, __LINE__);
    return 0;
}

static int32_t GatewayMqttSubscribe(void *oObj) {
    LogPrintf(LogGateway_Info, "%s %d\n", __func__, __LINE__);
    // GatewayObj *pObj    = (GatewayObj *)oObj;

    return 0;
}

// static void *GatewayMqttTestPushlish(void *args) {
    // int32_t num         = 0;
    // Message *message= NULL;
    // GatewayObj *pObj    = (GatewayObj *)args;

    // while (!pObj->mqttthreading) {

        // int8_t payload[128];
        // int32_t size[2];

        // snprintf ((char *)payload, sizeof(payload) - 1, "%s-%d", "payload", num++);
        // size[0] = strlen((char *)pObj->topic) + 1;
        // size[1] = strlen((char *)payload) + 1;

        // message = RequestMessage(DataAttr_MqqtTest, size, sizeof(size)/sizeof(size[0]));
        // if (message) {
            // message->attr           = DataAttr_MqqtTest;
            // message->mqtt.payloadlen= strlen((char *)payload) + 1;
            // strcpy((char *)message->mqtt.topic, (char *)pObj->topic);
            // strcpy((char *)message->mqtt.payload, (char *)payload);
            // TSKA_quePut(&pObj->bufQue, (long)message, TSKA_TIMEOUT_FOREVER);
        // }

        // sleep(3);
    // }

    // pthread_exit((void *)"gatewayMqttMessage exit");

    // return NULL;
// }

/*
 * static int32_t GatewayMqttSubScri(GatewayObj *pObj) {
 *     int32_t status = -1;
 * 
 *     pObj->mqttsubscribe = MQTTNewSubscribe(
 *             (const char *)pObj->username, 
 *             (const char *)pObj->password,
 *             (const char *)pObj->topic, 
 *             (const char *)pObj->host, 
 *             pObj->port, 
 *             pObj->qos, 
 *             GatewayMqttSubscribeConnect, 
 *             GatewayMqttSubscribeDisconnect, 
 *             GatewayMqttSubscribe, 
 *             GatewayMqttSubscribeMessage, 
 *             pObj);
 *     ERRP(NULL == pObj->mqttsubscribe, return -1, 1, "MQTTNewSubscribe failure\n");
 * 
 *     // status = TSKA_thrCreate(
 *             // &pObj->mqtttestthread, 
 *             // GatewayMqttTestPushlish, 
 *             // 80, 0, pObj);
 *     // ERRP(0 != status, goto ERR1, 1, "TSKA_thrCreate pthread failure\n");
 * 
 *     return 0;
 * // ERR1:
 *     // MQTTDestory(pObj->mqttsubscribe);
 *     // return -1;
 * }
 */
#endif

/*
 * MQTT EXIT
 * MQTT EXIT
 * MQTT EXIT
 * MQTT EXIT
 * MQTT EXIT
 */

/**
 * UART UART UART
 * UART UART UART
 * UART UART UART
 * UART UART UART
 * UART UART UART
 */

#ifdef DOUART
static int32_t GatewayUartSend(void *oObj, 
        void *data, int32_t millis) {
    return 0;
}

static int32_t GatewayUartRecv(void *oObj, 
        void *data, int32_t *length, int32_t millis) {
    return 0;
}

static int32_t GatewayUartInit(GatewayObj *pObj) {
    UartConfig config;

    UartInitLog(pObj, gPrint);
    UartSetLogLevel(LogUart_Info);

    config.baudrate     = 115200;
    config.bits         = 8;
    config.parity       = 'N';
    config.stop         = 1;
    config.priv         = pObj;
    config.send         = GatewayUartSend;
    config.recv         = GatewayUartRecv;
    strcpy(config.tty, "/dev/ttyUSB0");

    pObj->uart = UartInit(&config);
    ERRP(NULL == pObj->uart, return -1, 1, "UartInit failure\n");

    return 0;
}
#endif

/*
 * UART EXIT
 * UART EXIT
 * UART EXIT
 * UART EXIT
 * UART EXIT
 * UART EXIT
 */

//初始化各个模块
void *GatewayInit(void) {
    GatewayObj *pObj    = NULL;

    pObj = (GatewayObj *) malloc (sizeof(*pObj));
    ERRP(NULL == pObj, goto ERR0, 1, "malloc GatewayObj failure\n");
    memset(pObj, 0x0, sizeof(*pObj)); //整个句柄做初始化处理

    strcpy((char *)pObj->topic, "amdin");       /*default*/
    strcpy((char *)pObj->host, "192.168.0.107");      /*default*/
    strcpy((char *)pObj->username, "admin");    /*default*/
    strcpy((char *)pObj->password, "123456");   /*default*/
    pObj->port  = 1883;                 /*default*/
    pObj->qos   = 0;                    /*default*/

    MQTTInitLog(pObj, gPrint);
    MQTTInit();

    {
        pObj->ackValid = 0;
        pObj->connect = 0;
        pObj->mqttsubscribe = MQTTNewSubscribe(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toEnumString(ModuleDataAttr_GetTemperature), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttSubscribeConnect, 
                GatewayMqttSubscribeDisconnect, 
                GatewayMqttSubscribe, 
                GatewayMqttSubscribeMessage, 
                pObj);
        
        pObj->mqttpublish = MQTTNewPublish(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toAckEnumString(ModuleDataAttr_GetTemperature), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttPushlishConnect, 
                GatewayMqttPushlishDisconnect,
                GatewayMqttPushlish, 
                pObj);
        while (!pObj->connect) {
            sleep(1);
        }
        if (pObj->mqttpublish) {
            cJSON *root = NULL;
            cJSON *sub  = NULL;

            root = cJSON_CreateArray();
            if (root) {
                sub = cJSON_CreateObject();
                if (sub) {
                    cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetTemperature));
                    // cJSON_AddNumberToObject(sub, "temperature", 20);
                    cJSON_AddItemToArray(root, sub);
                    char *json = cJSON_Print(root);
                    if (json) {
                        LogPrintf(LogGateway_Info, "json:\r\n%s\n", json);
                        MQTTPush(pObj->mqttpublish,
                                (const char *)json,
                                strlen(json));
                        free(json);
                    }
                    cJSON_Delete(root);
                }
            }
        }


        while (!pObj->ackValid) {
            sleep(1);
        }
        LogPrintf(LogGateway_Info, "Message %s suc\n", toEnumString(ModuleDataAttr_GetTemperature));
        MQTTDestory(pObj->mqttpublish);
        MQTTDestory(pObj->mqttsubscribe);
    }

    {
        pObj->ackValid = 0;
        pObj->connect = 0;
        pObj->mqttsubscribe = MQTTNewSubscribe(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toEnumString(ModuleDataAttr_GetModuleVersion), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttSubscribeConnect, 
                GatewayMqttSubscribeDisconnect, 
                GatewayMqttSubscribe, 
                GatewayMqttSubscribeMessage, 
                pObj);

        pObj->mqttpublish = MQTTNewPublish(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toAckEnumString(ModuleDataAttr_GetModuleVersion), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttPushlishConnect, 
                GatewayMqttPushlishDisconnect,
                GatewayMqttPushlish, 
                pObj);
        while (!pObj->connect) {
            sleep(1);
        }
        if (pObj->mqttpublish) {
            cJSON *root = NULL;
            cJSON *sub  = NULL;

            root = cJSON_CreateArray();
            if (root) {
                sub = cJSON_CreateObject();
                if (sub) {
                    cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetModuleVersion));
                    // cJSON_AddNumberToObject(sub, "temperature", 20);
                    cJSON_AddItemToArray(root, sub);
                    char *json = cJSON_Print(root);
                    if (json) {
                        LogPrintf(LogGateway_Info, "json:\r\n%s\n", json);
                        MQTTPush(pObj->mqttpublish,
                                (const char *)json,
                                strlen(json));
                        free(json);
                    }
                    cJSON_Delete(root);
                }
            }
        }

        while (!pObj->ackValid) {
            sleep(1);
        }
        LogPrintf(LogGateway_Info, "Message %s suc\n", toEnumString(ModuleDataAttr_GetModuleVersion));
        MQTTDestory(pObj->mqttpublish);
        MQTTDestory(pObj->mqttsubscribe);
    }

    {
        pObj->ackValid = 0;
        pObj->connect = 0;
        pObj->mqttsubscribe = MQTTNewSubscribe(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toEnumString(ModuleDataAttr_GetModuleInfo), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttSubscribeConnect, 
                GatewayMqttSubscribeDisconnect, 
                GatewayMqttSubscribe, 
                GatewayMqttSubscribeMessage, 
                pObj);
        pObj->mqttpublish = MQTTNewPublish(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toAckEnumString(ModuleDataAttr_GetModuleInfo), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttPushlishConnect, 
                GatewayMqttPushlishDisconnect,
                GatewayMqttPushlish, 
                pObj);
        while (!pObj->connect) {
            sleep(1);
        }
        if (pObj->mqttpublish) {
            cJSON *root = NULL;
            cJSON *sub  = NULL;

            root = cJSON_CreateArray();
            if (root) {
                sub = cJSON_CreateObject();
                if (sub) {
                    cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetModuleInfo));
                    // cJSON_AddNumberToObject(sub, "temperature", 20);
                    cJSON_AddItemToArray(root, sub);
                    char *json = cJSON_Print(root);
                    if (json) {
                        LogPrintf(LogGateway_Info, "json:\r\n%s\n", json);
                        MQTTPush(pObj->mqttpublish,
                                (const char *)json,
                                strlen(json));
                        free(json);
                    }
                    cJSON_Delete(root);
                }
            }
        }


        while (!pObj->ackValid) {
            sleep(1);
        }
        LogPrintf(LogGateway_Info, "Message %s suc\n", toEnumString(ModuleDataAttr_GetModuleInfo));
        MQTTDestory(pObj->mqttpublish);
        MQTTDestory(pObj->mqttsubscribe);
    }

    {
        pObj->ackValid = 0;
        pObj->connect = 0;
        pObj->mqttsubscribe = MQTTNewSubscribe(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toEnumString(ModuleDataAttr_SetModuleInfo), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttSubscribeConnect, 
                GatewayMqttSubscribeDisconnect, 
                GatewayMqttSubscribe, 
                GatewayMqttSubscribeMessage, 
                pObj);
        pObj->mqttpublish = MQTTNewPublish(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toAckEnumString(ModuleDataAttr_SetModuleInfo), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttPushlishConnect, 
                GatewayMqttPushlishDisconnect,
                GatewayMqttPushlish, 
                pObj);
        while (!pObj->connect) {
            sleep(1);
        }
        if (pObj->mqttpublish) {
            cJSON *root = NULL;
            cJSON *sub  = NULL;

            root = cJSON_CreateArray();
            if (root) {
                sub = cJSON_CreateObject();
                if (sub) {
                    cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_SetModuleInfo));
                    cJSON_AddStringToObject(sub, "info", "zengzhen");
                    cJSON_AddItemToArray(root, sub);
                    char *json = cJSON_Print(root);
                    if (json) {
                        LogPrintf(LogGateway_Info, "json:\r\n%s\n", json);
                        MQTTPush(pObj->mqttpublish,
                                (const char *)json,
                                strlen(json));
                        free(json);
                    }
                    cJSON_Delete(root);
                }
            }
        }


        while (!pObj->ackValid) {
            sleep(1);
        }
        LogPrintf(LogGateway_Info, "Message %s suc\n", toEnumString(ModuleDataAttr_SetModuleInfo));
        MQTTDestory(pObj->mqttpublish);
        MQTTDestory(pObj->mqttsubscribe);
    }

    {
        pObj->ackValid = 0;
        pObj->connect = 0;
        pObj->mqttsubscribe = MQTTNewSubscribe(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toEnumString(ModuleDataAttr_GetPower), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttSubscribeConnect, 
                GatewayMqttSubscribeDisconnect, 
                GatewayMqttSubscribe, 
                GatewayMqttSubscribeMessage, 
                pObj);
        pObj->mqttpublish = MQTTNewPublish(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toAckEnumString(ModuleDataAttr_GetPower), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttPushlishConnect, 
                GatewayMqttPushlishDisconnect,
                GatewayMqttPushlish, 
                pObj);
        while (!pObj->connect) {
            sleep(1);
        }
        if (pObj->mqttpublish) {
            cJSON *root = NULL;
            cJSON *sub  = NULL;

            root = cJSON_CreateArray();
            if (root) {
                sub = cJSON_CreateObject();
                if (sub) {
                    cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetPower));
                    // cJSON_AddStringToObject(sub, "info", "zengzhen");
                    cJSON_AddItemToArray(root, sub);
                    char *json = cJSON_Print(root);
                    if (json) {
                        LogPrintf(LogGateway_Info, "json:\r\n%s\n", json);
                        MQTTPush(pObj->mqttpublish,
                                (const char *)json,
                                strlen(json));
                        free(json);
                    }
                    cJSON_Delete(root);
                }
            }
        }


        while (!pObj->ackValid) {
            sleep(1);
        }
        LogPrintf(LogGateway_Info, "Message %s suc\n", toEnumString(ModuleDataAttr_GetPower));
        MQTTDestory(pObj->mqttpublish);
        MQTTDestory(pObj->mqttsubscribe);
    }

    {
        pObj->ackValid = 0;
        pObj->connect = 0;
        pObj->mqttsubscribe = MQTTNewSubscribe(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toEnumString(ModuleDataAttr_GetEthCfg), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttSubscribeConnect, 
                GatewayMqttSubscribeDisconnect, 
                GatewayMqttSubscribe, 
                GatewayMqttSubscribeMessage, 
                pObj);
        pObj->mqttpublish = MQTTNewPublish(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toAckEnumString(ModuleDataAttr_GetEthCfg), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttPushlishConnect, 
                GatewayMqttPushlishDisconnect,
                GatewayMqttPushlish, 
                pObj);
        while (!pObj->connect) {
            sleep(1);
        }
        if (pObj->mqttpublish) {
            cJSON *root = NULL;
            cJSON *sub  = NULL;

            root = cJSON_CreateArray();
            if (root) {
                sub = cJSON_CreateObject();
                if (sub) {
                    cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetEthCfg));
                    // cJSON_AddStringToObject(sub, "info", "zengzhen");
                    cJSON_AddItemToArray(root, sub);
                    char *json = cJSON_Print(root);
                    if (json) {
                        LogPrintf(LogGateway_Info, "json:\r\n%s\n", json);
                        MQTTPush(pObj->mqttpublish,
                                (const char *)json,
                                strlen(json));
                        free(json);
                    }
                    cJSON_Delete(root);
                }
            }
        }


        while (!pObj->ackValid) {
            sleep(1);
        }
        LogPrintf(LogGateway_Info, "Message %s suc\n", toEnumString(ModuleDataAttr_GetEthCfg));
        MQTTDestory(pObj->mqttpublish);
        MQTTDestory(pObj->mqttsubscribe);
    }

    {
        pObj->ackValid = 0;
        pObj->connect = 0;
        pObj->mqttsubscribe = MQTTNewSubscribe(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toEnumString(ModuleDataAttr_SetEthCfg), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttSubscribeConnect, 
                GatewayMqttSubscribeDisconnect, 
                GatewayMqttSubscribe, 
                GatewayMqttSubscribeMessage, 
                pObj);
        pObj->mqttpublish = MQTTNewPublish(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toAckEnumString(ModuleDataAttr_SetEthCfg), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttPushlishConnect, 
                GatewayMqttPushlishDisconnect,
                GatewayMqttPushlish, 
                pObj);
        while (!pObj->connect) {
            sleep(1);
        }
        if (pObj->mqttpublish) {
            cJSON *root = NULL;
            cJSON *sub  = NULL;

            root = cJSON_CreateArray();
            if (root) {
                sub = cJSON_CreateObject();
                if (sub) {
                    cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_SetEthCfg));
                    cJSON_AddStringToObject(sub, "address", "192.168.0.103");
                    cJSON_AddStringToObject(sub, "netmask", "255.255.255.0");
                    cJSON_AddStringToObject(sub, "gateway", "192.168.0.1");
                    cJSON_AddItemToArray(root, sub);
                    char *json = cJSON_Print(root);
                    if (json) {
                        LogPrintf(LogGateway_Info, "json:\r\n%s\n", json);
                        MQTTPush(pObj->mqttpublish,
                                (const char *)json,
                                strlen(json));
                        free(json);
                    }
                    cJSON_Delete(root);
                }
            }
        }


        while (!pObj->ackValid) {
            sleep(1);
        }
        LogPrintf(LogGateway_Info, "Message %s suc\n", toEnumString(ModuleDataAttr_SetEthCfg));
        MQTTDestory(pObj->mqttpublish);
        MQTTDestory(pObj->mqttsubscribe);
    }

    {
        pObj->ackValid = 0;
        pObj->connect = 0;
        pObj->mqttsubscribe = MQTTNewSubscribe(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toEnumString(ModuleDataAttr_GetWifiCfg), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttSubscribeConnect, 
                GatewayMqttSubscribeDisconnect, 
                GatewayMqttSubscribe, 
                GatewayMqttSubscribeMessage, 
                pObj);
        pObj->mqttpublish = MQTTNewPublish(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toAckEnumString(ModuleDataAttr_GetWifiCfg), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttPushlishConnect, 
                GatewayMqttPushlishDisconnect,
                GatewayMqttPushlish, 
                pObj);
        while (!pObj->connect) {
            sleep(1);
        }
        if (pObj->mqttpublish) {
            cJSON *root = NULL;
            cJSON *sub  = NULL;

            root = cJSON_CreateArray();
            if (root) {
                sub = cJSON_CreateObject();
                if (sub) {
                    cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_GetWifiCfg));
                    /*
                     * cJSON_AddStringToObject(sub, "address", "192.168.0.103");
                     * cJSON_AddStringToObject(sub, "netmask", "255.255.255.0");
                     * cJSON_AddStringToObject(sub, "gateway", "192.168.0.1");
                     */
                    cJSON_AddItemToArray(root, sub);
                    char *json = cJSON_Print(root);
                    if (json) {
                        LogPrintf(LogGateway_Info, "json:\r\n%s\n", json);
                        MQTTPush(pObj->mqttpublish,
                                (const char *)json,
                                strlen(json));
                        free(json);
                    }
                    cJSON_Delete(root);
                }
            }
        }


        while (!pObj->ackValid) {
            sleep(1);
        }
        LogPrintf(LogGateway_Info, "Message %s suc\n", toEnumString(ModuleDataAttr_GetWifiCfg));
        MQTTDestory(pObj->mqttpublish);
        MQTTDestory(pObj->mqttsubscribe);
    }

    {
        pObj->ackValid = 0;
        pObj->connect = 0;
        pObj->mqttsubscribe = MQTTNewSubscribe(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toEnumString(ModuleDataAttr_Update), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttSubscribeConnect, 
                GatewayMqttSubscribeDisconnect, 
                GatewayMqttSubscribe, 
                GatewayMqttSubscribeMessage, 
                pObj);
        pObj->mqttpublish = MQTTNewPublish(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toAckEnumString(ModuleDataAttr_Update), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttPushlishConnect, 
                GatewayMqttPushlishDisconnect,
                GatewayMqttPushlish, 
                pObj);
        while (!pObj->connect) {
            sleep(1);
        }
        if (pObj->mqttpublish) {
            cJSON *root = NULL;
            cJSON *sub  = NULL;

            root = cJSON_CreateArray();
            if (root) {
                sub = cJSON_CreateObject();
                if (sub) {
                    cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_Update));
                    cJSON_AddStringToObject(sub, "url", "http://192.168.0.107:8080/build/hello_world.bin");
                    cJSON_AddItemToArray(root, sub);
                    char *json = cJSON_Print(root);
                    if (json) {
                        LogPrintf(LogGateway_Info, "json:\r\n%s\n", json);
                        MQTTPush(pObj->mqttpublish,
                                (const char *)json,
                                strlen(json));
                        free(json);
                    }
                    cJSON_Delete(root);
                }
            }
        }


        while (!pObj->ackValid) {
            sleep(1);
        }
        LogPrintf(LogGateway_Info, "Message %s suc\n", toEnumString(ModuleDataAttr_Update));
        MQTTDestory(pObj->mqttpublish);
        MQTTDestory(pObj->mqttsubscribe);
    }

    {
        pObj->ackValid = 0;
        pObj->connect = 0;
        pObj->mqttsubscribe = MQTTNewSubscribe(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toEnumString(ModuleDataAttr_SetWifiCfg), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttSubscribeConnect, 
                GatewayMqttSubscribeDisconnect, 
                GatewayMqttSubscribe, 
                GatewayMqttSubscribeMessage, 
                pObj);
        pObj->mqttpublish = MQTTNewPublish(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toAckEnumString(ModuleDataAttr_SetWifiCfg), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                GatewayMqttPushlishConnect, 
                GatewayMqttPushlishDisconnect,
                GatewayMqttPushlish, 
                pObj);
        while (!pObj->connect) {
            sleep(1);
        }
        if (pObj->mqttpublish) {
            cJSON *root = NULL;
            cJSON *sub  = NULL;

            root = cJSON_CreateArray();
            if (root) {
                sub = cJSON_CreateObject();
                if (sub) {
                    cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_SetWifiCfg));
                    cJSON_AddStringToObject(sub, "ssid", "TP-LINK_342B");
                    cJSON_AddStringToObject(sub, "password", "88888888");
                    // cJSON_AddStringToObject(sub, "ssid", "zz");
                    // cJSON_AddStringToObject(sub, "password", "12345678");
                    // cJSON_AddStringToObject(sub, "address", "0.0.0.0");
                    // cJSON_AddStringToObject(sub, "netmask", "255.255.255.0");
                    // cJSON_AddStringToObject(sub, "gateway", "192.168.0.1");
                    cJSON_AddStringToObject(sub, "address", "192.168.0.103");
                    cJSON_AddStringToObject(sub, "netmask", "255.255.255.0");
                    cJSON_AddStringToObject(sub, "gateway", "192.168.0.1");
                    cJSON_AddItemToArray(root, sub);
                    char *json = cJSON_Print(root);
                    if (json) {
                        LogPrintf(LogGateway_Info, "json:\r\n%s\n", json);
                        MQTTPush(pObj->mqttpublish,
                                (const char *)json,
                                strlen(json));
                        free(json);
                    }
                    cJSON_Delete(root);
                }
            }
        }


        while (!pObj->ackValid) {
            sleep(1);
        }
        LogPrintf(LogGateway_Info, "Message %s suc\n", toEnumString(ModuleDataAttr_SetWifiCfg));
        MQTTDestory(pObj->mqttpublish);
        MQTTDestory(pObj->mqttsubscribe);
    }



    return pObj;
ERR0:
    return NULL;
}
