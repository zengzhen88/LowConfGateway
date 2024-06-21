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

//ModuleDataAttr_SetEthConfig
//ModuleDataAttr_GetEthConfig
typedef struct {
    ModuleDataAttr attr;
    uint32_t ip;
    uint32_t netmask;
    uint32_t gateway;
} ModuleMessageEthConfig; 

//ModuleDataAttr_Update
typedef struct {
    ModuleDataAttr attr;
    char url[128];
} ModuleMessageUpdate;

//ModuleDataAttr_GetTemperature
typedef struct {
    ModuleDataAttr attr;
    float temperature;
} ModuleMessageGetTemperature;

//ModuleDataAttr_GetModuleVersion
typedef struct {
    ModuleDataAttr attr;
    char ver[128];
} ModuleMessageGetModuleVersion;

//ModuleDataAttr_GetModuleInfo
typedef struct {
    ModuleDataAttr attr;
    char info[128];
} ModuleMessageGetModuleInfo;

//ModuleDataAttr_SetModuleInfo
typedef struct {
    ModuleDataAttr attr;
    char info[128];
} ModuleMessageSetModuleInfo;

typedef enum {
    DC,
    BAT,
    PowerCnt,
} PowerSupplyMode;

//ModuleDataAttr_GetPower
typedef struct {
    ModuleDataAttr attr;
    PowerSupplyMode mode;
    int level;
} ModuleMessageGetPower;

//ModuleDataAttr_GetPower
typedef struct {
    ModuleDataAttr attr;
    PowerSupplyMode mode;
    int level;
} ModuleMessageGetPower;

typedef enum {
    NetState_CONNSER, /*连接服务器*/
    NetState_CONNET,  /*网络连接,但未连接服务器*/
    NetState_UNNET,   /*网络没有连接*/
    NetState_Cnt,
} NetState;

//ModuleDataAttr_NetState
typedef struct {
    ModuleDataAttr attr;
    NetState state;
} ModuleMessageNetState;

//ModuleDataAttr_GetWifiCfg
typedef struct {
    ModuleDataAttr attr;
    char ssid[32];
    char password[32];
    char address[32];
    char netmask[32];
    char gateway[32];
} ModuleMessageGetWifiCfg;

//ModuleDataAttr_SetWifiCfg
typedef struct {
    ModuleDataAttr attr;
    char ssid[32];
    char password[32];
    char address[32];
    char netmask[32];
    char gateway[32];
} ModuleMessageSetWifiCfg;

//ModuleDataAttr_GetEthCfg
typedef struct {
    ModuleDataAttr attr;
    char address[32];
    char netmask[32];
    char gateway[32];
} ModuleMessageGetEthCfg;

//ModuleDataAttr_SetEthCfg
typedef struct {
    ModuleDataAttr attr;
    char address[32];
    char netmask[32];
    char gateway[32];
} ModuleMessageSetEthCfg;

//ModuleDataAttr_Ack
typedef struct {
    ModuleDataAttr attr;
} ModuleMessageAck;

//ModuleDataAttr_PtSend
typedef struct {
    ModuleDataAttr attr;
    int timeOut;
    char mac[32];
    char data[64];
} ModuleMessagePtSend;

typedef union {
    ModuleDataAttr attr;
    ModuleMessageHelloWorld helloworld;
    ModuleMessageUpdate update;
    ModuleMessageGetTemperature getTemperature;
    ModuleMessageGetModuleVersion getModuleVersion;
    ModuleMessageGetModuleInfo getModuleInfo;
    ModuleMessageSetModuleInfo setModuleInfo;
    ModuleMessageGetPower getPower;
    ModuleMessageNetState netState;
    ModuleMessageGetWifiCfg getWifiCfg;
    ModuleMessageSetWifiCfg setWifiCfg;
    ModuleMessageGetEthCfg getEthCfg;
    ModuleMessageSetEthCfg setEthCfg;
    ModuleMessagePtSend ptSend;
    ModuleMessageAck ack;
} ModuleMessage;

#endif

