/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include <mqtt.h>
#include <wifi.h>
#include <uart.h>
#include "esp_log.h"
#include <eth.h>
#include <message.h>
#include <update.h>
#include <spi.h>
#include <queue.h>
#include <uart.h>
#include "driver/uart.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <esp_mac.h>
#include "re.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t cpp_regexFunction(const char *inStr, const char *matchStr);

#ifdef __cplusplus
}
#endif

typedef struct {
    void *wifi;
    void *eth;
    void *mqtt;
    void *update;
    void *uart;
    void *spi;

    float temperature;

    /* char version[32]; */
    char module[32];
    char hardwareVer[32];
    char firmwareVer[32];
    char mac[32];


    char info[32];

    PowerSupplyMode mode;
    int32_t level;

    /*eth*/
    char ethAddress[32];
    char ethNetmask[32];
    char ethGateway[32];

    /*wifi*/
    char wifiSsid[32];
    char wifiPassword[32];
    char wifiAddress[32];
    char wifiNetmask[32];
    char wifiGateway[32];

    /*mqtt*/
    char user[32];
    char password[32];
    char url[64];
    char clientid[64];
    short port;

    char regex[64];

/* #define USE_RING_BUFFER */

#ifndef USE_RING_BUFFER
    QueueHandle_t bufQue[DataAttr_Cnt];
#else 
    Queue bufQue[DataAttr_Cnt];
#endif
} Gateway;

#define IPVALID "^(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)" \
            "(?:\\." \
            "(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)" \
            "){3}$"
#define MASKVALID   "^((255.255.255.(0|128|192|224|240|248|252|254|255))|(255.255.(0|128|192|224|240|248|252|254|255).0)|(255.(0|128|192|224|240|248|252|254|255).0.0)|((0|128|192|224|240|248|252|254|255).0.0.0))$"

int32_t appPrint(void *priv, const char *strings) {
    return printf ("%s", strings);
}
static int32_t running = 1;
static int32_t appTriggerRecv(void *handle, DataAttr attr) {
    Gateway *gateWay = (Gateway *)handle;

    if (!running) {
        return 0;
    }

    switch (attr) {
        case DataAttr_MqttToWifi:
            {
                return WifiTriggerRecv(gateWay->wifi);
            }
        case DataAttr_UartToMqtt:
            {
                return MQTTUartTriggerRecv(gateWay->mqtt);
            }
        case DataAttr_EthToMqtt:
            {
                return MQTTEthTriggerRecv(gateWay->mqtt);
            }
        case DataAttr_EthToWifi:
            {
                return WifiTriggerRecv(gateWay->wifi);
            }
        case DataAttr_WifiToUart:
            {
                return 0;//UartTriggerRecv(gateWay->mqtt);
            }
        case DataAttr_WifiToMqtt:
            {
                return MQTTWifiTriggerRecv(gateWay->mqtt);
            }
        case DataAttr_MqttToEth:
            {
                return EthTriggerRecv(gateWay->eth);
            }
        case DataAttr_MqttToUpdate:
            {
                return UpdateTriggerRecv(gateWay->update);
            }
        case DataAttr_MqttToSpi:
            {
                return SpiTriggerRecv(gateWay->spi);
            }
        case DataAttr_SpiToMqtt:
            {
                return MQTTSpiTriggerRecv(gateWay->mqtt);
            }
        default:break;
    }

    return 0;
}

static int sendNum[DataAttr_Cnt] = {
    0
};
static int recvNum[DataAttr_Cnt] = {
    0
};

