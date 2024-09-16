#include <stdint.h>
#include <string.h>
#include <message.h>

float MQTTGetTemperature(void) {
    return 20.00000;
}

const char *MQTTGetModuleVersion(char *version, int32_t size) {
    strcpy(version, "10.0.0");
    return 0;
}

int32_t MQTTGetModuleInfo(char *info, int32_t size) {
    strcpy(info, "GZ248 Gateway");
    return 0;
}

int32_t MQTTGetPower(PowerSupplyMode *mode, int *level) {
    *mode = DC;
    *level = 100;

    return 0;
}

static const char *NetState2Name[] = {
    "CONNSER",
    "CONNET",
    "UNNET"
};

const char *MQTTGetNetState(void) {
    return NetState2Name[NetState_CONNET];
}

int32_t MQTTGetEthCfg(char *address, char *netmask, char *gateway, int32_t size) {
    strcpy(address, "192.168.0.108");
    strcpy(netmask, "255.255.255.0");
    strcpy(gateway, "192.168.0.1");
    return 0;
}


int32_t MQTTGetWifiCfg(char *ssid, char *password, char *address, char *netmask, char *gateway, int32_t size) {
    strcpy(ssid, "TP-LINK_342B");
    strcpy(password, "88888888");
    strcpy(address, "192.168.0.108");
    strcpy(netmask, "255.255.255.0");
    strcpy(gateway, "192.168.0.1");
    return 0;
}

int32_t MQTTGetMQTTCfg(char *user, char *password, char *url, int32_t size) {
    strcpy(user, "admin");
    strcpy(password, "123456");
    strcpy(url, "mqtt://192.168.0.107:1884");
    return 0;
}
