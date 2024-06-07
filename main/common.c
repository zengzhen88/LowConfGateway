#include <stdio.h>
#include <stdlib.h>
#include <common.h>

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
