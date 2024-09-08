#include <stdint.h>
#include <message.h>

float MQTTGetTemperature(void) {
    return 20.00000;
}

const char *MQTTGetModuleVersion(void) {
    return "10.0.0";
}

const char *MQTTGetModuleInfo(void) {
    return "GZ248 Gateway";
}

int32_t MQTTGetPower(PowerSupplyMode *mode, int *level) {
    *mode = DC;
    *level = 100;

    return 0;
}
