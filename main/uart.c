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
static UartPrint gPrint;
static LogUart gLevel = LogUart_Info;


/*************************************************/
/**************LogPrintf**************************/
/*************************************************/
static int UartLogPrintf(LogUart level,
        const char *file, const char *func,
        int line, const char *format, ...) {
    char logBuf[256];
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
    UartLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogUart_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogUart_Debug, __VA_ARGS__);
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

#define Uart_TEST
#ifdef Uart_TEST
    char *uartAck;
    int ackSize;
#endif

    UartSigSend send;
    UartSigRecv recv;
} Uart;

//设置日志输出对象，主要看你想输出到哪里，就对应填充回调
int32_t UartInitLog(void *priv, UartPrint print) {
    gPriv   = priv;
    gPrint  = print;
    return 0;
}

//设置日志输出等级，看需要，调试、错误、宕机等
int32_t UartSetLogLevel(LogUart level) {
    gLevel  = level;
    return 0;
}

char *UartRecvParse(Uart *uart, char *recv, int32_t length) {
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
            LogPrintf(LogUart_Info, "uart start send:%s\n", strings);
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
        if (uart->send) {
            LogPrintf(LogUart_Info, "uart end send:%s\n", strings);
            uart->send(gPriv, DataAttr_UartToMqtt, 
                    strings, strlen(strings), DataTimeStatus_BLOCK);
        }
        return strings;
    }

    return NULL;
}

