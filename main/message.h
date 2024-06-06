#ifndef __MESSAGE_H__
#define __MESSAGE_H__ 

#include <common.h>

typedef struct {
    DataAttr attr;
    uint32_t ip;
    uint32_t netmask;
    uint32_t gateway;
} WifiMessage;

typedef union {
    DataAttr attr;
    WifiMessage wifiConfig;
} Message;

Message *RequestMessage(DataAttr attr, int32_t *size, int32_t count);
int32_t ReleaseMessage(Message *mess);

#endif

