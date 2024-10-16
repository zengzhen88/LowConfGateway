#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <common.h>
#include <string.h>

const char *toEnumChineseString(ModuleDataAttr attr) {
    switch (attr) {
//        case ModuleDataAttr_ReportData:
//            return "上报数据";
        case ModuleDataAttr_Update:
            return "升级系统";
        case ModuleDataAttr_GetTemperature:
            return "查询温度";
        case ModuleDataAttr_GetModuleVersion:
            return "查询模块版本";
        case ModuleDataAttr_GetModuleInfo:
            return "查询模块信息";
        case ModuleDataAttr_SetModuleInfo:
            return "设置模块信息";
        case ModuleDataAttr_GetPower:
            return "查询模块供电";
        case ModuleDataAttr_Reboot:
            return "模块重启";
        case ModuleDataAttr_GetWifiCfg:
            return "查询无线配置";
        case ModuleDataAttr_SetWifiCfg:
            return "设置无线配置";
        case ModuleDataAttr_GetEthCfg:
            return "查询有线配置";
        case ModuleDataAttr_SetEthCfg:
            return "设置有线配置";
        case ModuleDataAttr_GetMqttCfg:
            return "查询MQTT配置";
        case ModuleDataAttr_SetMqttCfg:
            return "设置MQTT配置";
        case ModuleDataAttr_PtSend:
            return "发送下发数据";
        //case ModuleDataAttr_PtRecv:
        //    return "下发数据响应";
        case ModuleDataAttr_SetScanTimeout:
            return "设置下发数据超时时间";
        case ModuleDataAttr_GetScanTimeout:
            return "查询下发数据超时时间";
        //case ModuleDataAttr_NetState:
        //    return "查询网络状态";
        default:return "Ack";
    }
}

const char *toEnumString(ModuleDataAttr attr) {
    switch (attr) {
        case ModuleDataAttr_ReportData:
            return "ReportData";
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
        case ModuleDataAttr_GetMqttCfg:
            return "GetMqttCfg";
        case ModuleDataAttr_SetMqttCfg:
            return "SetMqttCfg";
        case ModuleDataAttr_PtSend:
            return "PtSend";
        case ModuleDataAttr_PtRecv:
            return "PtRecv";
        case ModuleDataAttr_SetScanTimeout:
            return "SetScanTimeout";
        case ModuleDataAttr_GetScanTimeout:
            return "GetScanTimeout";
        default:return "Ack";
    }
}

const char *toAckEnumString(ModuleDataAttr attr) {
    switch (attr) {
        case ModuleDataAttr_ReportData:
            return "ReportDataAck";
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
        case ModuleDataAttr_GetMqttCfg:
            return "GetMqttCfgAck";
        case ModuleDataAttr_SetMqttCfg:
            return "SetMqttCfgAck";
        case ModuleDataAttr_PtSend:
            return "PtSendAck";
        case ModuleDataAttr_PtRecv:
            return "PtRecvAck";
        case ModuleDataAttr_SetScanTimeout:
            return "SetScanTimeoutAck";
        case ModuleDataAttr_GetScanTimeout:
            return "GetScanTimeoutAck";
        default:return "Ack";
    }
}

