#ifndef TRANSMIT_H
#define TRANSMIT_H


float MQTTGetTemperature(void);
const char *MQTTGetModuleVersion(void);
const char *MQTTGetModuleInfo(void);
int32_t MQTTGetPower(PowerSupplyMode *mode, int *level);
#endif // TRANSMIT_H
