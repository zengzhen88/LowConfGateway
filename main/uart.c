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
#include "esp_timer.h"
#include "driver/gpio.h"

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

    
    snprintf (logBuf, sizeof(logBuf), "[%s][%s][%s][%d]", file, func, esp_log_system_timestamp(), line);
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

/* #define Uart_TEST */
#ifdef Uart_TEST
    char *uartAck;
    int ackSize;
#endif

    int32_t signals;
    esp_timer_handle_t timer;

    int mqttStatus;
    int wifiNetStatus;
    int ethNetStatus;

    int getConfDown;

    UartSigSend send;
    UartSigRecv recv;
    UartSigPeek peek;

    int recover[DataAttr_Cnt];
    int mark[DataAttr_Cnt];
    uint32_t timestamp[DataAttr_Cnt];
} Uart;
#define GPIO_TRIGGER        4

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

#define INIT (0)
#define PEEK (1) 
#define RECV (2)
#define TIMEOUT (500)
#define RECOVERNUM (3)
char *UartRecvParse(Uart *uart, char *recv, int32_t length) {
    int32_t msgvalid    = 0;
    char *strings       = recv;
    char *chrr          = NULL;

    if (uart->fillLength > 0) {
        if (strstr(uart->restore, "AT+") 
                || strchr(uart->restore, '+')) {
            //如果没有有效的标志头，数据直接丢弃
            memcpy(strings - uart->fillLength, uart->restore, uart->fillLength);
            length  += uart->fillLength;
            strings -= uart->fillLength;
            strings[length] = '\0';
            uart->fillLength   = 0;
        }
        else {
            uart->fillLength = 0;
        }
    }

    while (1) {
        msgvalid = 0;
        if (length > 0) {
            chrr = strchr(strings, '\r');
            if (chrr) {
                if (chrr[1] == '\n') {
                    msgvalid = 1;
                    chrr[0] = '\0';
                }
                else {
                    memcpy(uart->restore, strings, length);
                    uart->fillLength += length;
                    break;
                }
            }
            else {
                memcpy(uart->restore, strings, length);
                uart->fillLength += length;
                break;
            }

            if (msgvalid) {
                if (uart->send) {
                    LogPrintf(LogUart_Info, "send %s to mqtt\n", strings);
                    if (!uart->getConfDown) {
                        if (NULL == strstr(strings, "UPDATE"))
                            uart->send(gPriv, 
                                    DataAttr_uartToMain, 
                                    strings, strlen(strings), DataTimeStatus_BLOCK);
                    }
                    else {
                        uart->send(gPriv, 
                                DataAttr_UartToMqtt, 
                                strings, strlen(strings), DataTimeStatus_BLOCK);
                    }

                    if (strstr(strings, "+TEMPERATUREUPDATE")
                            || strstr(strings, "+USERINFOUPDATE")
                            || strstr(strings, "+POWERUPDATE")
                            || strstr(strings, "+OFFNOWUPDATE")
                            || strstr(strings, "+ETHCFGUPDATE")
                            || strstr(strings, "+WIFICFGUPDATE")
                            || strstr(strings, "+MQTTCFGUPDATE")
                            || strstr(strings, "+PTUPDATE")
                            || strstr(strings, "+SCANTIMEOUTUPDATE")
                            || strstr(strings, "+REGEXUPDATE")
                            || strstr(strings, "+DEBUGUPDATE")
                            || strstr(strings, "+PTRECV")) {
                        snprintf (uart->buffer, uart->bufSize, 
                                "+OK\r\n");
                        uart->buffer[uart->bufSize - 1] = '\0';
                        int status = uart_write_bytes(uart->uartIndex, 
                                (const char *)uart->buffer, strlen(uart->buffer));
                        if (status < 0) {
                            LogPrintf(LogUart_Error, "uart_write_bytes failure\n");
                        }
                    }
                    else {
                        if (uart->mark[DataAttr_mainToUart] == PEEK) {
                            //表示已经发送了需要ACK的消息
                            if (strstr(strings, "+TEMPERATURE:")
                                    || (strstr(strings, "+DEVINFO:"))
                                    || (strstr(strings, "+USERINFO:"))
                                    || (strstr(strings, "+POWER:")) 
                                    || (strstr(strings, "+ETHCFG:"))
                                    || (strstr(strings, "+WIFICFG:"))
                                    || (strstr(strings, "+MQTTCFG:"))
                                    || (strstr(strings, "+PTSEND:"))
                                    || (strstr(strings, "+SCANTIMEOUT:"))
                                    || (strstr(strings, "+REGEX"))
                                    || (strstr(strings, "+OK"))) {
                                uart->mark[DataAttr_mainToUart] = RECV;
                            }
                        }
                        if (uart->mark[DataAttr_MqttToUart] == PEEK) {
                            //表示已经发送了需要ACK的消息
                            if (strstr(strings, "+TEMPERATURE:")
                                    || (strstr(strings, "+DEVINFO:"))
                                    || (strstr(strings, "+USERINFO:"))
                                    || (strstr(strings, "+POWER:")) 
                                    || (strstr(strings, "+ETHCFG:"))
                                    || (strstr(strings, "+WIFICFG:"))
                                    || (strstr(strings, "+MQTTCFG:"))
                                    || (strstr(strings, "+PTSEND:"))
                                    || (strstr(strings, "+SCANTIMEOUT:"))
                                    || (strstr(strings, "+REGEX"))
                                    || (strstr(strings, "+OK"))) {
                                uart->mark[DataAttr_MqttToUart] = RECV;
                            }
                        }
                        if (uart->mark[DataAttr_EthToUart] == PEEK) {
                            //表示已经发送了需要ACK的消息
                            if (strstr(strings, "+TEMPERATURE:")
                                    || (strstr(strings, "+DEVINFO:"))
                                    || (strstr(strings, "+USERINFO:"))
                                    || (strstr(strings, "+POWER:")) 
                                    || (strstr(strings, "+ETHCFG:"))
                                    || (strstr(strings, "+WIFICFG:"))
                                    || (strstr(strings, "+SCANTIMEOUT:"))
                                    || (strstr(strings, "+MQTTCFG:"))
                                    || (strstr(strings, "+PTSEND:"))
                                    || (strstr(strings, "+REGEX"))
                                    || (strstr(strings, "+OK"))) {
                                uart->mark[DataAttr_EthToUart] = RECV;
                            }
                        }
                        if (uart->mark[DataAttr_WifiToUart] == PEEK) {
                            //表示已经发送了需要ACK的消息
                            if (strstr(strings, "+TEMPERATURE:")
                                    || (strstr(strings, "+DEVINFO:"))
                                    || (strstr(strings, "+USERINFO:"))
                                    || (strstr(strings, "+POWER:")) 
                                    || (strstr(strings, "+ETHCFG:"))
                                    || (strstr(strings, "+SCANTIMEOUT:"))
                                    || (strstr(strings, "+WIFICFG:"))
                                    || (strstr(strings, "+MQTTCFG:"))
                                    || (strstr(strings, "+REGEX"))
                                    || (strstr(strings, "+PTSEND:"))
                                    || (strstr(strings, "+OK"))) {
                                uart->mark[DataAttr_WifiToUart] = RECV;
                            }
                        }
                    }

                    length -= strlen(strings) + 2;
                    strings += strlen(strings) + 2; //指针偏移
                }
            }
        }
        else {
            break;
        }
    }

    return NULL;
}

