
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ui.h>
#include <tska_que.h>
#include <tska_thr.h>
#include <uart.h>
#include <http.h>
#include <mqtt.h>
#include <uart.h>
#include "cJSON.h"
#include <lvgl/lvgl.h>
extern "C" {
#include <message.h>
}

static void *gPriv = NULL;
static UIPrint gPrint;
static LogUI gLevel = LogUI_Info;


/*************************************************/ 
/**************LogPrintf**************************/
/*************************************************/ 

static int UILogPrintf(LogUI level,
        const char *file, const char *func, 
        int line, const char *format, ...) {
    char logBuf[1024];
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
        UILogPrintf(level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)     
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogUI_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogUI_Debug, __VA_ARGS__);
#endif

#ifndef ERRP
#define ERRP(con, ret, flag, ...)               \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif

typedef struct {
    /*mqtt*/
    void *mqttpublish;
    void *mqttsubscribe;

    int8_t username[128];
    int8_t password[128];

    int8_t topic[128];  /*主题名*/
    int8_t host[128];   /*ip*/
    int16_t port;       /*port*/
    int32_t qos;        /*传输质量  0, 1, 2*/

    int32_t mqttthreading;
    TSKA_ThrHndl mqttthread;
    TSKA_QueHndl bufQue;

// #define MQTTTEST
#ifdef MQTTTEST
    void *mqttsubscribe;
    TSKA_ThrHndl mqtttestthread;
#endif

    int32_t ackValid;
    int32_t connect;

    /*uart*/
    void *uart;
} UIObj;

/*
 * MQTT MQTT MQTT MQTT
 * MQTT MQTT MQTT MQTT
 * MQTT MQTT MQTT MQTT
 * MQTT MQTT MQTT MQTT
 * MQTT MQTT MQTT MQTT
 */
//设置日志输出对象，主要看你想输出到哪里，就对应填充回调
int32_t UIInitLog(void *priv, UIPrint print) {
    gPriv   = priv;
    gPrint  = print;

    return 0;
}

//设置日志输出等级，看需要，调试、错误、宕机等
int32_t UISetLogLevel(LogUI level) {
    gLevel  = level;

    return 0;
}

static int32_t UIMqttPushlishConnect(void *oObj) {
    UIObj *pObj    = (UIObj *)oObj;

    pObj->connect = 1;
    LogPrintf(LogUI_Info, "%s %d push connect\n", __func__, __LINE__);
    return 0;
}

static int32_t UIMqttPushlishDisconnect(void *oObj) {
    // UIObj *pObj    = (UIObj *)oObj;

    LogPrintf(LogUI_Info, "%s %d\n", __func__, __LINE__);
    return 0;
}

static int32_t UIMqttPushlish(void *oObj) {
    // UIObj *pObj    = (UIObj *)oObj;

    LogPrintf(LogUI_Info, "%s %d push data\n", __func__, __LINE__);
    return 0;
}

/*
 * static int32_t UIMqttSubscribeConnect(void *oObj) {
 *     // UIObj *pObj    = (UIObj *)oObj;
 * 
 *     LogPrintf(LogUI_Info, "%s %d\n", __func__, __LINE__);
 *     return 0;
 * }
 */
/*
 * 
 * static int32_t UIMqttSubscribeDisconnect(void *oObj) {
 *     // UIObj *pObj    = (UIObj *)oObj;
 * 
 *     LogPrintf(LogUI_Info, "%s %d\n", __func__, __LINE__);
 *     return 0;
 * }
 */

/*
 * static int32_t UIMqttSubscribeMessage(void *oObj) {
 *     UIObj *pObj    = (UIObj *)oObj;
 * 
 *     pObj->ackValid = 1;
 * 
 *     // LogPrintf(LogUI_Info, "%s %d\n", __func__, __LINE__);
 *     return 0;
 * }
 */

/*
 * static int32_t UIMqttSubscribe(void *oObj) {
 *     LogPrintf(LogUI_Info, "%s %d\n", __func__, __LINE__);
 *     // UIObj *pObj    = (UIObj *)oObj;
 * 
 *     return 0;
 * }
 */

