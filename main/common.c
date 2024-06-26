#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <common.h>
#include <string.h>

const char *toEnumString(ModuleDataAttr attr) {
    switch (attr) {
        case ModuleDataAttr_Update:
            return "Update";
        case ModuleDataAttr_GetTemperature:
            return "GetTemperature";
        case ModuleDataAttr_GetModuleVersion:
            return "GetModuleVersion";
        case ModuleDataAttr_GetModuleInfo:
            return "GetModuleInfo";
        case ModuleDataAttr_SetModuleInfo:
            return "SetModuleInfo";
        case ModuleDataAttr_GetPower:
            return "GetPower";
        case ModuleDataAttr_Reboot:
            return "Reboot";
        case ModuleDataAttr_NetState:
            return "NetState";
        case ModuleDataAttr_GetWifiCfg:
            return "GetWifiCfg";
        case ModuleDataAttr_SetWifiCfg:
            return "SetWifiCfg";
        case ModuleDataAttr_GetEthCfg:
            return "GetEthCfg";
        case ModuleDataAttr_SetEthCfg:
            return "SetEthCfg";
        case ModuleDataAttr_PtSend:
            return "PtSend";
        default:return "Ack";
    }
}

const char *toAckEnumString(ModuleDataAttr attr) {
    switch (attr) {
        case ModuleDataAttr_Update:
            return "UpdateAck";
        case ModuleDataAttr_GetTemperature:
            return "GetTemperatureAck";
        case ModuleDataAttr_GetModuleVersion:
            return "GetModuleVersionAck";
        case ModuleDataAttr_GetModuleInfo:
            return "GetModuleInfoAck";
        case ModuleDataAttr_SetModuleInfo:
            return "SetModuleInfoAck";
        case ModuleDataAttr_GetPower:
            return "GetPowerAck";
        case ModuleDataAttr_Reboot:
            return "RebootAck";
        case ModuleDataAttr_NetState:
            return "NetStateAck";
        case ModuleDataAttr_GetWifiCfg:
            return "GetWifiCfgAck";
        case ModuleDataAttr_SetWifiCfg:
            return "SetWifiCfgAck";
        case ModuleDataAttr_GetEthCfg:
            return "GetEthCfgAck";
        case ModuleDataAttr_SetEthCfg:
            return "SetEthCfgAck";
        case ModuleDataAttr_PtSend:
            return "PtSendAck";
        default:return "Ack";
    }
}

ModuleDataAttr toStringEnum(const char *strings) {
    if (strstr(strings, "Update")) {
        return ModuleDataAttr_Update;
    }
    else if (strstr(strings, "GetTemperature")) {
        return ModuleDataAttr_GetTemperature;
    }
    else if (strstr(strings, "GetModuleVersion")) {
        return ModuleDataAttr_GetModuleVersion;
    }
    else if (strstr(strings, "GetModuleInfo")) {
        return ModuleDataAttr_GetModuleInfo;
    }
    else if (strstr(strings, "SetModuleInfo")) {
        return ModuleDataAttr_SetModuleInfo;
    }
    else if (strstr(strings, "GetPower")) {
        return ModuleDataAttr_GetPower;
    }
    else if (strstr(strings, "Reboot")) {
        return ModuleDataAttr_Reboot;
    }
    else if (strstr(strings, "NetState")) {
        return ModuleDataAttr_NetState;
    }
    else if (strstr(strings, "GetWifiCfg")) {
        return ModuleDataAttr_GetWifiCfg;
    }
    else if (strstr(strings, "SetWifiCfg")) {
        return ModuleDataAttr_SetWifiCfg;
    }
    else if (strstr(strings, "GetEthCfg")) {
        return ModuleDataAttr_GetEthCfg;
    }
    else if (strstr(strings, "SetEthCfg")) {
        return ModuleDataAttr_SetEthCfg;
    }
    else if (strstr(strings, "PtSend")) {
        return ModuleDataAttr_PtSend;
    }
    else {
        return ModuleDataAttr_Cnt;
    }

    return ModuleDataAttr_Cnt;
}

const char *toNetStateEnumString(NetState state) {
    switch (state) {
        case NetState_CONNSER:
            return "CONNSER";
        case NetState_CONNET:
            return "CONNET";
        case NetState_UNNET:
            return "UNNET";
        default:return "Invalid";
    }
}


