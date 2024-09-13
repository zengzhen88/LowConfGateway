#ifndef TRANSMIT_H
#define TRANSMIT_H


float MQTTGetTemperature(void);
int32_t MQTTGetModuleVersion(char *version, int32_t size);
int32_t MQTTGetModuleInfo(char *info, int32_t size);
int32_t MQTTGetPower(PowerSupplyMode *mode, int *level);
const char *MQTTGetNetState();
int32_t MQTTGetEthCfg(char *address, char *netmask, char *gateway, int32_t size);
#endif // TRANSMIT_H