int32_t UartMessageRecvHandler(Uart *uart) {
    int32_t length  = -1;
    int status      = -1;
    ModuleMessage message;
    ModuleMessage *mess = NULL;

    length = uart->bufSize;
    if (uart->recv) {
        length = sizeof(message);
        status = uart->recv(gPriv, 
                DataAttr_MqttToUart, &message, &length, 0);
        if (!status) {
            mess = &message;
            switch (mess->attr) {
                case ModuleDataAttr_GetTemperature:
                    {
                        snprintf (uart->buffer, uart->bufSize, "AT+TEMPERATURE?\r\n");
                        uart->buffer[uart->bufSize - 1] = '\0';
                        LogPrintf(LogUart_Info, "recv AT===>%s\n", uart->buffer);
#ifdef Uart_TEST
                        strcpy(uart->uartAck, "+TEMPERATURE:20\r\n");
                        uart->ackSize = strlen("+TEMPERATURE:20\r\n") + 1;
#endif
                        break;
                    }
                case ModuleDataAttr_GetModuleVersion:
                    {
                        snprintf (uart->buffer, uart->bufSize, "AT+VER?\r\n");
                        uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                        strcpy(uart->uartAck, "+VER:1.1.1\r\n");
                        uart->ackSize = strlen("+VER:1.1.1\r\n") + 1;
#endif
                        break;
                    }
                case ModuleDataAttr_GetModuleInfo:
                    {
                        snprintf (uart->buffer, uart->bufSize, "AT+INFO?\r\n");
                        uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                        strcpy(uart->uartAck, "+INFO:abcd\r\n");
                        uart->ackSize = strlen("+INFO:abcd\r\n") + 1;
#endif
                        break;
                    }
                case ModuleDataAttr_SetModuleInfo:
                    {
                        snprintf (uart->buffer, uart->bufSize, "AT+INFO=%s\r\n", mess->getModuleInfo.info);
                        uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                        strcpy(uart->uartAck, "+OK\r\n");
                        uart->ackSize = strlen("+OK\r\n") + 1;
#endif
                        break;
                    }
                case ModuleDataAttr_GetPower:
                    {
                        snprintf (uart->buffer, uart->bufSize, "AT+POWER?\r\n");
                        uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                        strcpy(uart->uartAck, "+POWER:<DC>,<100>\r\n");
                        uart->ackSize = strlen("+POWER:<DC>,<100>\r\n") + 1;
#endif
                        break;
                    }
                case ModuleDataAttr_Reboot:
                    {
                        snprintf (uart->buffer, uart->bufSize, "AT+OFFNOW?\r\n");
                        uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                        strcpy(uart->uartAck, "+OK\r\n");
                        uart->ackSize = strlen("+OK\r\n") + 1;
#endif
                        break;
                    }
                case ModuleDataAttr_NetState:
                    {
                        snprintf (uart->buffer, uart->bufSize, "AT+NETSTATE=<%s>\r\n", 
                                toNetStateEnumString(mess->netState.state));
                        uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                        strcpy(uart->uartAck, "+OK\r\n");
                        uart->ackSize = strlen("+OK\r\n") + 1;
#endif
                        break;
                    }
                case ModuleDataAttr_GetWifiCfg:
                    {
                        snprintf (uart->buffer, uart->bufSize, "AT+WIFICFG?\r\n"); 
                        uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                        strcpy(uart->uartAck, "+WIFICFG:<TP-LINK_432B>,<88888888>,<192.168.0.102>,<255.255.255.0>,<102.168.0.1>\r\n");
                        uart->ackSize = strlen("+WIFICFG:<TP-LINK_432B>,<88888888>,<192.168.0.102>,<255.255.255.0>,<102.168.0.1>\r\n") + 1;
#endif
                        break;
                    }
                case ModuleDataAttr_SetWifiCfg:
                    {
                        snprintf (uart->buffer, uart->bufSize, 
                                "AT+WIFICFG=<%s>,<%s>,<%s>,<%s>,<%s>\r\n", 
                                mess->setWifiCfg.ssid, 
                                mess->setWifiCfg.password, 
                                mess->setWifiCfg.address, 
                                mess->setWifiCfg.netmask, 
                                mess->setWifiCfg.gateway); 
                        uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                        strcpy(uart->uartAck, "+OK\r\n");
                        uart->ackSize = strlen("+OK\r\n") + 1;
#endif
                        break;
                    }
                case ModuleDataAttr_GetMqttCfg:
                    {
                        snprintf (uart->buffer, uart->bufSize, "AT+MQTTCFG?\r\n"); 
                        uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                        strcpy(uart->uartAck, "+MQTTCFG:<admin>,<123456>,<mqtt://192.168.0.107:1883>,<1883>\r\n");
                        uart->ackSize = strlen("+MQTTCFG:<admin>,<123456>,<mqtt://192.168.0.107:1883>,<1883>\r\n") + 1;
#endif
                        break;
                    }
                case ModuleDataAttr_SetMqttCfg:
                    {
                        snprintf (uart->buffer, uart->bufSize, 
                                "AT+WIFICFG=<%s>,<%s>,<%s>,<%d>\r\n", 
                                mess->setMqttCfg.user, 
                                mess->setMqttCfg.password, 
                                mess->setMqttCfg.url, 
                                mess->setMqttCfg.port); 
                        uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                        strcpy(uart->uartAck, "+OK\r\n");
                        uart->ackSize = strlen("+OK\r\n") + 1;
#endif
                        break;
                    }
                case ModuleDataAttr_GetEthCfg:
                    {
                        snprintf (uart->buffer, uart->bufSize, "AT+ETHCFG?\r\n"); 
                        uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                        strcpy(uart->uartAck, "+ETHCFG:<192.168.0.102>,<255.255.255.0>,<102.168.0.1>\r\n");
                        uart->ackSize = strlen("+ETHCFG:<192.168.0.102>,<255.255.255.0>,<102.168.0.1>\r\n") + 1;
#endif
                        break;
                    }
                case ModuleDataAttr_SetEthCfg:
                    {
                        snprintf (uart->buffer, uart->bufSize, 
                                "AT+ETHCFG=<%s>,<%s>,<%s>\r\n", 
                                mess->setEthCfg.address, 
                                mess->setEthCfg.netmask, 
                                mess->setEthCfg.gateway); 
                        uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                        strcpy(uart->uartAck, "+OK\r\n");
                        uart->ackSize = strlen("+OK\r\n") + 1;
#endif
                        break;
                    }
                case ModuleDataAttr_PtSend:
                    {
                        snprintf (uart->buffer, uart->bufSize, 
                                "AT+PTSEND=<%d>,<%s>,<%s>\r\n", 
                                mess->ptSend.timeOut, 
                                mess->ptSend.mac, 
                                mess->ptSend.data); 
                        uart->buffer[uart->bufSize - 1] = '\0';
                        break;
                    }
                case ModuleDataAttr_Cnt:
                    {
                        snprintf (uart->buffer, uart->bufSize, 
                                "+OK");
                        uart->buffer[uart->bufSize - 1] = '\0';
                        break;
                    }
                default:break;
            }
            /* strcat(uart->buffer, "\r\n"); */
            LogPrintf(LogUart_Info, "Uart AT Send: %s\n", uart->buffer);
#ifndef Uart_TEST
            uart_write_bytes(uart->uartIndex, 
                    (const char *)uart->buffer, strlen(uart->buffer));
#endif
        }
    }

    return 0;
}

