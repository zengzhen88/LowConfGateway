#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <mosquitto.h>
#include <mqtt.h>

static void *gPriv = NULL;
static MQTTPrint gPrint;
static LogMQTT gLevel = LogMQTT_Info;

#define KEEP_ALIVE 60

/*************************************************/ 
/**************LogPrintf**************************/
/*************************************************/ 

static int MQTTLogPrintf(LogMQTT level,
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
        MQTTLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

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

typedef struct {
    void *mqtt;

    void *priv;
    MQTTConnect connect;
    MQTTDisconnect disconnect;
    MQTTPushlish pushlish;
    MQTTSubscribe subscribe;
    MQTTMessage message;

    char topic[1024];
    char host[1024];
    int16_t port;
    int32_t mid;
    int32_t qos;
} MQTTObj;

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

static void MQTTCallbackPushlishConnect(struct mosquitto *mosq, void *oObj, int32_t rc) {
    LogPrintf(LogMQTT_Info, "connect suc(%s)\n", mosquitto_connack_string(rc));
    MQTTObj *pObj   = (MQTTObj *)oObj;

    if (0 != rc) {
        //重连
        // reconnect():
    }
    else {
        if (pObj) {
            if (pObj->connect) {
                pObj->connect(pObj->priv);
            }
        }
    }

}

static void MQTTCallbackSubscribeConnect(struct mosquitto *mosq, void *oObj, int32_t rc) {
    LogPrintf(LogMQTT_Info, "connect suc(%s)\n", mosquitto_connack_string(rc));
    MQTTObj *pObj   = (MQTTObj *)oObj;

    if (0 != rc) {
        //重连
        // reconnect():
    }
    else {
        mosquitto_subscribe((struct mosquitto *)pObj->mqtt, NULL, pObj->topic, pObj->qos);

        if (pObj) {
            if (pObj->connect) {
                pObj->connect(pObj->priv);
            }
        }
    }
}

static void MQTTCallbackDisconnect(struct mosquitto *mosq, void *oObj, int32_t rc) {
    LogPrintf(LogMQTT_Info, "disconnect suc\n");
    MQTTObj *pObj   = (MQTTObj *)oObj;

    if (pObj) {
        if (pObj->disconnect) {
            pObj->disconnect(pObj->priv);
        }
    }
}

static void MQTTCallbackPushlish(struct mosquitto *mosq, void *oObj, int32_t mid) {
    LogPrintf(LogMQTT_Info, "pushlish suc\n");
    MQTTObj *pObj   = (MQTTObj *)oObj;

    if (pObj) {
        if (pObj->pushlish) {
            pObj->pushlish(pObj->priv);
        }
    }
}

static void MQTTCallbackSubscribe(struct mosquitto *mosq, void *oObj, int32_t mid, 
        int32_t qos_count, const int32_t *granted_qos) {
    LogPrintf(LogMQTT_Info, "subscribe suc\n");
    MQTTObj *pObj   = (MQTTObj *)oObj;

    if (pObj) {
        if (pObj->subscribe) {
            pObj->subscribe(pObj->priv);
        }
    }
}

static void MQTTCallbackMessage(struct mosquitto *mosq, void *oObj,
        const struct mosquitto_message *msg) {
    LogPrintf(LogMQTT_Info, "message suc ==> topic:%s payload:%s\n",
            (char *)msg->topic, (char *)msg->payload);
    MQTTObj *pObj   = (MQTTObj *)oObj;

    if (pObj) {
        if (pObj->message) {
            pObj->message(pObj->priv);
        }
    }
}

int32_t MQTTInit(void) {
    return mosquitto_lib_init();
}

void *MQTTNewPublish(
        const char *username, 
        const char *password,
        const char *topic,
        const char *host, 
        const short port,
        int32_t qos,
        MQTTConnect connect,
        MQTTDisconnect disconnect,
        MQTTPushlish pushlish,
        void *callctx) {
    int32_t status  = -1;
    MQTTObj *pObj   = (MQTTObj *) malloc (sizeof(*pObj));
    ERRP(NULL == pObj, return NULL, 1, 
            "malloc MQTTObj host:%s port:%dfailure\n", 
            host, port);

    memset(pObj, 0x0, sizeof(*pObj));

    pObj->connect       = connect;
    pObj->disconnect    = disconnect;
    pObj->pushlish      = pushlish;
    strcpy(pObj->host, host);
    pObj->port          = port;
    pObj->qos           = qos;
    pObj->priv          = callctx;
    strcpy(pObj->topic, topic);

    pObj->mqtt = mosquitto_new(NULL, true, pObj);
    ERRP(NULL == pObj->mqtt, goto ERR0, 1,
            "mosquitto_new  host:%d port:%d failure\n", 
            host, port);

    mosquitto_connect_callback_set((struct mosquitto *)pObj->mqtt, MQTTCallbackPushlishConnect);
    mosquitto_disconnect_callback_set((struct mosquitto *)pObj->mqtt, MQTTCallbackDisconnect);
    mosquitto_publish_callback_set((struct mosquitto *)pObj->mqtt, MQTTCallbackPushlish);

    mosquitto_username_pw_set((struct mosquitto *)pObj->mqtt, username, password);

    status = mosquitto_connect((struct mosquitto *)pObj->mqtt, host, port, KEEP_ALIVE);
    ERRP(0 != status, goto ERR1, 1,
            "mosquitto_connect host:%s port:%d failure\n", 
            host, port); 

    status = mosquitto_loop_start((struct mosquitto *)pObj->mqtt);
    ERRP(0 != status, goto ERR2, 1, 
            "mosquitto_loop_start host:%s port:%d failure\n", 
            host, port);

    return pObj;
ERR2:
    mosquitto_disconnect((struct mosquitto *)pObj->mqtt);
ERR1:
    if (pObj->mqtt) mosquitto_destroy((struct mosquitto *)pObj->mqtt);
    mosquitto_lib_cleanup();
ERR0:
    free(pObj);

    return NULL;
}

void *MQTTNewSubscribe(
        const char *username, 
        const char *password,
        const char *topic,
        const char *host, 
        const short port,
        int32_t qos,
        MQTTConnect connect,
        MQTTDisconnect disconnect,
        MQTTSubscribe subscribe,
        MQTTMessage message,
        void *callctx) {
    int32_t status  = -1;
    MQTTObj *pObj   = (MQTTObj *) malloc (sizeof(*pObj));
    ERRP(NULL == pObj, return NULL, 1, 
            "malloc MQTTObj host:%s port:%dfailure\n", 
            host, port);

    memset(pObj, 0x0, sizeof(*pObj));

    pObj->connect       = connect;
    pObj->disconnect    = disconnect;
    pObj->subscribe     = subscribe;
    pObj->message       = message;
    strcpy(pObj->host, host);
    pObj->port          = port;
    pObj->qos           = qos;
    pObj->priv          = callctx;
    strcpy(pObj->topic, topic);

    pObj->mqtt = mosquitto_new(NULL, true, pObj);
    ERRP(NULL == pObj->mqtt, goto ERR0, 1,
            "mosquitto_new host:%d port:%d failure\n", 
            host, port);

    mosquitto_connect_callback_set((struct mosquitto *)pObj->mqtt, MQTTCallbackSubscribeConnect);
    mosquitto_disconnect_callback_set((struct mosquitto *)pObj->mqtt, MQTTCallbackDisconnect);
    mosquitto_subscribe_callback_set((struct mosquitto *)pObj->mqtt, MQTTCallbackSubscribe);
    mosquitto_message_callback_set((struct mosquitto *)pObj->mqtt, MQTTCallbackMessage);

    mosquitto_username_pw_set((struct mosquitto *)pObj->mqtt, username, password);

    status = mosquitto_connect((struct mosquitto *)pObj->mqtt, host, port, KEEP_ALIVE);
    ERRP(0 != status, goto ERR1, 1,
            "mosquitto_connect host:%s port:%d failure\n", 
            host, port); 

    status = mosquitto_loop_start((struct mosquitto *)pObj->mqtt);
    ERRP(0 != status, goto ERR2, 1, 
            "mosquitto_loop_start host:%s port:%d failure\n", 
            host, port);

    return pObj;
ERR2:
    mosquitto_disconnect((struct mosquitto *)pObj->mqtt);
ERR1:
    if (pObj->mqtt) mosquitto_destroy((struct mosquitto *)pObj->mqtt);
    mosquitto_lib_cleanup();
ERR0:
    free(pObj);

    return NULL;
}

int32_t MQTTDestory(void *oObj) {
    MQTTObj *pObj   = (MQTTObj *)oObj;

    if (pObj->mqtt) mosquitto_destroy((struct mosquitto *)pObj->mqtt);
    mosquitto_lib_cleanup();
    free(pObj);

    return 0;
}

int32_t MQTTPush(void *oObj, const char *payload,
        int32_t payloadlen) {
    int32_t status = -1;
    MQTTObj *pObj   = (MQTTObj *)oObj;

    status = mosquitto_publish(
            (struct mosquitto *)pObj->mqtt, 
            &pObj->mid, 
            pObj->topic, 
            payloadlen, 
            payload, 
            pObj->qos, 
            0);
    ERRP(0 != status, return -1, 1, 
            "mosquitto_publish %s failure(MOSQ_ERR:%d)\n", 
            payload, status);
    printf ("mid:%d\n", pObj->mid);

    return 0;
}

