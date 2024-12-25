#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/ip_addr.h"

#include <spi.h>
#include <message.h>
#include "esp_timer.h"
#include "cJSON.h"

static void *gPriv = NULL;
static SpiPrint gPrint;
static LogSpi gLevel = LogSpi_Info;


/*************************************************/
/**************LogPrintf**************************/
/*************************************************/



static int SpiLogPrintf(LogSpi level,
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
    SpiLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogSpi_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogSpi_Debug, __VA_ARGS__);
#endif

#ifndef ERRP
#define ERRP(con, ret, flag, ...)               \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif

#define SPI_UPDATE_USERPASSWORD BIT0

typedef struct {
    int32_t isRunning;

    TaskHandle_t spiTask;

    SpiSigSend send;
    SpiSigRecv recv;
    SpiSigRequest request;
    SpiSigRequest1 request1;
    SpiSigRelease release;

    esp_timer_handle_t timer;
    int32_t signals;

    int32_t isRegex; /*正则表达式是否有效*/
    int32_t devType;  /*上报的设备类型，内部、外部 和所有  1 0 2*/

    int32_t regexCount;
    char **regex;

    int32_t reportTime;
} Spi;

//设置日志输出对象，主要看你想输出到哪里，就对应填充回调
int32_t SpiInitLog(void *priv, SpiPrint print) {
    gPriv   = priv;
    gPrint  = print;

    return 0;
}

//设置日志输出等级，看需要，调试、错误、宕机等
int32_t SpiSetLogLevel(LogSpi level) {
    gLevel  = level;

    return 0;
}

#define GPIO_HANDSHAKE      2
#define GPIO_MOSI           38
#define GPIO_MISO           13
#define GPIO_SCLK           39
#define GPIO_CS             37
#define GPIO_INIT           34
#define GPIO_TRIGGER        4

#ifdef CONFIG_IDF_TARGET_ESP32
#define RCV_HOST    HSPI_HOST
#else
#define RCV_HOST    SPI2_HOST
#endif

typedef enum {
    MYSPIERNET_EVENT_START,
} myeth_event_t;
ESP_EVENT_DEFINE_BASE(MYSPI_EVENT);
#define MYSPI_EVENT_SPI_ANY_ID (-1)


int callbackCompute(void *key, void *_key) {
    if (!strcmp(key, _key)) {
        return 0;
    }

    return -1;
}

//Called after a transaction is queued and ready for pickup by master. We use this to set the handshake line high.
void my_post_setup_cb(spi_slave_transaction_t *trans)
{
    gpio_set_level(GPIO_HANDSHAKE, 1);
}

//Called after transaction is sent/received. We use this to set the handshake line low.
void my_post_trans_cb(spi_slave_transaction_t *trans)
{
    gpio_set_level(GPIO_HANDSHAKE, 0);
}

static int32_t isValidRegex(Spi *spi, char *data);
void SpiEventHandler(void* arg, esp_event_base_t event_base,
        int32_t event_id, void *event_data) {
    esp_err_t status                = ESP_FAIL;        
    Spi *spi = (Spi *)arg;

    LogPrintf(LogSpi_Info, "eventBase:%s eventId:%d\n", 
            event_base, event_id);

    if (event_base == MYSPI_EVENT) {
        switch (event_id) {
            case MYSPIERNET_EVENT_START:
                {
                    ModuleMessage message;
                    int32_t length = sizeof(message);
                    int status = spi->recv(gPriv, DataAttr_MqttToSpi, &message, &length, 0);
                    if (!status) {
                        /* SpiEventRecvHandler(spi); */
                        isValidRegex(spi, (char *)message.setREGEX.data);
                    }
                    break;
                }
            default:break;
        }
    }
}

int32_t SpiTriggerRecv(void *data) {
    Spi *spi = (Spi *)data;

    if (spi) {
        ModuleInternalMessage message;
        message.attr = ModuleInternalDataAttr_TriggerRecv;
        esp_event_post(MYSPI_EVENT, 
                MYSPIERNET_EVENT_START, &message, sizeof(message), 0);
        return 0;
    }

    return -1;
}

unsigned char IntToHexChar(unsigned char c) {
    if (c > 9) return (c + 55);
    else return (c + 0x30);
}