void UartSelectTask(void *args) {
    int status      = -1;
    char *sbuffer   = NULL;
    char *bbuffer   = NULL;
    Uart *uart      = (Uart *)args;

    /*指向内存块的中间，方便后面组合数据*/
    sbuffer = (char *)&uart->buffer[uart->bufSize >> 1];

    while (1) {
#ifndef Uart_TEST
        status = uart_read_bytes(uart->uartIndex, 
                sbuffer, ((uart->bufSize >> 1) - 1), 20 / portTICK_PERIOD_MS);
#else
        if (uart->ackSize) {
            strcpy(sbuffer, uart->uartAck);
            status = uart->ackSize;
            uart->ackSize = 0;
        }
        else {
            vTaskDelay(pdMS_TO_TICKS(200));
            status = -1;
        }
#endif
        if (status >= 0) {
            sbuffer[status] = '\0';
            LogPrintf(LogUart_Info, "read AT===:%s\n", sbuffer);
            bbuffer = UartRecvParse(uart, sbuffer, status);
            if (NULL != bbuffer) {
                LogPrintf(LogUart_Info, "Uart AT Recv :%s\n", bbuffer);
            }
            continue;
        }

        UartMessageRecvHandler(uart);
    }
}

void *UartInit(UartConfig *config) {
    /* esp_err_t status                = ESP_FAIL;         */
    BaseType_t baseType             = pdFAIL;
#ifndef Uart_TEST
    uart_config_t uart_config;
#endif

    Uart *uart = (Uart *) malloc (sizeof(*uart));
    ERRP(NULL == uart, return NULL, 1, " uartmalloc Uart Instance failure\n");
    memset(uart, 0x0, sizeof(*uart));

    uart->send      = config->send;
    uart->recv      = config->recv;

    LogPrintf(LogUart_Info, "baud_rate     :%d\n", config->baudRate);
    LogPrintf(LogUart_Info, "data_bits     :%d\n", config->dataBits);
    LogPrintf(LogUart_Info, "parity        :%d\n", config->parity);
    LogPrintf(LogUart_Info, "stop_bits     :%d\n", config->stopBits);
    LogPrintf(LogUart_Info, "flow_ctrl     :%d\n", config->flowCtrl);
    LogPrintf(LogUart_Info, "source_clk    :%d\n", config->sourceClk);

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

#ifndef Uart_TEST
    status = uart_driver_install(config->uartIndex, 2048, 2048, 0, NULL, 0);
    ERRP(ESP_OK != status, goto ERR01, 1, "uart uart_driver_install failure\n");

    //这里需要确定具体的GPIO口
    /* status = uart_set_pin(config->uartIndex, GPI0, GPI0, GPI0, GPIO); */
    /* ERRP(ESP_OK != status, goto ERR02, 1, "uart uart_set_pin failure\n"); */
#endif

#ifndef Uart_TEST
    uart_config.baud_rate   = uart->baudRate;
    uart_config.data_bits   = uart->dataBits;
    uart_config.parity      = uart->parity;
    uart_config.stop_bits   = uart->stopBits;
    uart_config.flow_ctrl   = uart->flowCtrl;
    uart_config.source_clk  = uart->sourceClk;
#endif

    uart->bufSize = 512;
    uart->buffer = (char *) malloc (uart->bufSize);
    ERRP(NULL == uart->buffer, goto ERR02, 1,
            "uart malloc buffer size:%d failure\n", uart->bufSize);

    uart->restore = (char *) malloc (uart->bufSize >> 1);
    ERRP(NULL == uart->restore, goto ERR03, 1,
            "uart malloc restore size:%d failure\n", uart->bufSize >> 1);

#ifndef Uart_TEST
    uart_param_config(config->uartIndex, &uart_config);
#endif

#ifdef Uart_TEST
    uart->uartAck = (char *) malloc (512);
#endif

    baseType = xTaskCreate(UartSelectTask, 
            "uartSelectTask", 4096, uart, 5, &uart->uartTask);
    ERRP(pdPASS != baseType, goto ERR04, 1, "uart xTaskCreate failure\n");

    return uart;
ERR04:
    free(uart->restore);
ERR03:
    free(uart->buffer);
ERR02:
#ifndef Uart_TEST
    uart_driver_delete(config->uartIndex);
ERR01:
#endif
    free(uart->name);
ERR00:
    free(uart);
    return NULL;
}

