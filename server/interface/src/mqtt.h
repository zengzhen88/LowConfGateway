#ifndef __MQTT_PROJECT_H__
#define __MQTT_PROJECT_H__

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
typedef int32_t (*MQTTConnect)(void *);//, int32_t);
typedef int32_t (*MQTTDisconnect)(void *);//, int32_t);
typedef int32_t (*MQTTPushlish)(void *);//, int32_t);
typedef int32_t (*MQTTSubscribe)(void *);//, int32_t);
typedef int32_t (*MQTTMessage)(void *);//, int32_t);

int32_t MQTTInitLog(void *priv, MQTTPrint print);
int32_t MQTTSetLogLevel(LogMQTT level);

int32_t MQTTInit(void);
int32_t MQTTDestory(void *oObj);
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
        void *callctx);
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
        void *callctx);
int32_t MQTTPush(void *oObj, const char *payload,
        int32_t payloadlen);

#endif /* __MQTT_PROJECT_H__ */