void SpiRecvTask(void *args) {
    int status      = -1;
    Spi *spi        = (Spi *)args;
    Message *message= NULL;

    WORD_ALIGNED_ATTR char sendbuf[SPI_DATA_LENGTH + 1] = "";
    WORD_ALIGNED_ATTR char recvbuf[SPI_DATA_LENGTH + 1] = "";
    /* WORD_ALIGNED_ATTR char combinebuf[(SPI_DATA_LENGTH << 1) + 1] = ""; */
    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));
    cJSON *root = NULL;
    cJSON *sub  = NULL;
    int messLen = 10;
    int messNum = 0;
    int allMessNum = 0;
    int32_t first = 0;
    int32_t add = 0;
    int32_t del = 0;
    int32_t index = 0;
    uint32_t timeDiff = esp_log_early_timestamp();
    /* char *sString = (char *)alloca(81); */
#define MessageNum (50)  //最大只发40个数据包，一次
    char *combineCollent[256] = { //目前最大256个蓝牙
        NULL
    };

    char par = '0';
    while (1) {
#if 1
        messNum = 0;
        root = cJSON_CreateArray();
        if (root) {
            sub = cJSON_CreateObject();
            if (sub) {
                cJSON_AddStringToObject(sub, "htype", toEnumString(ModuleDataAttr_TransmitData));
                cJSON *psJSON = cJSON_CreateArray();
                if (psJSON) {
                    /* messNum = 0; */
                    while (1) {
                        //如果实际的数据单位大于messLen
                        /* ERRP (messNum >= messLen, break, 0); */
#endif
                        char *combinebuf = (char *) malloc ((SPI_DATA_LENGTH << 1) + 2);
                        if (NULL != combinebuf) {
                            while (1) {
                                t.length = SPI_DATA_LENGTH * 8;
                                t.tx_buffer = sendbuf;
                                if (!first) {
                                    t.rx_buffer = combinebuf;//recvbuf;
                                }
                                else {
                                    t.rx_buffer = &combinebuf[SPI_DATA_LENGTH - 2];//recvbuf;
                                }

                                /* #define USE_SPI_TEST  */
#ifndef USE_SPI_TEST
                                status = spi_slave_transmit(RCV_HOST, &t, portMAX_DELAY);
#else 
                                vTaskDelay(pdMS_TO_TICKS(5));
                                static int32_t index = 0;
                                snprintf ((char *)t.rx_buffer, sizeof(recvbuf) - 1, "1234567820qwertyuiopasdfghj%ld", index++);
                                status = ESP_OK;
#endif
                                /* char spp[44]; */
                                /* strcpy(spp, t.rx_buffer); */
                                /* printf ("spp:%s\n", spp); */

                                if (!first) {
                                    if (((char *)t.rx_buffer)[SPI_DATA_LENGTH - 1] == '+') {
                                        /* printf ("1 recv +\n"); */
                                        add++;
                                        par = ((char *)t.rx_buffer)[SPI_DATA_LENGTH - 2]; //保存校验
                                        /* printf ("11 recv par:%c\n", par); */
                                        //收到第一组数据，
                                        first = !first;
                                        continue;
                                    }
                                    else if (((char *)t.rx_buffer)[SPI_DATA_LENGTH - 1] == '-') {
                                        /* printf ("2 recv -\n"); */
                                        del++;
                                        //收到一个尾部数据，需要丢掉
                                        first = 0;
                                        continue;
                                    }
                                    else {
                                        /* printf ("3 recv x\n"); */
                                        //收到非预期数据，需要丢掉
                                        first = 0;
                                        continue;
                                    }
                                }
                                else {
                                    if (((char *)t.rx_buffer)[SPI_DATA_LENGTH - 1] == '+') {
                                        /* printf ("4 recv +\n"); */
                                        add++;
                                        //收到第一组数据,需要丢掉上一次的数据
                                        /* printf ("recv first, so drop prev first\n"); */
                                        memcpy(combinebuf, &combinebuf[SPI_DATA_LENGTH - 2], SPI_DATA_LENGTH - 2);
                                        first = 1;
                                        par = ((char *)t.rx_buffer)[SPI_DATA_LENGTH - 2]; //保存校验
                                        /* printf ("44 recv par:%c\n", par); */
                                        continue;
                                    }
                                    else if (((char *)t.rx_buffer)[SPI_DATA_LENGTH - 1] == '-') {
                                        /* printf ("5 recv -\n"); */
                                        if (((char *)t.rx_buffer)[SPI_DATA_LENGTH - 2] == par) {
                                            /* printf ("55 recv par:%c match\n", par); */
                                            del++;
                                            //收到第二组数据，有效
                                            first = !first;
                                            break;
                                        }
                                        else {
                                            /* printf ("555 recv par:%c not match\n", par); */
                                            //连续丢掉了前一次的尾部及前一次的头
                                            first = 0;
                                            continue;
                                        }
                                    }
                                    else {
                                        /* printf ("6 recv x\n"); */
                                        first = 0;
                                        continue;
                                        //收到非预期数据，需要丢掉
                                    }
                                }
                            }

                            combinebuf[(SPI_DATA_LENGTH << 1) - 4] = '\0';
                            /* printf ("add:%ld del:%ld combinebuf:%s\n", add, del, combinebuf); */
#if 1
                            char devType[16];
                            strncpy(devType, &((char *)combinebuf)[78], 2);
                            devType[2] = '\0';
                            (((char *)combinebuf)[78]) = '\0';
                            if (spi->isRegex) {
                                //正则表达式生效中
                                switch (spi->devType) {
                                    case 0://外部
                                    case 1://内部
                                        {
                                            if ((atoi(devType) != spi->devType)) {
                                                LogPrintf(LogSpi_Warning, 
                                                        "DevType:%d Match:%d failure\n", 
                                                        atoi(devType), spi->devType);
                                                //设备类型没有匹配不成功
                                                continue;
                                            }
                                            break;
                                        }
                                    case 2://所有
                                    default:
                                        {
                                            break;
                                        }
                                }
                                int32_t match = 0;
                                for (index = 0; index < spi->regexCount; index++) {
                                    if (NULL == strstr((char *)combinebuf, spi->regex[index])) {
                                        LogPrintf(LogSpi_Warning,
                                                "Message Match[%d]:%s failure\n",
                                                index, spi->regex[index]);
                                        //数据相关信息匹配不成功
                                        match = 1;
                                        break;
                                    }
                                }
                                if (match) {
                                    continue;
                                }
                            }

                            int32_t mark = 0;
                            for (index = 0; index < messNum; index++) {
                                if (!strncmp(combineCollent[index], combinebuf, 6)) {
                                    free(combineCollent[index]);
                                    combineCollent[index] = combinebuf;//更新
                                    mark = 1;
                                    break;
                                }
                            }

                            if (!mark) {
                                combineCollent[messNum] = combinebuf;
                            }

                            messNum++;

                            if (esp_log_early_timestamp() - timeDiff >= spi->reportTime) {
                                timeDiff = esp_log_early_timestamp();
                                break;
                            }
                        }
                    }
                    if (messNum) {
                        for (index = 0; index < messNum; index++) {
                            cJSON *psQJSON = cJSON_CreateObject();
                            if (psQJSON) {
                                /* printf ("json:%s\n", combineCollent[index]); */
                                cJSON_AddNumberToObject(psQJSON, "timestamp", esp_log_early_timestamp());
                                cJSON_AddStringToObject(psQJSON, "data", combineCollent[index]);
                                free(combineCollent[index]);
                                cJSON_AddItemToArray(psJSON, psQJSON);
                            }
                        }

                        cJSON_AddItemToObject(sub, "dataAdv", psJSON);

                        cJSON_AddItemToArray(root, sub);
                        char *json = cJSON_Print(root);
                        if (json) {
                            /* LogPrintf(LogSpi_Info, "json:%d\n", strlen(json)); */
                            if (spi->request1) {
                                status = spi->request1(gPriv, DataAttr_SpiToMqtt, &message, (void **)&json, strlen(json));
                                if (!status) {
                                    //Set up a transaction of 128 bytes to send/receive
                                    message->size = strlen(json);
                                    message->length = strlen(json);
                                    /* This call enables the SPI slave interface to send/receive to the sendbuf and recvbuf. The transaction is
                                       initialized by the SPI master, however, so it will not actually happen until the master starts a hardware transaction
                                       by pulling CS low and pulsing the clock etc. In this specific example, we use the handshake line, pulled up by the
                                       .post_setup_cb callback that is called as soon as a transaction is ready, to let the master know it is free to transfer
                                       data.
                                       */
                                    if (ESP_OK == status) {
                                        //spi_slave_transmit does not return until the master has done a transmission, so by here we have sent our data and
                                        //received data from the master. Print it.
                                        if (spi->send) {
                                            status = spi->send(gPriv, DataAttr_SpiToMqtt, 
                                                    message, strlen(json) + 1, DataTimeStatus_UNBLOCK);
                                            if (status) {
                                                if (spi->release)
                                                    spi->release(gPriv, DataAttr_SpiToMqtt, message);
                                            }
                                        }
                                        else {
                                            if (spi->release)
                                                spi->release(gPriv, DataAttr_SpiToMqtt, message);
                                        }
                                    }
                                    else {
                                        if (spi->release)
                                            spi->release(gPriv, DataAttr_SpiToMqtt, message);
                                    }
                                }
                                else {
                                    free(json);
                                }
                            }
                            else {
                                free(json);
                            }
                        }
                    }
                }
            }
            cJSON_Delete(root);
        }
#endif
    }
}

