#ifndef __Gateway_UART_PROJECT_H__
#define __Gateway_UART_PROJECT_H__

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

typedef int32_t (*UartPrint)(void *, const char *);

///////////////////////////////////////////////////
typedef int32_t (*UartSend)(void *oObj, 
        void *data, int32_t millis);
typedef int32_t (*UartRecv)(void *oObj, 
        void *data, int32_t *length, int32_t millis);

typedef struct {
    char tty[64];
    int32_t baudrate;
    int32_t c_flow;
    int32_t bits;
    int32_t stop;
    char parity;

    void *priv;
    UartSend send;
    UartRecv recv;
} UartConfig;

int32_t UartInitLog(void *priv, UartPrint print);
int32_t UartSetLogLevel(LogUart level);

void *UartInit(UartConfig *config);

#endif /* __Gateway_PROJECT_H__ */