int32_t UartMessageNetStateX(Uart *uart, ModuleMessage *mess) {
    int change = 0;
    int mqttStatus = uart->mqttStatus;
    int wifiNetStatus = uart->wifiNetStatus;
    int ethNetStatus = uart->ethNetStatus;

    switch (mess->netState._state) {
        case _NetState_MqttConnect:
            {
                mqttStatus = 1;
                LogPrintf(LogUart_Info, "mqtt server connect\n");
                break;
            }
        case _NetState_MqttUnconnect:
            {
                mqttStatus = 0;
                LogPrintf(LogUart_Info, "mqtt server disconnect\n");
                break;
            }
        case _NetState_WifiNetConnect:
            {
                wifiNetStatus = 1;
                LogPrintf(LogUart_Info, "wifi net server connect\n");
                break;
            }
        case _NetState_WifiNetUnconnect:
            {
                wifiNetStatus = 0;
                LogPrintf(LogUart_Info, "wifi net server disconnect\n");
                break;
            }
        case _NetState_EthNetConnect:
            {
                ethNetStatus = 1;
                LogPrintf(LogUart_Info, "eth net server connect\n");
                break;
            }
        case _NetState_EthNetUnconnect:
            {
                ethNetStatus = 0;
                LogPrintf(LogUart_Info, "eth net server disconnect\n");
                break;
            }
        default:
    }

    LogPrintf(LogUart_Info, 
            "mqttStatus:%d uart->mqttStatus:%d "
            "wifinetStatus:%d uart->wifinetStatus:%d"
            "ethnetStatus:%d uart->ethnetStatus:%d\n",
            mqttStatus, uart->mqttStatus, 
            wifiNetStatus, uart->wifiNetStatus,
            ethNetStatus, uart->ethNetStatus);
    if (mqttStatus != uart->mqttStatus 
            || wifiNetStatus != uart->wifiNetStatus
            || ethNetStatus != uart->ethNetStatus) {
        /*发生变化就调整*/
        uart->mqttStatus = mqttStatus;
        uart->wifiNetStatus = wifiNetStatus;
        uart->ethNetStatus = ethNetStatus;
        change = 1;
    }
    if (uart->mqttStatus && (uart->wifiNetStatus || uart->ethNetStatus)) {
        //服务器及网络都有
        mess->netState.state = NetState_CONNSER;
    }
    else if (!uart->mqttStatus && (uart->wifiNetStatus || uart->ethNetStatus)) {
        //服务器无及网络有
        mess->netState.state = NetState_CONNET;
    }
    else if (uart->mqttStatus) {
        //服务器有及网络无 -- 理论不存在,可能存在不同步的问题，这种情况肯定也有网络
        mess->netState.state = NetState_CONNSER;
    }
    else if (!uart->mqttStatus && !uart->wifiNetStatus && !uart->ethNetStatus) {
        //服务器及网络都无 
        mess->netState.state = NetState_UNNET;
    }

    if (change) {
        snprintf (uart->buffer, uart->bufSize - 1, "AT+NETSTATE=%s\r\n", 
                toNetStateEnumString(mess->netState.state));
        uart->buffer[uart->bufSize - 1] = '\0';
        LogPrintf(LogUart_Info, "Uart AT >>> Send: %s\n", uart->buffer);
#ifdef Uart_TEST
        strcpy(uart->uartAck, "+OK\r\n");
        uart->ackSize = strlen("+OK\r\n") + 1;
#endif
        return 0;
    }
    else {
        return -1;
    }
}