ModuleDataAttr toAckStringEnum(const char *strings) {
    if (strstr(strings, "UpdateAck")) {
        return ModuleDataAttr_Update;
    }
    else if (strstr(strings, "ReportDataAck")) {
        return ModuleDataAttr_ReportData;
    }
    else if (strstr(strings, "GetTemperatureAck")) {
        return ModuleDataAttr_GetTemperature;
    }
    else if (strstr(strings, "GetModuleVersionAck")) {
        return ModuleDataAttr_GetModuleVersion;
    }
    else if (strstr(strings, "GetModuleInfoAck")) {
        return ModuleDataAttr_GetModuleInfo;
    }
    else if (strstr(strings, "SetModuleInfoAck")) {
        return ModuleDataAttr_SetModuleInfo;
    }
    else if (strstr(strings, "GetPowerAck")) {
        return ModuleDataAttr_GetPower;
    }
    else if (strstr(strings, "RebootAck")) {
        return ModuleDataAttr_Reboot;
    }
    else if (strstr(strings, "NetStateAck")) {
        return ModuleDataAttr_NetState;
    }
    else if (strstr(strings, "GetWifiCfgAck")) {
        return ModuleDataAttr_GetWifiCfg;
    }
    else if (strstr(strings, "SetWifiCfgAck")) {
        return ModuleDataAttr_SetWifiCfg;
    }
    else if (strstr(strings, "GetEthCfgAck")) {
        return ModuleDataAttr_GetEthCfg;
    }
    else if (strstr(strings, "SetEthCfgAck")) {
        return ModuleDataAttr_SetEthCfg;
    }
    else if (strstr(strings, "GetMqttCfgAck")) {
        return ModuleDataAttr_GetMqttCfg;
    }
    else if (strstr(strings, "SetMqttCfgAck")) {
        return ModuleDataAttr_SetMqttCfg;
    }
    else if (strstr(strings, "PtSendAck")) {
        return ModuleDataAttr_PtSend;
    }
    else if (strstr(strings, "PtRecvAck")) {
        return ModuleDataAttr_PtRecv;
    }
    else if (strstr(strings, "SetScanTimeoutAck")) {
        return ModuleDataAttr_SetScanTimeout;
    }
    else if (strstr(strings, "GetScanTimeoutAck")) {
        return ModuleDataAttr_GetScanTimeout;
    }
    else {
        return ModuleDataAttr_Cnt;
    }

    return ModuleDataAttr_Cnt;
}

ModuleDataAttr toChineseStringEnum(const char *strings) {
    if (strstr(strings, "升级系统")) {
        return ModuleDataAttr_Update;
    }
    else if (strstr(strings, "查询温度")) {
        return ModuleDataAttr_GetTemperature;
    }
    else if (strstr(strings, "查询模块版本")) {
        return ModuleDataAttr_GetModuleVersion;
    }
    else if (strstr(strings, "查询模块信息")) {
        return ModuleDataAttr_GetModuleInfo;
    }
    else if (strstr(strings, "设置模块信息")) {
        return ModuleDataAttr_SetModuleInfo;
    }
    else if (strstr(strings, "查询模块供电")) {
        return ModuleDataAttr_GetPower;
    }
    else if (strstr(strings, "模块重启")) {
        return ModuleDataAttr_Reboot;
    }
    else if (strstr(strings, "查询无线配置")) {
        return ModuleDataAttr_GetWifiCfg;
    }
    else if (strstr(strings, "设置MQTT配置")) {
        return ModuleDataAttr_SetMqttCfg;
    }
    else if (strstr(strings, "查询MQTT配置")) {
        return ModuleDataAttr_GetMqttCfg;
    }
    else if (strstr(strings, "设置无线配置")) {
        return ModuleDataAttr_SetWifiCfg;
    }
    else if (strstr(strings, "查询有线配置")) {
        return ModuleDataAttr_GetEthCfg;
    }
    else if (strstr(strings, "设置有线配置")) {
        return ModuleDataAttr_SetEthCfg;
    }
    else if (strstr(strings, "发送下发数据")) {
        return ModuleDataAttr_PtSend;
    }
    else if (strstr(strings, "设置下发数据超时时间")) {
        return ModuleDataAttr_SetScanTimeout;
    }
    else if (strstr(strings, "查询下发数据超时时间")) {
        return ModuleDataAttr_GetScanTimeout;
    }
    else {
        return ModuleDataAttr_Cnt;
    }

    return ModuleDataAttr_Cnt;
}

ModuleDataAttr toStringEnum(const char *strings) {
    if (strstr(strings, "Update")) {
        return ModuleDataAttr_Update;
    }
    else if (strstr(strings, "ReportData")) {
        return ModuleDataAttr_ReportData;
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
    else if (strstr(strings, "SetMqttCfg")) {
        return ModuleDataAttr_SetMqttCfg;
    }
    else if (strstr(strings, "GetMqttCfg")) {
        return ModuleDataAttr_GetMqttCfg;
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
    else if (strstr(strings, "PtRecv")) {
        return ModuleDataAttr_PtRecv;
    }
    else if (strstr(strings, "SetScanTimeout")) {
        return ModuleDataAttr_SetScanTimeout;
    }
    else if (strstr(strings, "GetScanTimeout")) {
        return ModuleDataAttr_GetScanTimeout;
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
