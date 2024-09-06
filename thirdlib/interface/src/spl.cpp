#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "spl.h"

static uint8_t mode = 0;                 /* SPI 通信使用全双工，设置 CPOL＝0，CPHA＝0。 */
static uint8_t bits = 8;                 /* 8bits读写，MSB first。 */
static uint32_t speed = 5 * 1000 * 1000; /* 设置传输速度 */
static uint16_t delay = 0;

static int g_SPI_fd = 0;

static void pabort(const char *s)
{
    perror(s);
    abort();
}

int SPI_transfer(const uint8_t *wdata, uint8_t *rdata, uint16_t len)
{
    int ret;
    int fd = g_SPI_fd;

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)wdata,
        .rx_buf = (unsigned long)rdata,
        .len = len,
        .delay_usecs = delay,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

    if (ret < 1)
    {
        perror("can't send spi message\n");
    }
#ifdef SPI_DEBUG
    else
    {
        int i;
        printf("nsend spi message succeed\n");
        printf("nSPI Send [Len:%d]: ", len);
        for (i = 0; i < len; i++)
        {
            printf("%02X ", tx_buf[i]);
        }
        printf("\r\n");
        printf("SPI Receive [len:%d]: ", len);
        for (i = 0; i < len; i++)
        {
            printf("%02X ", rx_buf[i]);
        }
        printf("\r\n");
    }
#endif
    return ret;
}

int SPI_open(char *spi_dev)
{
    int fd;
    int ret = 0;

    /* 设备已打开 */
    if (g_SPI_fd != 0)
    {
        return 0xF1;
    }

    fd = open(spi_dev, O_RDWR);
    if (fd < 0)
    {
        pabort("can't open device\n");
    }
#ifdef SPI_DEBUG
    else
    {
        printf("SPI - Open Succeed. Start Init SPI...\n");
    }
#endif

    g_SPI_fd = fd;

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
    {
        pabort("can't set spi mode\n");
    }
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
    {
        pabort("can't get spi mode\n");
    }

    // bits per word
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
    {
        pabort("can't set bits per word\n");
    }
    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
    {
        pabort("can't get bits per word\n");
    }

    // max speed hz
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
    {
        pabort("can't set max speed hz\n");
    }
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
    {
        pabort("can't get max speed hz\n");
    }

#ifdef SPI_DEBUG
    printf("spi mode: %d\n", mode);
    printf("bits per word: %d\n", bits);
    printf("max speed: %d KHz (%d MHz)\n", speed / 1000, speed / 1000 / 1000);
#endif

    return ret;
}

int SPI_close(void)
{
    size_t fd = g_SPI_fd;
    /* SPI 是否已经打开 */
    if (fd == 0)
    {
        return 0;
    }
    close(fd);
    g_SPI_fd = 0;
    return 0;
}


#define BLEM_SPI_DATA_LEN 41

#define BLE_MAC_SIZE       6
#define BLE_ADV_DATA_SIZE  31
#define BLE_RESP_DATA_SIZE 31

typedef struct _mble_data_t
{
    uint8_t mac[BLE_MAC_SIZE];
    int8_t rssi;
    uint8_t tx_power;
    uint8_t adv_data[BLE_ADV_DATA_SIZE];
    uint8_t resp_data[BLE_RESP_DATA_SIZE];
    uint64_t rtime;
} mble_data_t;


static mble_data_receive_callback_t mble_data_receive_callback = NULL;

static char SPI_IRQ_GPIO_VALUE[128] = {0};

/*
 * static void pabort(const char *s)
 * {
 *     perror(s);
 *     abort();
 * }
 */

/**
 ****************************************************************
 * @brief 校验和计算
 ****************************************************************
 */
static inline uint16_t checksum(uint8_t *data, uint8_t len)
{
    uint16_t sum = 0;
    for (uint8_t i = 0; i < len; i++)
    {
        sum += *(data + i);
    }
    return sum;
}

void mble_data_init(char *spi_dev, uint16_t irq_gpio_num)
{
#ifdef MBLE_DATA_DEBUG
    printf("%s spidev: %s irq_gpio: %d\r\n", __FUNCTION__, spi_dev, irq_gpio_num);
#endif
    
    SPI_open(spi_dev);

    FILE *fp;
    char filename[80] = {0};

    // 配置GPIO方向
    while (1)
    {
        sprintf(filename, "/sys/class/gpio/gpio%d/direction", irq_gpio_num);
        fp = fopen(filename, "a");
        if (fp != NULL)
        {
            fprintf(fp, "in");
            fclose(fp);
            break;
        }
        else
        {
            fp = fopen("/sys/class/gpio/export", "a");
            if (fp != NULL)
            {
                fprintf(fp, "%d", irq_gpio_num);
                fclose(fp);
            }
            else
            {
                pabort("GPIO export NULL\r\n");
                return;
            }
        }
    }

    sprintf(filename, "/sys/class/gpio/gpio%d/edge", irq_gpio_num);
    fp = fopen(filename, "a");
    if (fp != NULL)
    {
        fprintf(fp, "rising");
        fclose(fp);
    }
    else
    {
        pabort("GPIO edge NULL\r\n");
        return;
    }

    sprintf(SPI_IRQ_GPIO_VALUE, "/sys/class/gpio/gpio%d/value", irq_gpio_num);
}