int32_t UartMessageRecvHandler(Uart *uart) {
    int32_t length  = -1;
    int status      = -1;
    ModuleMessage message;
    ModuleMessage *mess = NULL;

    /* LogPrintf(LogUart_Info, "DataAttr_MqttToUart recv from\n"); */
    if (uart->recv) {
        /* LogPrintf(LogUart_Info, "DataAttr_MqttToUart recv from xxx\n"); */
        /*这里主要是保证消息不会嵌套*/
        if (uart->mark[DataAttr_MqttToUart] != INIT) goto MQTTTOUART;
        if (uart->mark[DataAttr_EthToUart] != INIT) goto ETHTOUART;
        if (uart->mark[DataAttr_WifiToUart] != INIT) goto WIFITOUART;
        if (uart->mark[DataAttr_mainToUart] != INIT) goto MAINTOUART;

MQTTTOUART:
        length = sizeof(message);
        if (PEEK == uart->mark[DataAttr_MqttToUart]) {
            if (esp_log_early_timestamp() - uart->timestamp[DataAttr_MqttToUart] >= TIMEOUT) {
                //超时，数据重发
                length = uart->bufSize;
                /* status = uart->recv(gPriv,  */
                        /* DataAttr_MqttToUart, &message, &length, 0); */
                //理论一定成功
                uart->mark[DataAttr_MqttToUart] = INIT;
                if (uart->recover[DataAttr_MqttToUart]++ >= RECOVERNUM) {
                    //多次超时，丢数据
                    status = uart->recv(gPriv, 
                            DataAttr_MqttToUart, &message, &length, 0);
                }
            }
        }
        else if (RECV == uart->mark[DataAttr_MqttToUart]) {
            length = uart->bufSize;
            status = uart->recv(gPriv, 
                    DataAttr_MqttToUart, &message, &length, 0);
            //理论一定成功
            uart->mark[DataAttr_MqttToUart] = INIT;
        }
        else if (INIT == uart->mark[DataAttr_MqttToUart]) {
            /* LogPrintf(LogUart_Info, "DataAttr_MqttToUart Init\n"); */
            length = uart->bufSize;
            status = uart->peek(gPriv, 
                    DataAttr_MqttToUart, &message, &length, 0);
            if (!status) {
                uart->mark[DataAttr_MqttToUart] = PEEK;
                uart->timestamp[DataAttr_MqttToUart] = esp_log_early_timestamp();
                uart->recover[DataAttr_MqttToUart] = 0;
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
                            snprintf (uart->buffer, uart->bufSize, "AT+DEVINFO?\r\n");
                            uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                            strcpy(uart->uartAck, "+DEVINFO:1.1.1\r\n");
                            uart->ackSize = strlen("+DEVINFO:1.1.1\r\n") + 1;
#endif
                            break;
                        }
                    case ModuleDataAttr_GetModuleInfo:
                        {
                            snprintf (uart->buffer, uart->bufSize, "AT+USERINFO?\r\n");
                            uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                            strcpy(uart->uartAck, "+USERINFO:abcd\r\n");
                            uart->ackSize = strlen("+USERINFO:abcd\r\n") + 1;
#endif
                            break;
                        }
                    case ModuleDataAttr_SetModuleInfo:
                        {
                            snprintf (uart->buffer, uart->bufSize, "AT+USERINFO=%s\r\n", mess->getModuleInfo.info);
                            uart->buffer[uart->bufSize - 1] = '\0';
                            /* LogPrintf(LogUart_Info, "setModuleInfo:%s\n", uart->buffer); */
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
                            strcpy(uart->uartAck, "+POWER:DC,100\r\n");
                            uart->ackSize = strlen("+POWER:DC,100\r\n") + 1;
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
                            status = UartMessageNetStateX(uart, mess);
                            if (status) {
                                status = uart->recv(gPriv, 
                                        DataAttr_MqttToUart, &message, &length, 0);
                                uart->mark[DataAttr_MqttToUart] = INIT;
                                return -1;
                            }
                            break;
                        }
                    case ModuleDataAttr_GetWifiCfg:
                        {
                            snprintf (uart->buffer, uart->bufSize, "AT+WIFICFG?\r\n"); 
                            uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                            strcpy(uart->uartAck, "+WIFICFG:TP-LINK_342B,88888888,192.168.0.102,255.255.255.0,102.168.0.1\r\n");
                            uart->ackSize = strlen("+WIFICFG:TP-LINK_342B,88888888,192.168.0.102,255.255.255.0,102.168.0.1\r\n") + 1;
#endif
                            break;
                        }
                    case ModuleDataAttr_SetWifiCfg:
                        {
                            snprintf (uart->buffer, uart->bufSize, 
                                    "AT+WIFICFG=%s,%s,%s,%s,%s\r\n", 
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
                            /* printf ("mqtttouart AT+MQTTCFG ============> %s\n", uart->buffer); */
#ifdef Uart_TEST
                            strcpy(uart->uartAck, "+MQTTCFG:admin,123456,mqtt://192.168.0.101:1883\r\n");
                            uart->ackSize = strlen("+MQTTCFG:admin,123456,mqtt://192.168.0.101:1883\r\n") + 1;
#endif
                            break;
                        }
                    case ModuleDataAttr_SetMqttCfg:
                        {
                            snprintf (uart->buffer, uart->bufSize, 
                                    "AT+WIFICFG=%s,%d,%s,%s,%s\r\n", 
                                    mess->setMqttCfg.url, 
                                    mess->setMqttCfg.port, 
                                    mess->setMqttCfg.clientid, 
                                    mess->setMqttCfg.user,
                                    mess->setMqttCfg.password); 
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
                            strcpy(uart->uartAck, "+ETHCFG:192.168.0.102,255.255.255.0,192.168.0.1\r\n");
                            uart->ackSize = strlen("+ETHCFG:192.168.0.102,255.255.255.0,192.168.0.1\r\n") + 1;
#endif
                            break;
                        }
                    case ModuleDataAttr_SetEthCfg:
                        {
                            snprintf (uart->buffer, uart->bufSize, 
                                    "AT+ETHCFG=%s,%s,%s\r\n", 
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
                                    "AT+PTSEND=%s,%d,%s\r\n", 
                                    mess->ptSend.mac, 
                                    (int)mess->ptSend.seq, 
                                    mess->ptSend.data); 
                            uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                            strcpy(uart->uartAck, "+OK\r\n");
                            uart->ackSize = strlen("+OK\r\n") + 1;
#endif
                            break;
                        }
                    case ModuleDataAttr_SetREGEX:
                        {
                            snprintf (uart->buffer, uart->bufSize, 
                                    "AT+REGEX=%s\r\n", 
                                    mess->setREGEX.data); 
                            uart->buffer[uart->bufSize - 1] = '\0';
                            break;
                        }
                    case ModuleDataAttr_GetREGEX:
                        {
                            snprintf (uart->buffer, uart->bufSize, "AT+REGEX?\r\n"); 
                            uart->buffer[uart->bufSize - 1] = '\0';
                            break;
                        }
                    case ModuleDataAttr_GetScanTimeout:
                        {
                            snprintf (uart->buffer, uart->bufSize, "AT+SCANTIMEOUT?\r\n"); 
                            uart->buffer[uart->bufSize - 1] = '\0';
                            break;
                        }
                    case ModuleDataAttr_SetScanTimeout:
                        {
                            snprintf (uart->buffer, uart->bufSize, 
                                    "AT+SCANTIMEOUT=%d\r\n", 
                                    mess->setScanTimeout.sec); 
                            uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                            strcpy(uart->uartAck, "+OK\r\n");
                            uart->ackSize = strlen("+OK\r\n") + 1;
#endif
                            break;
                        }
                    case ModuleDataAttr_ReportDebug:
                        {
                            snprintf (uart->buffer, uart->bufSize, 
                                    "AT+DEBUG=%s\r\n", 
                                    mess->reportDebug.data); 
                            uart->buffer[uart->bufSize - 1] = '\0';
                            break;
                        }
                    case ModuleDataAttr_Cnt:
                        {
                            /*
                             * snprintf (uart->buffer, uart->bufSize, 
                             *         "+OK");
                             * uart->buffer[uart->bufSize - 1] = '\0';
                             */
                            break;
                        }
                    default:break;
                }
                /* strcat(uart->buffer, "\r\n"); */
                LogPrintf(LogUart_Info, "Uart AT mess->attr:%d Send: %s\n", mess->attr, uart->buffer);
#ifndef Uart_TEST
                status = uart_write_bytes(uart->uartIndex, 
                        (const char *)uart->buffer, strlen(uart->buffer));
                if (status < 0) {
                    LogPrintf(LogUart_Error, "uart_write_bytes failure\n");
                }
#endif
            }
        }

ETHTOUART:
        if (PEEK == uart->mark[DataAttr_EthToUart]) {
            if (esp_log_early_timestamp() - uart->timestamp[DataAttr_EthToUart] >= TIMEOUT) {
                //超时，数据重发
                length = uart->bufSize;
                /* status = uart->recv(gPriv,  */
                        /* DataAttr_EthToUart, &message, &length, 0); */
                //理论一定成功
                uart->mark[DataAttr_EthToUart] = INIT;
                if (uart->recover[DataAttr_EthToUart]++ >= RECOVERNUM) {
                    //多次超时，丢数据
                    status = uart->recv(gPriv, 
                            DataAttr_EthToUart, &message, &length, 0);
                }
            }
        }
        else if (RECV == uart->mark[DataAttr_EthToUart]) {
            length = uart->bufSize;
            status = uart->recv(gPriv, 
                    DataAttr_EthToUart, &message, &length, 0);
            //理论一定会成功
            uart->mark[DataAttr_EthToUart] = INIT;
        }
        else if (INIT == uart->mark[DataAttr_EthToUart]) {
            length = uart->bufSize;
            status = uart->peek(gPriv, 
                    DataAttr_EthToUart, &message, &length, 0);
            if (!status) {
                uart->mark[DataAttr_EthToUart] = PEEK;
                uart->timestamp[DataAttr_EthToUart] = esp_log_early_timestamp();
                uart->recover[DataAttr_EthToUart] = 0;
                mess = &message;
                switch (mess->attr) {
                    case ModuleDataAttr_NetState:
                        {
                            status = UartMessageNetStateX(uart, mess);
                            if (status) {
                                uart->mark[DataAttr_EthToUart] = INIT;
                                status = uart->recv(gPriv, 
                                        DataAttr_EthToUart, &message, &length, 0);
                                return -1;
                            }
                            break;
                        }
                    case ModuleDataAttr_ReportDebug:
                        {
                            snprintf (uart->buffer, uart->bufSize, 
                                    "AT+DEBUG=%s\r\n", 
                                    mess->reportDebug.data); 
                            uart->buffer[uart->bufSize - 1] = '\0';
                            break;
                        }
                    default:break;
                }

                LogPrintf(LogUart_Info, "Uart AT Send: %s\n", uart->buffer);
#ifndef Uart_TEST
                status = uart_write_bytes(uart->uartIndex, 
                        (const char *)uart->buffer, strlen(uart->buffer));
                if (status < 0) {
                    LogPrintf(LogUart_Error, "uart_write_bytes failure\n");
                }
#endif
            }
        }

WIFITOUART:
        if (PEEK == uart->mark[DataAttr_WifiToUart]) {
            if (esp_log_early_timestamp() - uart->timestamp[DataAttr_WifiToUart] >= TIMEOUT) {
                //超时，数据重发
                length = uart->bufSize;
                /* status = uart->recv(gPriv,  */
                        /* DataAttr_WifiToUart, &message, &length, 0); */
                //理论一定成功
                uart->mark[DataAttr_WifiToUart] = INIT;
                if (uart->recover[DataAttr_WifiToUart]++ >= RECOVERNUM) {
                    //多次超时，丢数据
                    status = uart->recv(gPriv, 
                            DataAttr_WifiToUart, &message, &length, 0);
                }
            }
        }
        else if (RECV == uart->mark[DataAttr_WifiToUart]) {
            length = uart->bufSize;
            status = uart->recv(gPriv, 
                    DataAttr_WifiToUart, &message, &length, 0);
            //理论一定会成功
            uart->mark[DataAttr_WifiToUart] = INIT;
        }
        else if (INIT == uart->mark[DataAttr_WifiToUart]) {
            length = uart->bufSize;
            status = uart->peek(gPriv, 
                    DataAttr_WifiToUart, &message, &length, 0);
            if (!status) {
                uart->mark[DataAttr_WifiToUart] = PEEK;
                uart->timestamp[DataAttr_WifiToUart] = esp_log_early_timestamp();
                uart->recover[DataAttr_WifiToUart] = 0;
                mess = &message;
                switch (mess->attr) {
                    case ModuleDataAttr_NetState:
                        {
                            status = UartMessageNetStateX(uart, mess);
                            if (status) {
                                uart->mark[DataAttr_WifiToUart] = INIT;
                                status = uart->recv(gPriv, 
                                        DataAttr_WifiToUart, &message, &length, 0);
                                return -1;
                            }
                /* LogPrintf(LogUart_Info, "Uart AT >>> Send: %s\n", uart->buffer); */
                            break;
                        }
                    case ModuleDataAttr_SetWifiCfg:
                        {
                            snprintf (uart->buffer, uart->bufSize, 
                                    "AT+WIFICFG=%s,%s,%s,%s,%s\r\n", 
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
                    case ModuleDataAttr_ReportDebug:
                        {
                            snprintf (uart->buffer, uart->bufSize, 
                                    "AT+DEBUG=%s\r\n", 
                                    mess->reportDebug.data); 
                            uart->buffer[uart->bufSize - 1] = '\0';
                            break;
                        }
                    case ModuleDataAttr_Cnt:
                        {
                            /*
                             * snprintf (uart->buffer, uart->bufSize, 
                             *         "+OK");
                             * uart->buffer[uart->bufSize - 1] = '\0';
                             */
                            break;
                        }
                    default:break;
                }
                /* strcat(uart->buffer, "\r\n"); */
                LogPrintf(LogUart_Info, "Uart AT Send: %s\n", uart->buffer);
#ifndef Uart_TEST
                status = uart_write_bytes(uart->uartIndex, 
                        (const char *)uart->buffer, strlen(uart->buffer));
                if (status < 0) {
                    LogPrintf(LogUart_Error, "uart_write_bytes failure\n");
                }
#endif
            }
        }

MAINTOUART:
        if (PEEK == uart->mark[DataAttr_mainToUart]) {
            if (esp_log_early_timestamp() - uart->timestamp[DataAttr_mainToUart] >= TIMEOUT) {
                //超时，数据重发
                length = uart->bufSize;
                /* status = uart->recv(gPriv,  */
                        /* DataAttr_mainToUart, &message, &length, 0); */
                //理论一定成功
                uart->mark[DataAttr_mainToUart] = INIT;
                if (uart->recover[DataAttr_mainToUart]++ >= RECOVERNUM) {
                    //多次超时，丢数据
                    status = uart->recv(gPriv, 
                            DataAttr_mainToUart, &message, &length, 0);
                }
            }
        }
        else if (RECV == uart->mark[DataAttr_mainToUart]) {
            length = uart->bufSize;
            status = uart->recv(gPriv, 
                    DataAttr_mainToUart, &message, &length, 0);
            //理论一定会成功
            uart->mark[DataAttr_mainToUart] = INIT;
        }
        else if (INIT == uart->mark[DataAttr_mainToUart]) {
            length = uart->bufSize;
            status = uart->peek(gPriv, 
                    DataAttr_mainToUart, &message, &length, 0);
            if (!status) {
                uart->mark[DataAttr_mainToUart] = PEEK;
                uart->timestamp[DataAttr_mainToUart] = esp_log_early_timestamp();
                uart->recover[DataAttr_mainToUart] = 0;
                mess = &message;
                switch (mess->attr) {
                    case ModuleDataAttr_GetTemperature:
                        {
                            snprintf (uart->buffer, uart->bufSize, "AT+TEMPERATURE?\r\n");
                            uart->buffer[uart->bufSize - 1] = '\0';
                            /* LogPrintf(LogUart_Info, "recv AT===>%s\n", uart->buffer); */
#ifdef Uart_TEST
                            strcpy(uart->uartAck, "+TEMPERATURE:20\r\n");
                            uart->ackSize = strlen("+TEMPERATURE:20\r\n") + 1;
#endif
                            break;
                        }
                    case ModuleDataAttr_GetModuleVersion:
                        {
                            snprintf (uart->buffer, uart->bufSize, "AT+DEVINFO?\r\n");
                            uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                            strcpy(uart->uartAck, "+DEVINFO:1.1.1\r\n");
                            uart->ackSize = strlen("+DEVINFO:1.1.1\r\n") + 1;
#endif
                            break;
                        }
                    case ModuleDataAttr_GetModuleInfo:
                        {
                            snprintf (uart->buffer, uart->bufSize, "AT+USERINFO?\r\n");
                            uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                            strcpy(uart->uartAck, "+USERINFO:abcd\r\n");
                            uart->ackSize = strlen("+USERINFO:abcd\r\n") + 1;
#endif
                            break;
                        }
                    case ModuleDataAttr_GetPower:
                        {
                            snprintf (uart->buffer, uart->bufSize, "AT+POWER?\r\n");
                            uart->buffer[uart->bufSize - 1] = '\0';
#ifdef Uart_TEST
                            strcpy(uart->uartAck, "+POWER:DC,100\r\n");
                            uart->ackSize = strlen("+POWER:DC,100\r\n") + 1;
#endif
                            break;
                        }
                    case ModuleDataAttr_GetWifiCfg:
                        {
                            snprintf (uart->buffer, uart->bufSize, "AT+WIFICFG?\r\n"); 
                            uart->buffer[uart->bufSize - 1] = '\0';
                            /* printf ("AT+WIFICFG ============> %s\n", uart->buffer); */
#ifdef Uart_TEST
                            strcpy(uart->uartAck, "+WIFICFG:TP-LINK_342B,88888888,192.168.0.102,255.255.255.0,102.168.0.1\r\n");
                            uart->ackSize = strlen("+WIFICFG:TP-LINK_342B,88888888,192.168.0.102,255.255.255.0,102.168.0.1\r\n") + 1;
#endif
                            break;
                        }
                    case ModuleDataAttr_GetMqttCfg:
                        {
                            snprintf (uart->buffer, uart->bufSize, "AT+MQTTCFG?\r\n"); 
                            uart->buffer[uart->bufSize - 1] = '\0';
                            /* printf ("maintouart AT+MQTTCFG ============> %s\n", uart->buffer); */
#ifdef Uart_TEST
                            strcpy(uart->uartAck, "+MQTTCFG:admin,123456,mqtt://192.168.0.101:1883\r\n");
                            uart->ackSize = strlen("+MQTTCFG:admin,123456,mqtt://192.168.0.101:1883\r\n") + 1;
#endif
                            break;
                        }
                    case ModuleDataAttr_GetEthCfg:
                        {
                            snprintf (uart->buffer, uart->bufSize, "AT+ETHCFG?\r\n"); 
                            uart->buffer[uart->bufSize - 1] = '\0';
                            /* printf ("AT+ETHCFG ============> %s\n", uart->buffer); */
#ifdef Uart_TEST
                            strcpy(uart->uartAck, "+ETHCFG:192.168.0.102,255.255.255.0,192.168.0.1\r\n");
                            uart->ackSize = strlen("+ETHCFG:192.168.0.102,255.255.255.0,192.168.0.1\r\n") + 1;
#endif
                            break;
                        }
                    case ModuleDataAttr_GetScanTimeout:
                        {
                            snprintf (uart->buffer, uart->bufSize, "AT+SCANTIMEOUT?\r\n"); 
                            uart->buffer[uart->bufSize - 1] = '\0';
                            break;
                        }
                    case ModuleDataAttr_GetREGEX:
                        {
                            snprintf (uart->buffer, uart->bufSize, "AT+REGEX?\r\n"); 
                            uart->buffer[uart->bufSize - 1] = '\0';
                            break;
                        }
                    case ModuleDataAttr_GetConfDown:
                        {
                            uart->getConfDown = 1;
                            return 0;
                        }
                    default:break;
                }
                /* strcat(uart->buffer, "\r\n"); */
                LogPrintf(LogUart_Info, "Uart AT Send: %s\n", uart->buffer);
#ifndef Uart_TEST
                status = uart_write_bytes(uart->uartIndex, 
                        (const char *)uart->buffer, strlen(uart->buffer));
                if (status < 0) {
                    LogPrintf(LogUart_Error, "uart_write_bytes failure\n");
                }
#endif
            }
        }
    }

    return 0;
}

#ifdef Uart_TEST
int32_t UartMaunulSendAT(void *arg, ModuleDataAttr attr) {
    Uart *uart      = (Uart *)arg;

    switch (attr) {
        case ModuleDataAttr_GetTemperature:
            {
                strcpy(uart->uartAck, "+TEMPERATUREUPDATE:20\r\n");
                uart->ackSize = strlen("+TEMPERATUREUPDATE:20\r\n") + 1;
                break;
            }
        case ModuleDataAttr_GetModuleInfo:
            {
                strcpy(uart->uartAck, "+USERINFOUPDATE:abcd\r\n");
                uart->ackSize = strlen("+USERINFOUPDATE:abcd\r\n") + 1;
                break;
            }
        case ModuleDataAttr_GetPower:
            {
                strcpy(uart->uartAck, "+POWERUPDATE:DC,100\r\n");
                uart->ackSize = strlen("+POWERUPDATE:DC,100\r\n") + 1;
                break;
            }
        case ModuleDataAttr_Reboot:
            {
                strcpy(uart->uartAck, "+OFFNOWUPDATE\r\n");
                uart->ackSize = strlen("+OFFNOWUPDATE\r\n") + 1;
                break;
            }
        case ModuleDataAttr_GetEthCfg:
            {
                strcpy(uart->uartAck, "+ETHCFGUPDATE:192.168.0.104,255.255.255.0,192.168.0.1\r\n");
                uart->ackSize = strlen("+ETHCFGUPDATE:192.168.0.104,255.255.255.0,192.168.0.1\r\n") + 1;
                break;
            }
        case ModuleDataAttr_GetWifiCfg:
            {
                strcpy(uart->uartAck, "+WIFICFGUPDATE:TP-LINK_342B,88888888,192.168.0.103,255.255.255.0,192.168.0.1\r\n");
                uart->ackSize = strlen("+WIFICFGUPDATE:TP-LINK_342B,88888888,192.168.0.103,255.255.255.0,192.168.0.1\r\n") + 1;
                break;
            }
        case ModuleDataAttr_GetMqttCfg:
            {
                strcpy(uart->uartAck, "+MQTTCFGUPDATE:admin,123456,mqtt://192.168.0.101:1883\r\n");
                uart->ackSize = strlen("+MQTTCFGUPDATE:admin,123456,mqtt://192.168.0.101:1883\r\n") + 1;
                break;
            }
        default:break;
    }

    return 0;
}
#endif

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
                sbuffer, ((uart->bufSize >> 1) - 1), pdMS_TO_TICKS(20));
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
        if (status > 0) {
            sbuffer[status] = '\0';
            LogPrintf(LogUart_Info, "read(%d) cpuId(%d)AT===:%s\n", status, xPortGetCoreID(), sbuffer);
            /* bbuffer =  */UartRecvParse(uart, sbuffer, status);
            /* if (NULL != bbuffer) { */
                /* LogPrintf(LogUart_Info, "Uart AT Recv :%s\n", bbuffer); */
            /* } */
            continue;
        }

        UartMessageRecvHandler(uart);
    }
}

