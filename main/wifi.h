#ifndef __Wifi_PROJECT_H__
#define __Wifi_PROJECT_H__

#include <common.h>

typedef enum {
    LogWifi_None = 0,  //关闭日志输出 
    LogWifi_Urgent,    //必须打的
    LogWifi_Fatal,     //致使级
    LogWifi_Error,     //错误级
    LogWifi_Warning,   //告警级
    LogWifi_Info,      //业务级
    LogWifi_Debug,     //调试级
    LogWifi_Trace,     //跟踪级
    LogWifi_Detail,    //详细级
    LogWifi_Cnt
} LogWifi;

typedef int32_t (*WifiPrint)(void *, const char *);
typedef int32_t (*WifiSigSend)(void *oObj,
        DataAttr attr, void *data, int32_t millis);
typedef int32_t (*WifiSigRecv)(void *oObj,
        DataAttr attr, void *data, int32_t millis);

int32_t WifiInitLog(void *priv, WifiPrint print);
int32_t WifiSetLogLevel(LogWifi level);

typedef struct {
    char ssid[128];
    char password[128];
    WifiSigSend send;
    WifiSigRecv recv;
} WifiConfig;

void *WifiInit(WifiConfig *config);

#endif /* __Wifi_PROJECT_H__ */

