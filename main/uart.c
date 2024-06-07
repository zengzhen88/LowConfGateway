#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include <uart.h>
#include "driver/uart.h"
#include <message.h>

static void *gPriv = NULL;
static UARTPrint gPrint;
static LogUART gLevel = LogUART_Info;


/*************************************************/
/**************LogPrintf**************************/
/*************************************************/
static int UARTLogPrintf(LogUART level,
        const char *file, const char *func,
        int line, const char *format, ...) {
    char logBuf[128];
    va_list args;
    int funcLine        = 0;

    if (level > gLevel) return -1;

    snprintf (logBuf, sizeof(logBuf), "[%s][%s][%d]", file, func, line);
    funcLine = strlen(logBuf);

    /*va_list*/
    va_start(args, format);
    vsnprintf(&logBuf[funcLine], sizeof(logBuf) - funcLine, format, args);
    va_end(args);
    /*va_list*/

    if (gPrint) {
        return gPrint(gPriv, logBuf);
    }

    return -1;
}

#define LogPrintf(level, ...)\
    UARTLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogUART_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogUART_Debug, __VA_ARGS__);
#endif

#ifndef ERRP
#define ERRP(con, ret, flag, ...)               \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif

typedef struct {
    char *name;
    int32_t isRunning;

    int uartIndex;
    int baudRate;       /*波特率*/
    int dataBits;       /*数据位*/
    int parity;         /*校验位*/
    int stopBits;       /*停止位*/
    int flowCtrl;       /*硬件控*/
    int sourceClk;      /*源时钟*/

    char *buffer;
    int bufSize;
    char *restore;
    int fillLength;

    TaskHandle_t uartTask;

    UARTSigSend send;
    UARTSigRecv recv;
} UART;

//设置日志输出对象，主要看你想输出到哪里，就对应填充回调
int32_t UARTInitLog(void *priv, UARTPrint print) {
    gPriv   = priv;
    gPrint  = print;
    return 0;
}

//设置日志输出等级，看需要，调试、错误、宕机等
int32_t UARTSetLogLevel(LogUART level) {
    gLevel  = level;
    return 0;
}

char *UARTRecvParse(UART *uart, char *recv, int32_t length) {
    int32_t msgvalid    = 0;
    char *strings       = recv;
    char *chrr          = NULL;

    if (uart->fillLength > 0) {
        memcpy(strings - uart->fillLength, uart->restore, uart->fillLength);
        length  += uart->fillLength;
        strings -= uart->fillLength;
        strings[length] = '\0';
        uart->fillLength   = 0;
    }

    chrr = strchr(strings, '\r');
    if (chrr) {
        if (chrr[1] == '\n') {
            msgvalid = 1;
            chrr[0] = '\0';
            if (strlen(chrr + 2) > 0) {
                chrr += 2;
                memcpy(uart->restore, chrr, strlen(chrr));//只拷贝有效数据，不包括'\0';
                uart->fillLength = strlen(chrr);
            }
        }
        else {
            memcpy(uart->restore, strings, length);
            uart->fillLength += length;
        }
    }
    else {
        memcpy(uart->restore, strings, length);
        uart->fillLength += length;
    }

    if (msgvalid) {
        if (uart->send)
            uart->send(gPriv, DataAttr_UartToMqtt, 
                    strings, strlen(strings), DataTimeStatus_BLOCK);
        return strings;
    }

    return NULL;
}