/*
 * #include <esp_rom_sys.h>
 * static void timer_cb(void *arg) {
 *     Spi *spi = (Spi *)arg;
 * 
 *     gpio_set_level(GPIO_TRIGGER, spi->signals);
 *     esp_rom_delay_us(10);
 *     LogPrintf(LogSpi_Info, "signal:%d\n", gpio_get_level(GPIO_TRIGGER));
 *     spi->signals = !spi->signals;
 * }
 */

int32_t isValidRegex(Spi *spi, char *data) {
    /*GZ248:[0,2];23*/
    int32_t index = strlen(data);
    char *newPtr = (char *) alloca (index + 1);
    if (newPtr) {
        strcpy(newPtr, data);
    }
    else {
        memset(newPtr, 0x0, index);
    }
    char *ptr = (char *)newPtr;
    /* char *devType = NULL; */
    /*GZ248:[0,2];23*/
    /* char *sptr = strchr(ptr, ':'); */
    char *sptr = NULL;
    char *initPtr = NULL;
    /* if (sptr) */ 
    {
        /* :[0,2];23 */
        /* *sptr = '\0'; */
        /*\0[0,2];23*/
        /* sptr = sptr + 1; */
        /*[0,2];23*/
        if (strstr(ptr, "GZ248")) {
            sptr = strchr(ptr, '_');
            if (sptr) {
                spi->devType = atoi(sptr + 1);

                int32_t count = 0;
                char *delim = ";";
                char *chrr = NULL;

                initPtr = strchr(sptr, ';'); //指向第一个字段
                if (initPtr) {
                    chrr = initPtr + 1;
                    if (chrr) {
                        count++;//当前有第一个
                        while (1) {
                            char *chr = strchr(chrr, ';');
                            if (NULL == chr) {
                                break;
                            }
                            chrr = chr + 1;
                            //当前有新增加一个
                            count++;
                        }

                        if (spi->regexCount > 0) {
                            if (spi->regex) {
                                for (index = 0; index < spi->regexCount; index++) {
                                    if (spi->regex[index] != NULL) {
                                        free(spi->regex[index]);
                                    }
                                }

                                free(spi->regex);
                            }
                        }

                        spi->regexCount = count;
                        spi->regex = (char **) malloc (sizeof(char *) * spi->regexCount);
                        if (spi->regex) {
                            index = 0;
                            memset(spi->regex, 0x0, sizeof(char *) * spi->regexCount);
                            LogPrintf(LogSpi_Info, "spi->regexCount:%d\n", spi->regexCount);

                            chrr = initPtr + 1;
                            char *tok = strtok(chrr, delim);
                            if (NULL != tok) {
                                LogPrintf(LogSpi_Info, "tokxxxx:%s\n", tok);
                                spi->regex[index] = strdup(tok);
                                if (NULL != spi->regex[index++]) {
                                    while (tok != NULL) {
                                        tok = strtok(NULL, delim);
                                        if (NULL != tok) {
                                            spi->regex[index] = strdup(tok);
                                            if (NULL != spi->regex[index]) {
                                                index++;
                                            }
                                            else {
                                                LogPrintf(LogSpi_Error, "strdup spi->regex[%d] failure\n", index);
                                            }
                                        }
                                    }
                                }
                                else {
                                    LogPrintf(LogSpi_Error, "strdup spi>regex[0] failure\n");
                                    spi->isRegex = 0;
                                    return -1;
                                }
                            }
                        }
                        else {
                            spi->isRegex = 0;
                            LogPrintf(LogSpi_Error, "malloc spi->regex failure\n");
                            return -1;
                        }

                    }
                }
                for (index = 0; index < spi->regexCount; index++) {
                    if (spi->regex[index]) 
                        LogPrintf(LogSpi_Info, "spi->regex[%d]:%s\n", index, spi->regex[index]);
                }

                spi->isRegex = 1;
            }
        }
    }

    LogPrintf(LogSpi_Info, "regex isRegex(%d), devType(%d) initRegex:%s\n", 
            spi->isRegex, spi->devType, data);

    return -1;
}

