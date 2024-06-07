#ifndef __COMMON_H__
#define __COMMON_H__ 

typedef enum {
    DataAttr_Wifi,
    DataAttr_Uart,
} DataAttr;

typedef enum {
    DataTimeStatus_BLOCK = -1,
    DataTimeStatus_UNBLOCK,
} DataTimeStatus;

#endif /*__COMMON_H__*/