/*
 * MQTT EXIT
 * MQTT EXIT
 * MQTT EXIT
 * MQTT EXIT
 * MQTT EXIT
 */

/**
 * UART UART UART
 * UART UART UART
 * UART UART UART
 * UART UART UART
 * UART UART UART
 */

/*
 * UART EXIT
 * UART EXIT
 * UART EXIT
 * UART EXIT
 * UART EXIT
 * UART EXIT
 */

//初始化各个模块
void *UIInit(void) {
    UIObj *pObj    = NULL;

    pObj = (UIObj *) malloc (sizeof(*pObj));
    ERRP(NULL == pObj, goto ERR0, 1, "malloc UIObj failure\n");
    memset(pObj, 0x0, sizeof(*pObj)); //整个句柄做初始化处理

    strcpy((char *)pObj->topic, "amdin");       /*default*/
    strcpy((char *)pObj->host, "192.168.0.107");      /*default*/
    strcpy((char *)pObj->username, "admin");    /*default*/
    strcpy((char *)pObj->password, "123456");   /*default*/
    pObj->port  = 1883;                 /*default*/
    pObj->qos   = 0;                    /*default*/

    MQTTInitLog(pObj, gPrint);
    MQTTInit();


    int32_t mqtt = 0;
    if (mqtt) {
        pObj->mqttpublish = MQTTNewPublish(
                (const char *)pObj->username, 
                (const char *)pObj->password,
                toAckEnumString(ModuleDataAttr_SetWifiCfg), 
                (const char *)pObj->host, 
                pObj->port, 
                pObj->qos, 
                UIMqttPushlishConnect, 
                UIMqttPushlishDisconnect,
                UIMqttPushlish, 
                pObj);
        while (!pObj->connect) {
            sleep(1);
        }

        if (pObj->mqttpublish) {
            cJSON *root = NULL;
            cJSON *sub  = NULL;

            root = cJSON_CreateArray();
            if (root) {
                sub = cJSON_CreateObject();
                if (sub) {
                    cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_SetWifiCfg));
                    cJSON_AddStringToObject(sub, "ssid", "TP-LINK_342B");
                    cJSON_AddStringToObject(sub, "password", "88888888");
                    // cJSON_AddStringToObject(sub, "ssid", "zz");
                    // cJSON_AddStringToObject(sub, "password", "12345678");
                    // cJSON_AddStringToObject(sub, "address", "0.0.0.0");
                    // cJSON_AddStringToObject(sub, "netmask", "255.255.255.0");
                    // cJSON_AddStringToObject(sub, "ui", "192.168.0.1");
                    cJSON_AddStringToObject(sub, "address", "192.168.0.103");
                    cJSON_AddStringToObject(sub, "netmask", "255.255.255.0");
                    cJSON_AddStringToObject(sub, "ui", "192.168.0.1");
                    cJSON_AddItemToArray(root, sub);
                    char *json = cJSON_Print(root);
                    if (json) {
                        LogPrintf(LogUI_Info, "json:\r\n%s\n", json);
                        MQTTPush(pObj->mqttpublish,
                                (const char *)json,
                                strlen(json));
                        free(json);
                    }
                    cJSON_Delete(root);
                }
            }
        }

        while (!pObj->ackValid) {
            sleep(1);
        }

        LogPrintf(LogUI_Info, "Message %s suc\n", toEnumString(ModuleDataAttr_SetWifiCfg));
        MQTTDestory(pObj->mqttpublish);
        MQTTDestory(pObj->mqttsubscribe);
    }

    {
        // lvgl 初始化
        lv_init();
        // 初始化显示器和创建用于绘图的缓冲区
        lv_port_disp_init();
        //找到 LittelvGL 支持的输入设备
        lv_port_indev_init();

        /*更改活动屏幕的背景颜色*/
        lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);

        /*创建一个白色标签，设置其文本并将其与中心对齐*/
        lv_obj_t * mainLabel = lv_label_create(lv_scr_act());
        lv_label_set_text(mainLabel, "欢迎使用GZ248客户端软件");
        lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0xffffff), LV_PART_MAIN);
        lv_obj_align(mainLabel, LV_ALIGN_CENTER, 0, 0);
    }

    return pObj;
ERR0:
    return NULL;
}
