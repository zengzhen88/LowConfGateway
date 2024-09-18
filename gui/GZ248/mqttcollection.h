#ifndef MQTTCOLLECTION_H
#define MQTTCOLLECTION_H

#include <common.h>
#include <mqtt.h>

typedef struct {
    void *pushlish;
    void *scribe[ModuleDataAttr_Cnt];
} MQTTCollection;

void *CreateMQTTCollection(const char *username,
                           const char *password,
                           const char *hostname,
                           int32_t port,
                           int32_t qos,
                           MQTTConnect pushConnect,
                           MQTTDisconnect pushDisconnect,
                           MQTTPushlish pushPushlish,
                           MQTTConnect scribeConnect,
                           MQTTDisconnect scribeDisconnect,
                           MQTTSubscribe scribeScribe,
                           MQTTMessage scribeMessage);

#endif // MQTTCOLLECTION_H
