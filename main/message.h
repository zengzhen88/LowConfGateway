#ifndef __MESSAGE_H__
#define __MESSAGE_H__ 

#include <common.h>

typedef struct {
    DataAttr attr;
    void *data;
    int length;
    int size;
} Message;

Message *RequestMessage(int32_t size);
int32_t ReleaseMessage(Message *mess);

//ModuleDataAttr_helloworld
typedef struct {
    ModuleDataAttr attr;
    char helloworld[32];
} ModuleMessageHelloWorld; 

//ModuleDataAttr_SetWifiConfig
//ModuleDataAttr_GetWifiConfig
typedef struct {
    ModuleDataAttr attr;
    char ssid[32];
    char passwd[32];
    uint32_t ip;
    uint32_t netmask;
    uint32_t gateway;
} ModuleMessageWifiConfig; 

typedef union {
    ModuleDataAttr attr;
    ModuleMessageHelloWorld helloworld;
    ModuleMessageWifiConfig wifiConfig;
} ModuleMessage;

#endif

