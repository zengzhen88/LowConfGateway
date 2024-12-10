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
    SpiSigRelease release;

    esp_timer_handle_t timer;
    int32_t signals;
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

void SpiRecvTask(void *args) {
    int status      = -1;
    Spi *spi        = (Spi *)args;
    Message *message= NULL;

    WORD_ALIGNED_ATTR char sendbuf[SPI_DATA_LENGTH + 1] = "";
    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));

    while (1) {
#if 1
        if (spi->request) {
            status = spi->request(gPriv, DataAttr_SpiToMqtt, &message, SPI_DATA_LENGTH);
            if (!status) {
                /* if (message->data) */
                    //Clear receive buffer, set send buffer to something sane
                    /* memset(recvbuf, 0xA5, 129); */
                    /* sprintf(sendbuf, "This is the receiver, sending data for transmission number %04d.", n); */

                //Set up a transaction of 128 bytes to send/receive
                t.length = SPI_DATA_LENGTH * 8;
                t.tx_buffer = sendbuf;
                t.rx_buffer = (char *)message->data;//recvbuf;
                message->size = SPI_DATA_LENGTH;
                message->length = SPI_DATA_LENGTH;
                /* This call enables the SPI slave interface to send/receive to the sendbuf and recvbuf. The transaction is
                   initialized by the SPI master, however, so it will not actually happen until the master starts a hardware transaction
                   by pulling CS low and pulsing the clock etc. In this specific example, we use the handshake line, pulled up by the
                   .post_setup_cb callback that is called as soon as a transaction is ready, to let the master know it is free to transfer
                   data.
                   */
/* #define USE_SPI_TEST  */
#ifndef USE_SPI_TEST
                status = spi_slave_transmit(RCV_HOST, &t, portMAX_DELAY);
#else 
                vTaskDelay(pdMS_TO_TICKS(200));
                static int32_t index = 0;
                char buffer[SPI_DATA_LENGTH+1];
                snprintf (buffer, sizeof(buffer) - 1, "1234567890qwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcv%ld", index++);
                strcpy((char *)t.rx_buffer, buffer);
                status = ESP_OK;
#endif
                if (ESP_OK == status) {
                    //spi_slave_transmit does not return until the master has done a transmission, so by here we have sent our data and
                    //received data from the master. Print it.
                    if (spi->send) {
                        status = spi->send(gPriv, DataAttr_SpiToMqtt, 
                                message, SPI_DATA_LENGTH, DataTimeStatus_UNBLOCK);
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
        }
#else 
        vTaskDelay(pdMS_TO_TICKS(5000));
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

void *SpiInit(SpiConfig *config) {
    esp_err_t status                = ESP_FAIL;        
    BaseType_t baseType             = pdFAIL;

    Spi *spi = (Spi *) malloc (sizeof(*spi));
    ERRP(NULL == spi, return NULL, 1, "malloc Spi Instance failure\n");
    memset(spi, 0x0, sizeof(*spi));

    spi->send   = config->send;
    spi->recv   = config->recv;
    spi->request= config->request;
    spi->release= config->release;

    //Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = GPIO_MISO,
        .sclk_io_num = GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };

    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg = {
        .mode = 0,
        .spics_io_num = GPIO_CS,
        .queue_size = 3,
        .flags = 0,
        .post_setup_cb = my_post_setup_cb,
        .post_trans_cb = my_post_trans_cb
    };

    //Configuration for the handshake line
    /* gpio_config_t io_conf = { */
        /* .intr_type = GPIO_INTR_DISABLE, */
        /* .mode = GPIO_MODE_OUTPUT, */
        /* .pin_bit_mask = BIT64(GPIO_HANDSHAKE), */
    /* }; */

#ifndef USE_SPI_TEST
    //Configure handshake line as output
    /* gpio_config(&io_conf); */
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

    LogPrintf(LogSpi_Info, "<<<spi->spiSok>>>\n");

    return spi;
ERR04:
    return NULL;
}