int32_t appMsgSend(void *priv, DataAttr attr, 
        void *data, int32_t fillLength, int32_t millis) {
    int status       = ESP_FAIL;
    Message *message = NULL;
    TickType_t block = portMAX_DELAY; 
    Gateway *gateway = (Gateway *)priv;

    if (!running) {
        return -1;
    }

    switch (millis) {
        case DataTimeStatus_BLOCK:
            {
                block = portMAX_DELAY;
                break;
            }
        case DataTimeStatus_UNBLOCK:
        default:
            {
                block = pdMS_TO_TICKS(millis);
                break;
            }
    }

    message = RequestMessage(fillLength);
    if (message) {
        message->attr       = attr;
        message->length     = fillLength;
        memcpy(message->data, data, message->length);
    
        status = xQueueSend(gateway->bufQue[attr], &message, (block));
        if (pdPASS == status) {
            sendNum[attr]++;
            appTriggerRecv(gateway, attr);
            return 0;
        }

        ReleaseMessage(message);
    }


    return -1;
}

int32_t appMsgRecv(void *priv, DataAttr attr, 
        void *data, int32_t *fillLength, int32_t millis) {
    int32_t status   = -1;
    Message *message = NULL;
    TickType_t block = portMAX_DELAY; 
    Gateway *gateway = (Gateway *)priv;

    if (!running) {
        return -1;
    }

    switch (millis) {
        case DataTimeStatus_BLOCK:
            {
                block = portMAX_DELAY;
                break;
            }
        case DataTimeStatus_UNBLOCK:
        default:
            {
                block = pdMS_TO_TICKS(millis);
                break;
            }
    }

    status = xQueueReceive(gateway->bufQue[attr], &message, (block));
    if (pdTRUE == status) {
        if (*fillLength >= message->length) {
            /* 内存足够才送数据 */
            memcpy(data, message->data, message->length);
            *fillLength = message->length;
        }

        recvNum[attr]++;
        ReleaseMessage(message);

        return 0;
    }

    return -1;
}

int32_t appDataRequest1(void *priv, DataAttr attr, void *data, void **subData, int32_t fillLength) {
    Message *message = NULL;

    if (!running) {
        return -1;
    }

    message = RequestMessage1(subData, fillLength);
    if (message) {
        message->attr       = attr;
        message->length     = fillLength;
        sendNum[attr]++;

        *((Message **)data) = message;

        return 0;
    }


    return -1;
}

int32_t appDataRequest(void *priv, DataAttr attr, void *data, int32_t fillLength) {
    Message *message = NULL;

    if (!running) {
        return -1;
    }

    message = RequestMessage(fillLength);
    if (message) {
        message->attr       = attr;
        message->length     = fillLength;
        sendNum[attr]++;

        *((Message **)data) = message;

        return 0;
    }


    return -1;
}

int32_t appDataRelease(void *priv, DataAttr attr, void *data) {
    if (!running) {
        return -1;
    }

    recvNum[attr]++;

    ReleaseMessage((Message *)data);
    return 0;
}

int32_t appDataSend(void *priv, DataAttr attr, void *data, int32_t millis) {
    int status       = ESP_FAIL;
    Message *message = (Message *)data;
    TickType_t block = portMAX_DELAY; 
    Gateway *gateway = (Gateway *)priv;

    if (!running) {
        return -1;
    }

    switch (millis) {
        case DataTimeStatus_BLOCK:
            {
                block = portMAX_DELAY;
                break;
            }
        case DataTimeStatus_UNBLOCK:
        default:
            {
                block = pdMS_TO_TICKS(millis);
                break;
            }
    }

    status = xQueueSend(gateway->bufQue[attr], &message, (block));
    if (pdPASS == status) {
        appTriggerRecv(gateway, attr);
        return 0;
    }

    return -1;
}

int32_t appDataRecv(void *priv, DataAttr attr, void *data, int32_t millis) {
    BaseType_t status   = -1;
    Message *message = NULL;
    TickType_t block = portMAX_DELAY; 
    Gateway *gateway = (Gateway *)priv;

    if (!running) {
        return -1;
    }

    switch (millis) {
        case DataTimeStatus_BLOCK:
            {
                block = portMAX_DELAY;
                break;
            }
        case DataTimeStatus_UNBLOCK:
        default:
            {
                block = pdMS_TO_TICKS(millis);
                break;
            }
    }

    status = xQueueReceive(gateway->bufQue[attr], &message, (block));
    if (pdTRUE == status) {
        *((Message **)data) = message;
        return 0;
    }

    return -1;
}

