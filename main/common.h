#ifndef __COMMON_H__
#define __COMMON_H__ 

#include <stdint.h>

typedef enum {
    NetState_CONNSER, /*连接服务器*/
    NetState_CONNET,  /*网络连接,但未连接服务器*/
    NetState_UNNET,   /*网络没有连接*/
    NetState_Cnt,
} NetState;

typedef enum {
    DC,
    BAT,
    PowerCnt,
} PowerSupplyMode;

typedef enum {
    DataAttr_MqttToUart,
    DataAttr_MqttToWifi,
    DataAttr_MqttToEth,
    DataAttr_MqttToUpdate,
    DataAttr_WifiToMqtt,
    DataAttr_WifiToUart,
    DataAttr_UartToMqtt,
    DataAttr_EthToMqtt,
    DataAttr_SpiToMqtt,
    DataAttr_Cnt,
} DataAttr;

typedef enum {
    ModuleDataAttr_GetTemperature,
    ModuleDataAttr_GetModuleVersion,
    ModuleDataAttr_GetModuleInfo,
    ModuleDataAttr_SetModuleInfo,
    ModuleDataAttr_GetPower,
    ModuleDataAttr_Reboot,
    ModuleDataAttr_NetState,
    ModuleDataAttr_GetEthCfg,
    ModuleDataAttr_SetEthCfg,
    ModuleDataAttr_GetWifiCfg,
    ModuleDataAttr_SetWifiCfg,
    ModuleDataAttr_GetMqttCfg,
    ModuleDataAttr_SetMqttCfg,
    ModuleDataAttr_Update,
    ModuleDataAttr_ReportData,
    ModuleDataAttr_PtSend,
    ModuleDataAttr_Ack,
    ModuleDataAttr_Cnt,
} ModuleDataAttr;

typedef enum {
    ModuleInternalDataAttr_TriggerRecv,
    ModuleInternalDataAttr_Cnt,
} ModuleInternalDataAttr;

const char *toEnumString(ModuleDataAttr attr);
const char *toAckEnumString(ModuleDataAttr attr);
ModuleDataAttr toStringEnum(const char *strings);
const char *toEnumChineseString(ModuleDataAttr attr);
ModuleDataAttr toAckStringEnum(const char *strings);

typedef enum {
    DataTimeStatus_BLOCK = -1,
    DataTimeStatus_UNBLOCK,
} DataTimeStatus;

uint32_t IpStringToUInt32(char *ipString);
void IpUInt32ToString(uint32_t ipUInt, char *ip, int32_t ipLength);
uint64_t MacUInt8ToUInt64(uint8_t *macUInt8);
uint64_t MacStringToUInt64(char *macString);
void MacUInt64ToString(uint64_t macUInt64, char *mac, int32_t macLength);
void MacUInt64ToStrings(uint64_t macUInt64, char *mac, int32_t macLength);

const char *toNetStateEnumString(NetState state);

#endif /*__COMMON_H__*/
