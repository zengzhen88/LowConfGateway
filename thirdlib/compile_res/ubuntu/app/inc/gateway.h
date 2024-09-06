#ifndef __Gateway_PROJECT_H__
#define __Gateway_PROJECT_H__

typedef enum {
    LogGateway_None = 0,  //关闭日志输出 
    LogGateway_Urgent,    //必须打的
    LogGateway_Fatal,     //致使级
    LogGateway_Error,     //错误级
    LogGateway_Warning,   //告警级
    LogGateway_Info,      //业务级
    LogGateway_Debug,     //调试级
    LogGateway_Trace,     //跟踪级
    LogGateway_Detail,    //详细级
    LogGateway_Cnt
} LogGateway;

typedef int32_t (*GatewayPrint)(void *, const char *);

int32_t GatewayInitLog(void *priv, GatewayPrint print);
int32_t GatewaySetLogLevel(LogGateway level);

void *GatewayInit(void);

#endif /* __Gateway_PROJECT_H__ */

