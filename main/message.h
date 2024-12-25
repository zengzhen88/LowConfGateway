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
Message *RequestMessage1(void **data, int32_t size);

//ModuleDataAttr_TriggerRecv
typedef struct {
    ModuleDataAttr attr;
} ModuleMessageTriggerRecv; 

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
    char module[128];
    char hardwareVer[128];
    char firmwareVer[128];
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

//ModuleDataAttr_NetState
typedef struct {
    ModuleDataAttr attr;
    NetState state;
    _NetState _state;
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
    uint32_t flag;
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
    uint32_t flag;
} ModuleMessageSetEthCfg;

//ModuleDataAttr_GetMqttCfg
typedef struct {
    ModuleDataAttr attr;
    char user[32];
    char password[32];
    char url[64];
    short port;
    char clientid[64];
} ModuleMessageGetMqttCfg;

//ModuleDataAttr_SetMqttCfg
typedef struct {
    ModuleDataAttr attr;
    char user[32];
    char password[32];
    char url[64];
    short port;
    char clientid[64];
} ModuleMessageSetMqttCfg;

//ModuleDataAttr_Ack
typedef struct {
    ModuleDataAttr attr;
    ModuleDataAttr srcAttr;
    int32_t status;
} ModuleMessageAck;

//ModuleDataAttr_PtSend
typedef struct {
    ModuleDataAttr attr;
    int seq;
    char mac[32];
    char data[64];
} ModuleMessagePtSend;

//ModuleDataAttr_GetScanTimeout
typedef struct {
    ModuleDataAttr attr;
    int sec;
} ModuleMessageGetScanTimeout;

//ModuleDataAttr_SetScanTimeout
typedef struct {
    ModuleDataAttr attr;
    int sec;
} ModuleMessageSetScanTimeout;

//ModuleDataAttr_SetREGEX
typedef struct {
    ModuleDataAttr attr;
    char data[128];
} ModuleMessageSetREGEX;

//ModuleDataAttr_GetREGEX
typedef struct {
    ModuleDataAttr attr;
    char data[128];
} ModuleMessageGetREGEX;

//ModuleDataAttr_ReportDebug
typedef struct {
    ModuleDataAttr attr;
    char data[128];
} ModuleMessageReportDebug;

//ModuleDataAttr_UpWifi
typedef struct {
    ModuleDataAttr attr;
} ModuleMessageUpWifi;

//ModuleDataAttr_DownWifi
typedef struct {
    ModuleDataAttr attr;
} ModuleMessageDownWifi;

//ModuleDataAttr_NetConnect
typedef struct {
    ModuleDataAttr attr;
} ModuleMessageNetConnect;

//ModuleDataAttr_NetDisconnect
typedef struct {
    ModuleDataAttr attr;
} ModuleMessageNetDisconnect;

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
    ModuleMessageGetScanTimeout getScanTimeout;
    ModuleMessageSetScanTimeout setScanTimeout;
    ModuleMessageSetREGEX setREGEX;
    ModuleMessageGetREGEX getREGEX;
    ModuleMessageReportDebug reportDebug;
    ModuleMessageUpWifi upWifi;
    ModuleMessageDownWifi downWifi;
    ModuleMessageNetConnect netConnect;
    ModuleMessageNetDisconnect netDisconnect;
    ModuleMessageAck ack;
} ModuleMessage;

typedef union {
    ModuleDataAttr attr;
    ModuleMessageTriggerRecv triggerRecv;
} ModuleInternalMessage;

#endif

