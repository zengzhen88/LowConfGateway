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
#include <eth.h>
#include <message.h>
#include <update.h>

#define QueSize (15)

typedef struct {
    void *wifi;
    void *eth;
    void *mqtt;
    void *update;

    QueueHandle_t bufQue[DataAttr_Cnt];
} Gateway;

int32_t appPrint(void *priv, const char *strings) {
    return printf ("%s", strings);
}


int32_t appSend(void *priv, DataAttr attr, 
        void *data, int32_t fillLength, int32_t millis) {
    int status       = ESP_FAIL;
    Message *message = NULL;
    TickType_t block = portMAX_DELAY; 
    Gateway *gateway = (Gateway *)priv;

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
            return 0;
        }

        ReleaseMessage(message);
    }


    return -1;
}

int32_t appRecv(void *priv, DataAttr attr, 
        void *data, int32_t *fillLength, int32_t millis) {
    int32_t status   = -1;
    Message *message = NULL;
    TickType_t block = portMAX_DELAY; 
    Gateway *gateway = (Gateway *)priv;

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
            //内存足够才送数据
            memcpy(data, message->data, message->length);
            *fillLength = message->length;
        }

        ReleaseMessage(message);

        return 0;
    }

    return -1;
}


void app_main(void) {
    Gateway *gateway = (Gateway *) malloc (sizeof(*gateway));
    if (gateway) {
        memset(gateway, 0x0, sizeof(*gateway));

        {
            /*queue*/
            int32_t index = 0;

            for (index = 0; index < DataAttr_Cnt; index++) {
                gateway->bufQue[index] = xQueueCreate(QueSize, sizeof(int32_t *));
            }
        }

        {
            /*wifi*/
            WifiConfig config;
            memset(&config, 0x0, sizeof(config));

            strcpy(config.ssid, "TP-LINK_342B");
            strcpy(config.password, "88888888");
            config.send = appSend;
            config.recv = appRecv;

            WifiInitLog(gateway, appPrint);
            WifiSetLogLevel(LogWifi_Info);

            gateway->wifi = WifiInit(&config);
        }

/*
 *         {
 *             [>ethernet<]
 *             EthConfig config;
 *             memset(&config, 0x0, sizeof(config));
 * 
 *             config.send = appSend;
 *             config.recv = appRecv;
 * 
 *             EthInitLog(gateway, appPrint);
 *             EthSetLogLevel(LogEth_Info);
 * 
 *             gateway->eth = EthInit(&config);
 *         }
 */

        {
            /*update*/
        }

        {
            /*mqtt*/
            MQTTConfig config;
            memset(&config, 0x0, sizeof(config));

            MQTTInitLog(gateway, appPrint);
            MQTTSetLogLevel(LogMQTT_Info);

            strcpy(config.name, "Pushlish");
            strcpy(config.username, "admin");
            strcpy(config.password, "123456");
            strcpy(config.url, "mqtt://192.168.0.107:1883");
            strcpy(config.hostname, "192.168.0.107");
            config.port = 1883;
            config.send = appSend;
            config.recv = appRecv;

            gateway->mqtt = MQTTInit(&config);
        }

        {
            /*update*/
            UpdateConfig config;
            memset(&config, 0x0, sizeof(config));

            UpdateInitLog(gateway, appPrint);
            UpdateSetLogLevel(LogUpdate_Info);

            config.send = appSend;
            config.recv = appRecv;

            gateway->update = UpdateInit(&config);
        }

        {
            /* int running = 0; */
            while (1) {
                /* char buf[128]; */

                vTaskDelay(pdMS_TO_TICKS(2000));

                /* snprintf (buf, sizeof(buf) - 1, "running_%d", running++); */
                /* MQTTPushlish(gateway->mqtt, buf, strlen(buf), "zengzhen", 1, 0); */
            }
        }
    }
}