void log_adv_data(uint8_t *data)
{
    printf("ble_mac: ");
    for (int8_t i = 0; i < BLE_MAC_SIZE; i++)
    {
        printf("%02X", data[i]);
    }
    printf("  ");
    printf("rssi: %u", (uint8_t)data[6]);
    printf("  ");
    printf("tx_power: %d", data[7]);
    printf("  ");
    printf("checksum: %d\n", (data[BLEM_SPI_DATA_LEN - 2] << 8) | data[BLEM_SPI_DATA_LEN - 1]);

    printf("adv_data:  ");
    for (uint8_t i = 0; i < BLE_ADV_DATA_SIZE; i++)
    {
        printf("%02X ", data[8 + i]);
    }
    printf("\n");
}

/**
 ****************************************************************
 * @brief 传入SPI接收到的数据
 *
 * @param[in]  data: 固定长度 BLEM_SPI_DATA_LEN
 ****************************************************************
 */
static inline void parsing_spi_data(void *params, 
        uint8_t *data)
{
#ifdef MBLE_DATA_DEBUG
    log_adv_data(data);
#endif

    // 校验和
    uint16_t sum = checksum(data, BLEM_SPI_DATA_LEN - 2);
    uint16_t recv_sum = (data[BLEM_SPI_DATA_LEN - 2] << 8) | data[BLEM_SPI_DATA_LEN - 1];
    mble_data_t mble_data = {0}; 
    // MBleRawData mble_data = {0};

    if ((sum == recv_sum) 
            && (sum != 0) 
            && (mble_data_receive_callback != NULL) 
            && (params != NULL))
    {
        memcpy((char *)&mble_data, data, BLEM_SPI_DATA_LEN - 2);
        mble_data.rtime = time(NULL);
        mble_data_receive_callback(params, (void *)&mble_data);
    }
#ifdef MBLE_DATA_DEBUG
    else
    {
        printf("sum check error: c: %04x  r: %04x\n", sum, recv_sum);
        for (size_t i = 0; i < BLEM_SPI_DATA_LEN; i++)
        {
            printf("%02X ", data[i]);
        }
        printf("\n");
    }
#endif
}

/**
 ****************************************************************
 * @brief 设置回调函数，接收到新的蓝牙广播数据
 ****************************************************************
 */
void mble_data_receive_callback_set(mble_data_receive_callback_t mcallback)
{
    mble_data_receive_callback = mcallback;
}

/**
 ****************************************************************
 * @brief 接收任务
 ****************************************************************
 */
void *__mble_data_loop_run(void *param)
{
    int fd = open(SPI_IRQ_GPIO_VALUE, O_RDONLY);
    if (fd < 0)
    {
        pabort("open 'SPI_IRQ_GPIO_VALUE' failed!\n");
        return NULL;
    }

    /*
     * if (param != NULL)
     * {
     *     pabort("param error!\n");
     *     return NULL;
     * }
     */

    struct pollfd fds[1];

    fds[0].fd = fd;
    fds[0].events = POLLPRI;

    char gpio_value = 0;

    static uint8_t wdata[BLEM_SPI_DATA_LEN] = {0};
    static uint8_t rdata[BLEM_SPI_DATA_LEN] = {0};

    while (1)
    {
        if (poll(fds, 1, 1000) == -1)
        {
            pabort("poll failed!\n");
            return NULL;
        }

        if (fds[0].revents & POLLPRI)
        {
            if (lseek(fd, 0, SEEK_SET) == -1)
            {
                pabort("lseek failed!\n");
                return NULL;
            }

            if (read(fd, &gpio_value, sizeof(gpio_value)) < 0)
            {
                pabort("read failed!\n");
                return NULL;
            }

            // 上升沿
            if (gpio_value == '1')
            {
                SPI_transfer(wdata, rdata, BLEM_SPI_DATA_LEN);
                parsing_spi_data(param, rdata);
            }
        }
    }
}

void *mble_data_loop_run(void *args)
{
    // pthread_create(&thread_mble_data, NULL, __mble_data_loop_run, NULL);
    __mble_data_loop_run(args);

    return NULL;
}
