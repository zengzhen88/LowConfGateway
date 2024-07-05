#ifndef __Eth_PROJECT_H__
#define __Eth_PROJECT_H__

#include <common.h>

typedef enum {
    LogEth_None = 0,  //关闭日志输出 
    LogEth_Urgent,    //必须打的
    LogEth_Fatal,     //致使级
    LogEth_Error,     //错误级
    LogEth_Warning,   //告警级
    LogEth_Info,      //业务级
    LogEth_Debug,     //调试级
    LogEth_Trace,     //跟踪级
    LogEth_Detail,    //详细级
    LogEth_Cnt
} LogEth;

typedef int32_t (*EthPrint)(void *, const char *);
typedef int32_t (*EthSigSend)(void *oObj,
        DataAttr attr, void *data, 
        int32_t fillLength, int32_t millis);
typedef int32_t (*EthSigRecv)(void *oObj,
        DataAttr attr, void *data, 
        int32_t *fillLength, int32_t millis);

int32_t EthInitLog(void *priv, EthPrint print);
int32_t EthSetLogLevel(LogEth level);

typedef struct {
    char address[32];
    char netmask[32];
    char gateway[32];
    EthSigSend send;
    EthSigRecv recv;
} EthConfig;

void *EthInit(EthConfig *config);
int32_t EthTriggerRecv(void *arg);

#endif /* __Eth_PROJECT_H__ */