#include <esp_rom_sys.h>
static void timer_cb(void *arg) {
    Uart *uart = (Uart *)arg;

    gpio_set_level(GPIO_TRIGGER, uart->signals);
    esp_rom_delay_us(10);
    /* LogPrintf(LogSpi_Info, "signal:%d\n", gpio_get_level(GPIO_TRIGGER)); */
    uart->signals = !uart->signals;
}

void *UartInit(UartConfig *config) {
    esp_err_t status                = ESP_FAIL;        
    BaseType_t baseType             = pdFAIL;
#ifndef Uart_TEST
    uart_config_t uart_config;
    memset(&uart_config, 0x0, sizeof(uart_config));
#endif

    Uart *uart = (Uart *) malloc (sizeof(*uart));
    ERRP(NULL == uart, return NULL, 1, " uartmalloc Uart Instance failure\n");
    memset(uart, 0x0, sizeof(*uart));

    uart->send      = config->send;
    uart->recv      = config->recv;
    uart->peek      = config->peek;

    LogPrintf(LogUart_Info, "baud_rate     :%d\n", config->baudRate);
    LogPrintf(LogUart_Info, "data_bits     :%d\n", config->dataBits);
    LogPrintf(LogUart_Info, "parity        :%d\n", config->parity);
    LogPrintf(LogUart_Info, "stop_bits     :%d\n", config->stopBits);
    LogPrintf(LogUart_Info, "flow_ctrl     :%d\n", config->flowCtrl);
    LogPrintf(LogUart_Info, "source_clk    :%d\n", config->sourceClk);

    uart->mqttStatus= 0;
    uart->wifiNetStatus = 0;
    uart->ethNetStatus = 0;
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

    uart_config.baud_rate   = uart->baudRate;
    uart_config.data_bits   = uart->dataBits;
    uart_config.parity      = uart->parity;
    uart_config.stop_bits   = uart->stopBits;
    uart_config.flow_ctrl   = uart->flowCtrl;
    uart_config.source_clk  = uart->sourceClk;

    uart_param_config(config->uartIndex, &uart_config);

    //这里需要确定具体的GPIO口
    status = uart_set_pin(config->uartIndex, 10, 9, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    ERRP(ESP_OK != status, goto ERR02, 1, "uart uart_set_pin failure\n");
#endif

    uart->bufSize = 512;
    uart->buffer = (char *) malloc (uart->bufSize);
    ERRP(NULL == uart->buffer, goto ERR02, 1,
            "uart malloc buffer size:%d failure\n", uart->bufSize);

    uart->restore = (char *) malloc (uart->bufSize >> 1);
    ERRP(NULL == uart->restore, goto ERR03, 1,
            "uart malloc restore size:%d failure\n", uart->bufSize >> 1);

#ifdef Uart_TEST
    uart->uartAck = (char *) malloc (512);
#endif

    baseType = xTaskCreate(UartSelectTask, 
            "uartSelectTask", 8192, uart, 5, &uart->uartTask);
    ERRP(pdPASS != baseType, goto ERR04, 1, "uart xTaskCreate failure\n");

    gpio_config_t gpio_cfg = {
        .pin_bit_mask = (1ULL << GPIO_TRIGGER),
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
    };
    gpio_config(&gpio_cfg);

    const esp_timer_create_args_t timer_args = {
        timer_cb,
        uart,
        ESP_TIMER_TASK,
        "uart_timer",
        true,
    };
    esp_timer_create(&timer_args, &uart->timer);
    esp_timer_start_periodic(uart->timer, 1000000);//10ms

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

