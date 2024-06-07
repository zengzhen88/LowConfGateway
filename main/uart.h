#ifndef __UART_PROJECT_H__
#define __UART_PROJECT_H__

#include <common.h>

typedef enum {
    LogUART_None = 0,  //关闭日志输出 
    LogUART_Urgent,    //必须打的
    LogUART_Fatal,     //致使级
    LogUART_Error,     //错误级
    LogUART_Warning,   //告警级
    LogUART_Info,      //业务级
    LogUART_Debug,     //调试级
    LogUART_Trace,     //跟踪级
    LogUART_Detail,    //详细级
    LogUART_Cnt
} LogUART;

typedef enum {
    UARTNUM_0,
    UARTNUM_1, //暂时先定两个，后面看实际的情况调整
} UARTNUM;

typedef int32_t (*UARTPrint)(void *, const char *);
typedef int32_t (*UARTSigSend)(void *oObj,
        DataAttr attr, void *data, int32_t millis);
typedef int32_t (*UARTSigRecv)(void *oObj,
        DataAttr attr, void *data, int32_t millis);

int32_t UARTInitLog(void *priv, UARTPrint print);
int32_t UARTSetLogLevel(LogUART level);

typedef struct {
    char name[128];
    int baudRate;       /*波特率*/
    int dataBits;       /*数据位*/
    int parity;         /*校验位*/
    int stopBits;       /*停止位*/
    int flowCtrl;       /*硬件控*/
    int sourceClk;      /*源时钟*/
    UARTNUM uartIndex;
    UARTSigSend send;
    UARTSigRecv recv;
} UARTConfig;

void *UARTInit(UARTConfig *config);

#endif /* __UART_PROJECT_H__ */