void UARTSelectTask(void *args) {
    int length      = -1;
    int status      = -1;
    char *sbuffer   = NULL;
    char *bbuffer   = NULL;
    UART *uart      = (UART *)args;

    /*指向内存块的中间，方便后面组合数据*/
    sbuffer = (char *)&uart->buffer[uart->bufSize >> 1];

    while (1) {
        status = uart_read_bytes(uart->uartIndex, 
                sbuffer, ((uart->bufSize >> 1) - 1), 20 / portTICK_PERIOD_MS);
        if (status >= 0) {
            sbuffer[status] = '\0';
            bbuffer = UARTRecvParse(uart, sbuffer, status);
            if (NULL != bbuffer) {
                LogPrintf(LogUART_Info, "UART AT Recv :%s\n", bbuffer);
            }
            continue;
        }

        length = uart->bufSize;
        if (uart->recv) {
            status = uart->recv(gPriv, 
                    DataAttr_MqttToUart, uart->buffer, &length, 1);
            if (!status) {
                strcat(uart->buffer, "\r\n");
                LogPrintf(LogUART_Info, "UART AT Send: %s\n", uart->buffer);
                uart_write_bytes(uart->uartIndex, 
                        (const char *)uart->buffer, strlen(uart->buffer));
            }
        }
    }
}

void *UARTInit(UARTConfig *config) {
    esp_err_t status                = ESP_FAIL;        
    BaseType_t baseType             = pdFAIL;
    uart_config_t uart_config;

    UART *uart = (UART *) malloc (sizeof(*uart));
    ERRP(NULL == uart, return NULL, 1, " uartmalloc UART Instance failure\n");
    memset(uart, 0x0, sizeof(*uart));

    uart->send      = config->send;
    uart->recv      = config->recv;

    LogPrintf(LogUART_Info, "baud_rate     :%d\n", config->baudRate);
    LogPrintf(LogUART_Info, "data_bits     :%d\n", config->dataBits);
    LogPrintf(LogUART_Info, "parity        :%d\n", config->parity);
    LogPrintf(LogUART_Info, "stop_bits     :%d\n", config->stopBits);
    LogPrintf(LogUART_Info, "flow_ctrl     :%d\n", config->flowCtrl);
    LogPrintf(LogUART_Info, "source_clk    :%d\n", config->sourceClk);

    uart->baudRate  = config->baudRate;
    uart->dataBits  = config->dataBits;
    uart->parity    = config->parity;
    uart->stopBits  = config->stopBits;
    uart->flowCtrl  = config->flowCtrl;
    uart->sourceClk = config->sourceClk;
    uart->uartIndex = config->uartIndex;

    uart->name = (char *) malloc (strlen(config->name) + 1);
    ERRP(NULL == uart->name, goto ERR00, 1, "uart malloc name mem failure\n");
    strcpy(uart->name, config->name);

    status = uart_driver_install(config->uartIndex, 2048, 2048, 0, NULL, 0);
    ERRP(ESP_OK != status, goto ERR01, 1, "uart uart_driver_install failure\n");

    //这里需要确定具体的GPIO口
    /* status = uart_set_pin(config->uartIndex, GPI0, GPI0, GPI0, GPIO); */
    /* ERRP(ESP_OK != status, goto ERR02, 1, "uart uart_set_pin failure\n"); */

    uart_config.baud_rate   = uart->baudRate;
    uart_config.data_bits   = uart->dataBits;
    uart_config.parity      = uart->parity;
    uart_config.stop_bits   = uart->stopBits;
    uart_config.flow_ctrl   = uart->flowCtrl;
    uart_config.source_clk  = uart->sourceClk;

    uart->bufSize = 512;
    uart->buffer = (char *) malloc (uart->bufSize);
    ERRP(NULL == uart->buffer, goto ERR02, 1,
            "uart malloc buffer size:%d failure\n", uart->bufSize);

    uart->restore = (char *) malloc (uart->bufSize >> 1);
    ERRP(NULL == uart->restore, goto ERR03, 1,
            "uart malloc restore size:%d failure\n", uart->bufSize >> 1);

    uart_param_config(config->uartIndex, &uart_config);

    baseType = xTaskCreate(UARTSelectTask, 
            "uartSelectTask", 4096, uart, 5, &uart->uartTask);
    ERRP(pdPASS != baseType, goto ERR04, 1, "uart xTaskCreate failure\n");

    return uart;
ERR04:
    free(uart->restore);
ERR03:
    free(uart->buffer);
ERR02:
    uart_driver_delete(config->uartIndex);
ERR01:
    free(uart->name);
ERR00:
    free(uart);
    return NULL;
}

