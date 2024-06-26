#ifndef __COMMON_H__
#define __COMMON_H__ 

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
    DataAttr_UartToMqtt,
    DataAttr_EthToMqtt,
    DataAttr_Cnt,
} DataAttr;

typedef enum {
    ModuleDataAttr_helloworld,
    ModuleDataAttr_Ack,
    ModuleDataAttr_Update,
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
    ModuleDataAttr_PtSend,
    ModuleDataAttr_Cnt,
} ModuleDataAttr;
const char *toEnumString(ModuleDataAttr attr);
const char *toAckEnumString(ModuleDataAttr attr);
ModuleDataAttr toStringEnum(const char *strings);

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
