#ifndef __COMMON_H__
#define __COMMON_H__ 

#ifdef __cplusplus //而这一部分就是告诉编译器，如果定义了__cplusplus(即如果是cpp文件，
extern "C"{ //因为cpp文件默认定义了该宏),则采用C语言方式进行编译
#endif
#include <stdint.h>

typedef enum {
    NetState_CONNSER, /*连接服务器*/
    NetState_CONNET,  /*网络连接,但未连接服务器*/
    NetState_UNNET,   /*网络没有连接*/
    NetState_Cnt,
} NetState;

typedef enum {
    _NetState_WifiNetConnect,
    _NetState_WifiNetUnconnect,
    _NetState_EthNetConnect,
    _NetState_EthNetUnconnect,
    _NetState_MqttConnect,
    _NetState_MqttUnconnect,
} _NetState;

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
    DataAttr_EthToUart,
    DataAttr_EthToWifi,
    DataAttr_SpiToMqtt,
    DataAttr_mainToUart,
    DataAttr_uartToMain,
    DataAttr_Cnt,
} DataAttr;

typedef enum {
    ModuleDataAttr_GetTemperature, //0
    ModuleDataAttr_GetModuleVersion,
    ModuleDataAttr_GetModuleInfo,
    ModuleDataAttr_SetModuleInfo,
    ModuleDataAttr_GetPower,
    ModuleDataAttr_Reboot,//5
    ModuleDataAttr_GetEthCfg,
    ModuleDataAttr_SetEthCfg,
    ModuleDataAttr_GetWifiCfg,
    ModuleDataAttr_SetWifiCfg,
    ModuleDataAttr_GetMqttCfg,//10
    ModuleDataAttr_SetMqttCfg,
    ModuleDataAttr_Update,
    ModuleDataAttr_PtSend,
    ModuleDataAttr_SetScanTimeout,
    ModuleDataAttr_GetScanTimeout,//15
    ModuleDataAttr_NetState,
    ModuleDataAttr_PtRecv,
    ModuleDataAttr_TransmitData,
    ModuleDataAttr_ReportTemperature,
    ModuleDataAttr_ReportModuleInfo,//20
    ModuleDataAttr_ReportPower,
    ModuleDataAttr_ReportEthCfg,
    ModuleDataAttr_ReportWifiCfg,
    ModuleDataAttr_ReportMqttCfg,
    ModuleDataAttr_ReportTransmitData,//25
    ModuleDataAttr_ReportScanTimeout,
    ModuleDataAttr_SetREGEX,
    ModuleDataAttr_GetREGEX,
    ModuleDataAttr_ReportREGEX,
    ModuleDataAttr_ReportDebug,//30
    ModuleDataAttr_DownWifi,
    ModuleDataAttr_UpWifi,
    ModuleDataAttr_NetConnect,
    ModuleDataAttr_NetDisconnect,
    ModuleDataAttr_GetConfDown,//35
    ModuleDataAttr_Ack,//36
    ModuleDataAttr_Cnt,//37
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
ModuleDataAttr toChineseStringEnum(const char *strings);
int is_ipv4_addr(const char *ip);

#ifdef __cplusplus
}
#endif

#endif /*__COMMON_H__*/
