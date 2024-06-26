#ifndef __MQTT_PROJECT_H__
#define __MQTT_PROJECT_H__

#include <common.h>

typedef enum {
    LogMQTT_None = 0,  //关闭日志输出 
    LogMQTT_Urgent,    //必须打的
    LogMQTT_Fatal,     //致使级
    LogMQTT_Error,     //错误级
    LogMQTT_Warning,   //告警级
    LogMQTT_Info,      //业务级
    LogMQTT_Debug,     //调试级
    LogMQTT_Trace,     //跟踪级
    LogMQTT_Detail,    //详细级
    LogMQTT_Cnt
} LogMQTT;

typedef int32_t (*MQTTPrint)(void *, const char *);
typedef int32_t (*MQTTSigSend)(void *oObj,
        DataAttr attr, void *data, 
        int32_t fillLength, int32_t millis);
typedef int32_t (*MQTTSigRecv)(void *oObj,
        DataAttr attr, void *data,
        int32_t *fillLength, int32_t millis);

int32_t MQTTInitLog(void *priv, MQTTPrint print);
int32_t MQTTSetLogLevel(LogMQTT level);

typedef struct {
    char name[128];
    char username[128];
    char password[128];
    char url[128];
    char hostname[128];
    uint32_t port;
    MQTTSigSend send;
    MQTTSigRecv recv;
} MQTTConfig;

void *MQTTInit(MQTTConfig *config);
int32_t MQTTUartTriggerRecv(void *arg);
int32_t MQTTEthTriggerRecv(void *arg);
int32_t MQTTWifiTriggerRecv(void *arg);

#endif /* __MQTT_PROJECT_H__ */