int32_t appSend(void *priv, DataAttr attr, void *data, 
        int32_t fillLength, int32_t millis) { 
    if (!running) {
        return -1;
    }

    switch (attr) {
        case DataAttr_SpiToMqtt:
            {
                return appDataSend(priv, attr, data, millis);
            }
        default:
            return appMsgSend(priv, attr, data, fillLength, millis);
    }

    return 0;
}

int32_t appRecv(void *priv, DataAttr attr, void *data, 
        int32_t *fillLength, int32_t millis) { 
    if (!running) {
        return -1;
    }

    switch (attr) {
        case DataAttr_SpiToMqtt:
            {
                return appDataRecv(priv, attr, data, millis);
            }
        default:
            return appMsgRecv(priv, attr, data, fillLength, millis);
    }

    return 0;
}

int32_t appPeek(void *priv, DataAttr attr, 
        void *data, int32_t *fillLength, int32_t millis) {
    int32_t status   = -1;
    Message *message = NULL;
    TickType_t block = portMAX_DELAY; 
    Gateway *gateway = (Gateway *)priv;

    if (!running) {
        return -1;
    }

    switch (millis) {
        case DataTimeStatus_BLOCK:
            {
                block = portMAX_DELAY;
                break;
            }
        case DataTimeStatus_UNBLOCK:
        default:
            {
                block = pdMS_TO_TICKS(millis);
                break;
            }
    }

    status = xQueuePeek(gateway->bufQue[attr], &message, (block));
    if (pdTRUE == status) {
        if (*fillLength >= message->length) {
            /* 内存足够才送数据 */
            memcpy(data, message->data, message->length);
            *fillLength = message->length;
        }

        /* recvNum[attr]++; */
        /* ReleaseMessage(message); */

        return 0;
    }

    return -1;
}


