#ifndef __COMMON_H__
#define __COMMON_H__ 

typedef enum {
    DataAttr_MqttToUart,
    DataAttr_MqttToWifi,
    DataAttr_WifiToMqtt,
    DataAttr_UartToMqtt,
    DataAttr_Cnt,
} DataAttr;

typedef enum {
    ModuleDataAttr_helloworld,
    ModuleDataAttr_SetWifiConfig,
    ModuleDataAttr_GetWifiConfig,
    ModuleDataAttr_Cnt,
} ModuleDataAttr;
const char *toEnumString(ModuleDataAttr attr);
ModuleDataAttr toStringEnum(const char *strings);

typedef enum {
    DataTimeStatus_BLOCK = -1,
    DataTimeStatus_UNBLOCK,
} DataTimeStatus;

#endif /*__COMMON_H__*/
