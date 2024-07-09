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

//ModuleDataAttr_TriggerRecv
typedef struct {
    ModuleDataAttr attr;
} ModuleMessageTriggerRecv; 

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

//ModuleDataAttr_GetPower
typedef struct {
    ModuleDataAttr attr;
    PowerSupplyMode mode;
    int level;
} ModuleMessageGetPower;

/*
 * //ModuleDataAttr_GetPower
 * typedef struct {
 *     ModuleDataAttr attr;
 *     PowerSupplyMode mode;
 *     int level;
 * } ModuleMessageGetPower;
 */

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

//ModuleDataAttr_GetMqttCfg
typedef struct {
    ModuleDataAttr attr;
    char user[32];
    char password[32];
    char url[64];
} ModuleMessageGetMqttCfg;

//ModuleDataAttr_SetMqttCfg
typedef struct {
    ModuleDataAttr attr;
    char user[32];
    char password[32];
    char url[64];
} ModuleMessageSetMqttCfg;

/*
 * //ModuleDataAttr_SendData
 * typedef struct {
 *     ModuleDataAttr attr;
 *     char user[32];
 *     char password[32];
 *     char url[64];
 * } ModuleMessageSetMqttCfg;
 */

//ModuleDataAttr_Ack
typedef struct {
    ModuleDataAttr attr;
    ModuleDataAttr srcAttr;
    int32_t status;
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
    ModuleMessageTriggerRecv triggerRecv;
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
    ModuleMessageGetMqttCfg getMqttCfg;
    ModuleMessageSetMqttCfg setMqttCfg;
    ModuleMessagePtSend ptSend;
    ModuleMessageAck ack;
} ModuleMessage;

typedef union {
    ModuleDataAttr attr;
    ModuleMessageTriggerRecv triggerRecv;
} ModuleInternalMessage;

#endif