uint32_t IpStringToUInt32(char *ipString) {
    int32_t index   = 0;
    char *ipverc[4] = {NULL};
    char *tptr      = NULL;
    char *ptr       = ipString;
    uint32_t ip     = 0;

    if (ptr) {
        index = 0;
        for (tptr = strtok(ptr, "."); tptr != NULL; tptr = strtok(NULL, ".")) {
            ipverc[index++] = tptr;
        }

        ip |= (strtoull(ipverc[0], &tptr, 0) << 24);
        ip |= (strtoull(ipverc[1], &tptr, 0) << 16);
        ip |= (strtoull(ipverc[2], &tptr, 0) << 8);
        ip |= (strtoull(ipverc[3], &tptr, 0) << 0);

        return ip;
    }

    return 0;
}

void IpUInt32ToString(uint32_t ipUInt, char *ip, int32_t ipLength) {
    snprintf (ip, ipLength - 1, "%d.%d.%d.%d", 
                (unsigned int)ipUInt >> 24, 
                (unsigned int)((ipUInt & 0x00FFFFFF) >> 16),
                (unsigned int)((ipUInt & 0x0000FFFF) >> 8),
                (unsigned int)((ipUInt & 0x000000FF) >> 0));
    ip[ipLength - 1] = '\0';
}

uint64_t MacUInt8ToUInt64(uint8_t *macUInt8) {
    uint64_t mac                = 0;

    /*
     * printf ("macString:%02x %02x %02x %02x %02x %02x\n", 
     *         macUInt8[0], 
     *         macUInt8[1], 
     *         macUInt8[2], 
     *         macUInt8[3], 
     *         macUInt8[4], 
     *         macUInt8[5]);
     */
    mac |= ((uint64_t)macUInt8[0] << 40);
    mac |= ((uint64_t)macUInt8[1] << 32);
    mac |= ((uint64_t)macUInt8[2] << 24);
    mac |= ((uint64_t)macUInt8[3] << 16);
    mac |= ((uint64_t)macUInt8[4] << 8);
    mac |= ((uint64_t)macUInt8[5] << 0);

    return mac;
}

uint64_t MacStringToUInt64(char *macString) {
    int32_t index       = 0;
    uint64_t mac        = 0;
    char *tptr          = NULL;
    char ipverc[6][3]   = {0};

    for (index = 0; index < 6; index++) {
        ipverc[index][0]   = macString[2 * index + 0];
        ipverc[index][1]   = macString[2 * index + 1];
    }

    mac |= (strtoull((const char *)ipverc[0], &tptr, 16) << 40);
    mac |= (strtoull((const char *)ipverc[1], &tptr, 16) << 32);
    mac |= (strtoull((const char *)ipverc[2], &tptr, 16) << 24);
    mac |= (strtoull((const char *)ipverc[3], &tptr, 16) << 16);
    mac |= (strtoull((const char *)ipverc[4], &tptr, 16) << 8);
    mac |= (strtoull((const char *)ipverc[5], &tptr, 16) << 0);

    return mac;
}

void MacUInt64ToString(uint64_t macUInt64, char *mac, int32_t macLength) {
    snprintf (mac, macLength - 1, "%02X:%02X:%02X:%02X:%02X:%02X", 
                (uint8_t)((uint64_t)macUInt64 >> 40), 
                (uint8_t)((uint64_t)((macUInt64 & 0xFF00000000) >> 32)),
                (uint8_t)((uint64_t)((macUInt64 & 0xFF000000) >> 24)),
                (uint8_t)((uint64_t)((macUInt64 & 0xFF0000) >> 16)),
                (uint8_t)((uint64_t)((macUInt64 & 0xFF00) >> 8)),
                (uint8_t)((uint64_t)((macUInt64 & 0xFF) >> 0)));
    mac[macLength - 1] = '\0';
}

void MacUInt64ToStrings(uint64_t macUInt64, char *mac, int32_t macLength) {
    snprintf (mac, macLength - 1, "%02X%02X%02X%02X%02X%02X", 
                (uint8_t)((uint64_t)macUInt64 >> 40), 
                (uint8_t)((uint64_t)((macUInt64 & 0xFF00000000) >> 32)),
                (uint8_t)((uint64_t)((macUInt64 & 0xFF000000) >> 24)),
                (uint8_t)((uint64_t)((macUInt64 & 0xFF0000) >> 16)),
                (uint8_t)((uint64_t)((macUInt64 & 0xFF00) >> 8)),
                (uint8_t)((uint64_t)((macUInt64 & 0xFF) >> 0)));
    mac[macLength - 1] = '\0';
}
