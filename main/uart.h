#ifndef __Uart_PROJECT_H__
#define __Uart_PROJECT_H__

#include <common.h>

typedef enum {
    LogUart_None = 0,  //关闭日志输出 
    LogUart_Urgent,    //必须打的
    LogUart_Fatal,     //致使级
    LogUart_Error,     //错误级
    LogUart_Warning,   //告警级
    LogUart_Info,      //业务级
    LogUart_Debug,     //调试级
    LogUart_Trace,     //跟踪级
    LogUart_Detail,    //详细级
    LogUart_Cnt
} LogUart;

typedef enum {
    UartNUM_0,
    UartNUM_1, //暂时先定两个，后面看实际的情况调整
} UartNUM;

typedef int32_t (*UartPrint)(void *, const char *);
typedef int32_t (*UartSigSend)(void *oObj,
        DataAttr attr, void *data, 
        int32_t fillLength, int32_t millis);
typedef int32_t (*UartSigRecv)(void *oObj,
        DataAttr attr, void *data,
        int32_t *fillLength, int32_t millis);

int32_t UartInitLog(void *priv, UartPrint print);
int32_t UartSetLogLevel(LogUart level);

typedef struct {
    char name[128];
    int baudRate;       /*波特率*/
    int dataBits;       /*数据位*/
    int parity;         /*校验位*/
    int stopBits;       /*停止位*/
    int flowCtrl;       /*硬件控*/
    int sourceClk;      /*源时钟*/
    UartNUM uartIndex;
    UartSigSend send;
    UartSigRecv recv;
} UartConfig;

void *UartInit(UartConfig *config);

#endif /* __Uart_PROJECT_H__ */