int32_t appSearchConfig(void *args, ModuleMessage *message) {
    int32_t status  = -1;
    char recv[128];
    Gateway *gateway= (Gateway *) args;

    status = appSend(gateway, DataAttr_mainToUart,
            message, sizeof(*message), 0);
    if (!status) {
        int32_t length = sizeof(recv);
        status = appRecv(gateway, DataAttr_uartToMain, 
                recv, &length, 2000);
        if (!status) {
                    /* printf (">>>>>>>>>>>>> :%s\n", recv); */
            /*AT命令检测*/
            /* +TEMPERATURE:20 */
            recv[length] = '\0';
            char *valid = strchr(recv, ':');
            if (valid) {
                valid++;
                recv[length] = '\0';
                if (strstr(recv, "+TEMPERATURE:")) {
                    float temperature = atof(valid);/*获取温度*/
                    gateway->temperature = temperature;
                    printf ("===> temperature:%lf\n", temperature);
                }
                else if (strstr(recv, "+DEVINFO:")) {
                    char *version = valid;
                    char *sptr = version;//strchr(info, '<');
                    if (sptr) {
                        //sptr = module,hardwareVer,firmwareVer,mac
                        char *ptr = strchr(sptr, ',');
                        if (ptr) {
                            //ptr = ,hardwareVer,firmwareVer,mac
                            *ptr = '\0';
                            strcpy(gateway->module, sptr);
                            sptr = ptr + 1;
                            //sptr = hardwareVer,firmwareVer,mac
                            if (sptr) {
                                ptr = strchr(sptr, ',');
                                if (ptr) {
                                    *ptr = '\0';
                                    //ptr = ,firmwareVer,mac
                                    strcpy(gateway->hardwareVer, sptr);
                                    sptr = ptr + 1;
                                    //sptr firmwareVer,mac
                                    if (sptr) {
                                        ptr = strchr(sptr, ',');
                                        if (ptr) {
                                            *ptr = '\0';
                                            //ptr = ,mac
                                            strcpy(gateway->firmwareVer, sptr);
                                            sptr = ptr + 1;
                                            if (sptr) {
                                                strcpy(gateway->mac, sptr);
                                            }
                                        }
                                        printf ("===> moduleInfo>module:%s\n", gateway->module);
                                        printf ("===> moduleInfo>hardwareVer:%s\n", gateway->hardwareVer);
                                        printf ("===> moduleInfo>firmwareVer:%s\n", gateway->firmwareVer);
                                        printf ("===> moduleInfo>mac:%s\n", gateway->mac);
                                    }
                                }
                            }
                        }
                    }
                }
                else if (strstr(recv, "+USERINFO:")) {
                    char *info = valid;
                    strcpy(gateway->info, info);
                    printf ("===> moduleInfo:%s\n", info);
                }
                /* +POWER:DC,50 */
                else if (strstr(recv, "+POWER:")) {
                    int32_t power           = 0;;
                    PowerSupplyMode mode    = PowerCnt;
                    char *info = valid;
                    char *ptr = strchr(info, ',');
                    if (ptr) {
                        //,50
                        *ptr = '\0';
                        if (!strcmp(info, "DC")) {
                            mode = DC;
                        }
                        else if (!strcmp(info, "BAT")) {
                            mode = BAT;
                        }
                        else {
                            mode = PowerCnt;
                        }

                        power = atoi(ptr + 1);
                        gateway->mode = mode;
                        gateway->level= power;
                        printf ("===> Power Mode:%s level:%ld\n", 
                                mode == DC ? "DC" : "BAT", 
                                power);
                    }
                }
                else if (strstr(recv, "+ETHCFG:")) {
                    char *address           = NULL;
                    char *netmask           = NULL;
                    char *gateways          = NULL;

                    char *info = valid;
                    printf ("info:%s\n", info);
                    //address,netmask,gateway
                    char *sptr = info;//strchr(info, '<');
                    if (sptr) {
                        char *ptr = strchr(sptr, ',');
                        if (ptr) {
                            //ptr = ,netmask,gateway
                            *ptr = '\0';
                            address = sptr;
                            sptr = ptr + 1;
                            //sptr = netmask,gateway
                            if (sptr) {
                                ptr = strchr(sptr, ',');
                                if (ptr) {
                                    //ptr = ,gateway
                                    *ptr = '\0';
                                    netmask = sptr;
                                    sptr = ptr + 1;
                                    //sptr = gateway
                                    if (sptr) {
                                        gateways = sptr;
                                        printf ("===> EthConfig "
                                                "address:%s \n", 
                                                address); 
                                        printf ("===> EthConfig "
                                                "netmask:%s\n", 
                                                netmask); 
                                        printf ("===> EthConfig "
                                                "gateways:%s\n", 
                                                gateways); 
                                        status = is_ipv4_addr(address);
                                        /* status = cpp_regexFunction(address, IPVALID); */
                                        if (status) {
                                            printf ("ETHCFG address:%s invalid\n", address);
                                            return -1;
                                        }
                                        status = is_ipv4_addr(netmask);
                                        /* status = cpp_regexFunction(netmask, MASKVALID); */
                                        if (status) {
                                            printf ("ETHCFG netmask:%s invalid\n", netmask);
                                            return -1;
                                        }
                                        status = is_ipv4_addr(gateways);
                                        /* status = cpp_regexFunction(gateways, IPVALID); */
                                        if (status) {
                                            printf ("ETHCFG gateways:%s invalid\n", gateways);
                                            return -1;
                                        }
                                        strcpy(gateway->ethAddress, address);
                                        strcpy(gateway->ethNetmask, netmask);
                                        strcpy(gateway->ethGateway, gateways);
                                    }
                                }
                            }
                        }
                    }
                }
                else if (strstr(recv, "+WIFICFG:")) {
                    char *ssid              = NULL;
                    char *password          = NULL;
                    char *address           = NULL;
                    char *netmask           = NULL;
                    char *gateways          = NULL;

                    char *info = valid;
                    printf ("info:%s\n", info);
                    //ssid,password,address,netmask,gateway
                    char *sptr = info;//strchr(info, '<');
                    if (sptr) {
                        char *ptr = strchr(sptr, ',');
                        if (ptr) {
                            //ptr = ,password,address,netmask,gateway
                            *ptr = '\0';
                            ssid = sptr;
                            sptr = ptr + 1;
                            //sptr = password,address,netmask,gateway
                            if (sptr) {
                                ptr = strchr(sptr, ',');
                                if (ptr) {
                                    //ptr = ,address,netmask,gateway
                                    *ptr = '\0';
                                    password = sptr;
                                    sptr = ptr + 1;
                                    //sptr = address,netmask,gateway
                                    if (sptr) {
                                        ptr = strchr(sptr, ',');
                                        if (ptr) {
                                            //ptr = ,netmask,gateway
                                            *ptr = '\0';
                                            address = sptr;
                                            sptr = ptr + 1;
                                            //netmask,gateway 
                                            if (sptr) {
                                                ptr = strchr(sptr, ',');
                                                if (ptr) {
                                                    //,gateway
                                                    *ptr = '\0';
                                                    netmask = sptr;
                                                    sptr = ptr + 1;
                                                    //gateway
                                                    if (sptr) {
                                                        gateways = sptr;
                                                        printf ("===> WifiConfig ssid:%s\n", 
                                                                ssid); 
                                                        printf ("===> WifiConfig password:%s\n", 
                                                                password); 
                                                        printf ("===> WifiConfig address:%s \n", 
                                                                address); 
                                                        printf ("===> WifiConfig netmask:%s\n", 
                                                                netmask); 
                                                        printf ("===> WifiConfig gateways:%s\n", 
                                                                 gateways); 
                                                        status = is_ipv4_addr(address);
                                                        /* status = cpp_regexFunction(address, IPVALID); */
                                                        if (status) {
                                                            printf ("WIFICFG address:%s invalid\n", address);
                                                            return -1;
                                                        }
                                                        /* status = cpp_regexFunction(netmask, MASKVALID); */
                                                        status = is_ipv4_addr(netmask);
                                                        if (status) {
                                                            printf ("WIFICFG netmask:%s invalid\n", netmask);
                                                            return -1;
                                                        }
                                                        /* status = cpp_regexFunction(gateways, IPVALID); */
                                                        status = is_ipv4_addr(gateways);
                                                        if (status) {
                                                            printf ("WIFICFG gateways:%s invalid\n", gateways);
                                                            return -1;
                                                        }
                                                        strcpy(gateway->wifiSsid, ssid);
                                                        strcpy(gateway->wifiPassword, password);
                                                        strcpy(gateway->wifiAddress, address);
                                                        strcpy(gateway->wifiNetmask, netmask);
                                                        strcpy(gateway->wifiGateway, gateways);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (strstr(recv, "+MQTTCFG:")) {
                    char *user              = NULL;
                    char *password          = NULL;
                    char *url               = NULL;
                    char *clientid          = NULL;
                    short port              = 0;

                    char *info = valid;
                    printf ("info:%s\n", info);
                    //url,port,clientid,user,password
                    char *sptr = info;//strchr(info, '<');
                    if (sptr) {
                        char *ptr = strchr(sptr, ',');
                        if (ptr) {
                            //,port,clientid,user,password
                            *ptr = '\0';
                            url = sptr;
                            sptr = ptr + 1;
                            //port,clientid,user,password
                            if (sptr) {
                                ptr = strchr(sptr, ',');
                                if (ptr) {
                                    //,clientid,user,password
                                    *ptr = '\0';
                                    port = atoi(sptr);
                                    sptr = ptr + 1;
                                    //clientid,user,password
                                    if (sptr) {
                                        ptr = strchr(sptr, ',');
                                        if (ptr) {
                                            //,user,password
                                            *ptr = '\0';
                                            clientid = sptr;
                                            sptr = ptr + 1;
                                            //user,password
                                            if (sptr) {
                                                ptr = strchr(sptr, ',');
                                                if (ptr) {
                                                    //,password
                                                    *ptr = '\0';
                                                    user = sptr;
                                                    sptr = ptr + 1;
                                                    //password
                                                    if (sptr) {
                                                        password = sptr;
                                                        strcpy(gateway->user, user);
                                                        strcpy(gateway->password, password);
                                                        strcpy(gateway->url, url);
                                                        strcpy(gateway->clientid, clientid);
                                                        gateway->port = port;
                                                        printf ("===> MQTTConfig user:%s\n", gateway->user); 
                                                        printf ("===> MQTTConfig password:%s\n", gateway->password); 
                                                        printf ("===> MQTTConfig url:%s\n", gateway->url); 
                                                        printf ("===> MQTTConfig port:%d\n", gateway->port); 
                                                        printf ("===> MQTTConfig clientid:%s\n", gateway->clientid); 
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (strstr(recv, "+REGEX:")) {
                    char *regex             = NULL;
                    printf ("info:%s\n", valid);
                    char *sptr = valid;//strchr(info, '<');
                    if (sptr) {
                        regex = (sptr);
                        strcpy(gateway->regex, regex);
                        printf ("===> REGEX :%s\n", gateway->regex);
                    }
                }
            }
        }
    }

    return -1;
}


void app_main(void) {
    int QueSize     = 1024;

    Gateway *gateway = (Gateway *) malloc (sizeof(*gateway));
    if (gateway) {
        memset(gateway, 0x0, sizeof(*gateway));

        strcpy(gateway->ethAddress, "192.168.0.105");
        strcpy(gateway->ethNetmask, "255.255.255.0");
        strcpy(gateway->ethGateway, "192.168.0.1");

        /* strcpy(gateway->wifiSsid, "TP-LINK_324B"); */
        /* strcpy(gateway->wifiPassword, "88888888"); */
        strcpy(gateway->wifiSsid, "LCZL2");
        strcpy(gateway->wifiPassword, "June123456");
        strcpy(gateway->wifiAddress, "0.0.0.0");
        strcpy(gateway->wifiNetmask, "0.0.0.0");
        strcpy(gateway->wifiGateway, "0.0.0.0");

        strcpy(gateway->user, "admin");
        strcpy(gateway->password, "123456");
        /* strcpy(gateway->url, "mqtt://broker.emqx.io:1883"); */
        /* strcpy(gateway->url, "mqtt://175.12.109.90:1883"); */
        strcpy(gateway->url, "mqtt://192.168.1.9:1883");
        /* strcpy(gateway->clientid, "99999999"); */
        strcpy(gateway->clientid, "88888888");

        strcpy(gateway->module, "GZ248");
        strcpy(gateway->hardwareVer, "1.0");
        strcpy(gateway->firmwareVer, "1.0");
        strcpy(gateway->mac, "000000000000");

        strcpy(gateway->regex, "GZ248_2");

        strcpy(gateway->info, "first");

        gateway->mode   = DC;
        gateway->level  = 100;
        gateway->temperature = 30.0;

        {
            /*queue*/
            int32_t index = 0;

            for (index = 0; index < DataAttr_Cnt; index++) {
#ifndef USE_RING_BUFFER
                gateway->bufQue[index] = xQueueCreate(QueSize, sizeof(int32_t *));
#else 
                gateway->bufQue[index] = CreateQueue(QueSize);
#endif
            }

        }

        {
            /*uart*/
            UartConfig config;
            memset(&config, 0x0, sizeof(config));

            config.uartIndex = UART_NUM_1;
            config.send = appSend;
            config.recv = appRecv;
            config.peek = appPeek;
            config.baudRate = 115200;
            config.dataBits = UART_DATA_8_BITS;
            config.parity = UART_PARITY_DISABLE;
            config.stopBits = UART_STOP_BITS_1;
            config.flowCtrl = UART_HW_FLOWCTRL_DISABLE;
            config.sourceClk = UART_SCLK_DEFAULT;


            UartInitLog(gateway, appPrint);
            UartSetLogLevel(LogUart_Info);

            gateway->uart = UartInit(&config);
            if (NULL == gateway->uart) {
                while (1) {
                    printf ("UartInit failure\n");
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }
            }
        }

        {
            /* printf ("%s %d\n", __func__, __LINE__); */
            ModuleMessage message;
            message.attr = ModuleDataAttr_GetTemperature;
            appSearchConfig(gateway, &message);
            /* printf ("%s %d\n", __func__, __LINE__); */

            message.attr = ModuleDataAttr_GetModuleVersion;
            appSearchConfig(gateway, &message);
            /* printf ("%s %d\n", __func__, __LINE__); */

            message.attr = ModuleDataAttr_GetModuleInfo;
            appSearchConfig(gateway, &message);
            /* printf ("%s %d\n", __func__, __LINE__); */

            message.attr = ModuleDataAttr_GetPower;
            appSearchConfig(gateway, &message);
            /* printf ("%s %d\n", __func__, __LINE__); */

            message.attr = ModuleDataAttr_GetMqttCfg;
            appSearchConfig(gateway, &message);
            /* printf ("%s %d\n", __func__, __LINE__); */
            /* printf ("%s %d\n", __func__, __LINE__); */
            message.attr = ModuleDataAttr_GetREGEX;
            appSearchConfig(gateway, &message);
        }
        {
            /*mqtt*/
            MQTTConfig config;
            memset(&config, 0x0, sizeof(config));

            strcpy(config.user, gateway->user);
            strcpy(config.password, gateway->password);
            strcpy(config.url, gateway->url);
            strcpy(config.clientId, gateway->clientid);
            strcpy(config.mac, gateway->mac);
            strcpy(config.regex, gateway->regex);

            /* char mac[12]; */
            /* esp_read_mac((uint8_t *)mac, ESP_MAC_WIFI_STA); */
            /* uint32_t clinetId = 88888888; */
            
            /* printf ("mac:%s\n", gateway->mac); */
            /* snprintf (config.clientId, sizeof(config.clientId) - 1, "%lu", clinetId); */
            /* snprintf (config.topic, sizeof(config.topic) - 1,  */
                    /* "GZ248_%02x%02x%02x%02x%02x%02x_%lu",  */
                    /* gateway->mac[0], gateway->mac[1], gateway->mac[2],  */
                    /* gateway->mac[3], gateway->mac[4], gateway->mac[5], clinetId); */

            MQTTInitLog(gateway, appPrint);
            MQTTSetLogLevel(LogMQTT_Info);

            config.send     = appSend;
            config.recv     = appRecv;
            config.peek     = appPeek;
            config.request  = appDataRequest;
            config.release  = appDataRelease;

            gateway->mqtt = MQTTInit(&config);
            if (NULL == gateway->mqtt) {
                while (1) {
                    printf ("MQTTInit failure\n");
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }
            }
        }
        /*
         * {
         *     ModuleMessage message;
         *     message.attr = ModuleDataAttr_GetWifiCfg;
         *     appSearchConfig(gateway, &message);
         * }
         * {
         *     ModuleMessage message;
         *     message.attr = ModuleDataAttr_GetEthCfg;
         *     appSearchConfig(gateway, &message);
         * }
         */
        {
            ModuleMessage message;
            message.attr = ModuleDataAttr_GetConfDown;
            appSearchConfig(gateway, &message);
        }

        /* printf ("%s %d\n", __func__, __LINE__); */

        {
            /*wifi*/
            WifiConfig config;
            memset(&config, 0x0, sizeof(config));

            /* strcpy(config.ssid, "TP-LINK_342B"); */
            /* strcpy(config.password, "88888888"); */
            strcpy(config.ssid, gateway->wifiSsid);
            strcpy(config.password, gateway->wifiPassword);
            strcpy(config.address, gateway->wifiAddress);
            strcpy(config.netmask, gateway->wifiNetmask);
            strcpy(config.gateway, gateway->wifiGateway);
            config.send = appSend;
            config.recv = appRecv;

            WifiInitLog(gateway, appPrint);
            WifiSetLogLevel(LogWifi_Info);

            gateway->wifi = WifiInit(&config);
            if (NULL == gateway->wifi) {
                while (1) {
                    printf ("WifiInit failure\n");
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }
            }
        }

            /* printf ("%s %d\n", __func__, __LINE__); */
        {
            /*ethernet*/
            EthConfig config;
            memset(&config, 0x0, sizeof(config));

            strcpy(config.address, gateway->ethAddress);
            strcpy(config.netmask, gateway->ethNetmask);
            strcpy(config.gateway, gateway->ethGateway);

            config.send = appSend;
            config.recv = appRecv;

            EthInitLog(gateway, appPrint);
            EthSetLogLevel(LogEth_Info);

            gateway->eth = EthInit(&config);
        }


        {
            //if version match
            //update
            UpdateConfig config;
            memset(&config, 0x0, sizeof(config));

            UpdateInitLog(gateway, appPrint);
            UpdateSetLogLevel(LogUpdate_Info);

            config.send = appSend;
            config.recv = appRecv;

            gateway->update = UpdateInit(&config);
            if (NULL == gateway->update) {
                while (1) {
                    printf ("UpdateInit failure\n");
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }
            }
        }

        {
            /*spi*/
            SpiConfig config;
            memset(&config, 0x0, sizeof(config));

            SpiInitLog(gateway, appPrint);
            SpiSetLogLevel(LogSpi_Info);

            config.send     = appSend;
            config.recv     = appRecv;
            config.request  = appDataRequest;
            config.request1  = appDataRequest1;
            config.release  = appDataRelease;
            strcpy(config.regex, gateway->regex);

            gateway->spi = SpiInit(&config);
            if (NULL == gateway->spi) {
                while (1) {
                    printf ("SpiInit failure\n");
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }
            }
        }


        {
            uint8_t CPU_RunInfo[400];
            while (1) {
                /* if (running++ == 6) { */
                /* printf ("%s %d\n", __func__, __LINE__); */
                /* MQTTTriggerRecv(gateway->mqtt); */
                /* } */
                /* char buf[128]; */

                for (int index = 0; index < DataAttr_Cnt; index++) {
                    printf ("index :%d sendNum:%d recvNum:%d\n", index, sendNum[index], recvNum[index]);
                }
                vTaskDelay(pdMS_TO_TICKS(5000));
                static int a = 0;
                printf ("%s %d num:%d\n", __func__, __LINE__, a++);
                /* ModuleMessage message; */
                /* message.attr = ModuleDataAttr_GetTemperature; */
                /* appSearchConfig(gateway, &message); */

                memset(CPU_RunInfo, 0, 400);
                vTaskGetRunTimeStats((char *)&CPU_RunInfo);

                printf("task_name      run_cnt                 usage_rate   \r\n");
                printf("%s", CPU_RunInfo);
                printf("----------------------------------------------------\r\n");
                printf ("all free mem:%lu\n", esp_get_free_heap_size());
                printf ("internal free mem:%u\n", heap_caps_get_free_size( MALLOC_CAP_INTERNAL ));
                printf ("spiram free mem:%u\n", heap_caps_get_free_size( MALLOC_CAP_SPIRAM ));
                printf("----------------------------------------------------\r\n");
                /* xPortGetFreeHeapSize(); */
                printf("----------------------------------------------------\r\n");

            }
        }
    }
}
