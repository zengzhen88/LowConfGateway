#include <stdio.h>
#include <stdlib.h>
#include <common.h>
#include <string.h>

const char *toEnumString(ModuleDataAttr attr) {
    switch (attr) {
        case ModuleDataAttr_helloworld:
            return "helloworld";
        case ModuleDataAttr_SetWifiConfig:
            return "SetWifiConfig";
        case ModuleDataAttr_GetWifiConfig:
            return "GetWifiConfig";
        default:return "Invalid";
    }
}

ModuleDataAttr toStringEnum(const char *strings) {
    if (!strcmp(strings, "GetWifiConfig")) {
        return ModuleDataAttr_GetWifiConfig;
    }
    else if (!strcmp(strings, "SetWifiConfig")) {
        return ModuleDataAttr_SetWifiConfig;
    }
    else {

    }

    return ModuleDataAttr_Cnt;
}
