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

typedef struct {
    void *wifi;
    void *mqtt;
} Gateway;

int32_t appPrint(void *priv, const char *strings) {
    return printf ("%s", strings);
}

void app_main(void) {
    Gateway *gateway = (Gateway *) malloc (sizeof(*gateway));
    if (gateway) {
        memset(gateway, 0x0, sizeof(*gateway));

        {
            /*wifi*/
            WifiConfig config;
            memset(&config, 0x0, sizeof(config));

            strcpy(config.ssid, "TP-LINK_342B");
            strcpy(config.password, "88888888");

            WifiInitLog(gateway, appPrint);
            WifiSetLogLevel(LogWifi_Info);

            gateway->wifi = WifiInit(&config);
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
            strcpy(config.url, "mqtt://192.168.0.106:1883");
            strcpy(config.hostname, "192.168.0.106");
            config.port = 1883;

            gateway->mqtt = MQTTInit(&config);

            MQTTSubscribe(gateway->mqtt, "zengzhen", 1);
        }

        {
            int running = 0;
            while (1) {
                char buf[128];

                vTaskDelay(pdMS_TO_TICKS(2000));

                snprintf (buf, sizeof(buf) - 1, "running_%d", running++);
                MQTTPushlish(gateway->mqtt, buf, strlen(buf), "zengzhen", 1, 0);
            }
        }
    }
}
