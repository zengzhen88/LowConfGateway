#if 0
#include <mqttcollection.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqtt.h>


typedef int32_t (*MQTTPrint)(void *, const char *);
typedef int32_t (*MQTTConnect)(void *);//, int32_t);
typedef int32_t (*MQTTDisconnect)(void *);//, int32_t);
typedef int32_t (*MQTTPushlish)(void *);//, int32_t);
typedef int32_t (*MQTTSubscribe)(void *);//, int32_t);
typedef int32_t (*MQTTMessage)(void *, const char *topic, const char *, int32_t);

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
                           MQTTMessage scribeMessage) {
    MQTTCollection *collection = (MQTTCollection *)malloc (sizeof(sizeof(MQTTCollection)));
    if (NULL == collection) {
        printf ("malloc MQTTCollection failure(username:%s password:%s hostname:%s port:%d qos:%d\n",
                username, password, hostname, port, qos);
        return NULL;
    }
    memset(collection, 0x0, sizeof(*collection));

    collection->pushlish = MQTTNewPublish(
                username,
                password,
                hostname,
                port,
                qos,
                pushConnect,
                pushDisconnect,
                pushPushlish,
                collection);
    if (NULL == collection->pushlish) {
        printf ("create MQTTNewPushlish failure(username:%s password:%s hostname:%s port:%d qos:%d\n",
                username, password, hostname, port, qos);
        free(collection);
        return NULL;
    }

    for (int32_t index = 0; index < ModuleDataAttr_Cnt; index++) {
        if (!strcmp(toEnumChineseString((ModuleDataAttr)index), "Ack")) continue;
        collection->scribe[index] = MQTTNewSubscribe(
                    username,
                    password,
                    "invalid",
                    hostname,
                    port,
                    qos,
                    scribeConnect,
                    scribeDisconnect,
                    scribeScribe,
                    scribeMessage,
                    collection);
        if (NULL == collection->scribe[index]) {
            MQTTDestory(collection->pushlish);
            for (int32_t jndex = 0; jndex < ModuleDataAttr_Cnt; jndex++) {
                if (collection->scribe[jndex]) {
                    MQTTDestory(collection->scribe[index]);
                }
            }
        }
    }

    return collection;
}
#endif