void *SpiInit(SpiConfig *config) {
    esp_err_t status                = ESP_FAIL;        
    BaseType_t baseType             = pdFAIL;

    Spi *spi = (Spi *) malloc (sizeof(*spi));
    ERRP(NULL == spi, return NULL, 1, "malloc Spi Instance failure\n");
    memset(spi, 0x0, sizeof(*spi));

    spi->send   = config->send;
    spi->recv   = config->recv;
    spi->request= config->request;
    spi->request1= config->request1;
    spi->release= config->release;
    spi->reportTime = 1;

    LogPrintf(LogSpi_Info, "spi regex:%s\n", config->regex);
    isValidRegex(spi, config->regex);

    //Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = -1,//GPIO_MISO,
        .sclk_io_num = GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };

    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg = {
        .mode = 0,
        .spics_io_num = GPIO_CS,
        .queue_size = 50,
        .flags = 0,
        .post_setup_cb = my_post_setup_cb,
        .post_trans_cb = my_post_trans_cb
    };

    /* Configuration for the handshake line */
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = BIT64(GPIO_HANDSHAKE),
    };

#ifndef USE_SPI_TEST
    //Configure handshake line as output
    gpio_config(&io_conf);
    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);

    /* gpio_set_pull_mode(GPIO_INIT, GPIO_PULLUP_ONLY); */
    /* gpio_set_pull_mode(GPIO_TRIGGER, GPIO_PULLUP_PULLDOWN); */
    /*
     * gpio_config_t gpio_cfg = {
     *     .pin_bit_mask = (1ULL << GPIO_TRIGGER),
     *     .intr_type = GPIO_INTR_DISABLE,
     *     .mode = GPIO_MODE_OUTPUT,
     *     .pull_up_en = 0,
     *     .pull_down_en = 0,
     * };
     * gpio_config(&gpio_cfg);
     */

    /* gpio_set_direction(GPIO_INIT, GPIO_MODE_INPUT); */
    /* gpio_set_level(GPIO_INIT, 1); */

    /* gpio_set_direction(GPIO_TRIGGER, GPIO_MODE_OUTPUT); */

    //Initialize SPI slave interface
    status = spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    assert(status == ESP_OK);

    /*
     * const esp_timer_create_args_t timer_args = {
     *     timer_cb,
     *     spi,
     *     ESP_TIMER_TASK,
     *     "spi_timer",
     *     true,
     * };
     * esp_timer_create(&timer_args, &spi->timer);
     * esp_timer_start_periodic(spi->timer, 1000000);//10ms
     */
#endif

    baseType = xTaskCreate(SpiRecvTask, 
            "spiRecvTask", 4096, spi, 5, &spi->spiTask);
    ERRP(pdPASS != baseType, goto ERR04, 1, "spi xTaskCreate failure\n");

    esp_event_handler_register(MYSPI_EVENT, MYSPI_EVENT_SPI_ANY_ID, SpiEventHandler, spi);

    LogPrintf(LogSpi_Info, "<<<spi->spiSok>>>\n");

    return spi;
ERR04:
    return NULL;
}


