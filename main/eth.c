#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif_types.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/ip_addr.h"
#include "esp_netif.h"

#include <eth.h>
#include <message.h>
#include "esp_timer.h"

static void *gPriv = NULL;
static EthPrint gPrint;
static LogEth gLevel = LogEth_Info;


/*************************************************/
/**************LogPrintf**************************/
/*************************************************/



static int EthLogPrintf(LogEth level,
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
    EthLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogEth_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogEth_Debug, __VA_ARGS__);
#endif

#ifndef ERRP
#define ERRP(con, ret, flag, ...)               \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif

#define ETH_UPDATE_USERPASSWORD BIT0

typedef struct {
    int32_t isRunning;
    int32_t retryNum;
    int32_t ethSok;
    esp_event_loop_handle_t event;
    esp_event_handler_instance_t instanceAnyId;
    esp_event_handler_instance_t instanceGotIp;
    esp_event_handler_instance_t instanceGotIpV6;
    esp_eth_handle_t ethHandle;
    esp_netif_t *ethif;
    esp_eth_netif_glue_handle_t ethNetifGlues;
    ModuleMessage message; //保存配置

    esp_timer_handle_t timer;
    /* TaskHandle_t ethTask; */

    EthSigSend send;
    EthSigRecv recv;
} Eth;

//设置日志输出对象，主要看你想输出到哪里，就对应填充回调
int32_t EthInitLog(void *priv, EthPrint print) {
    gPriv   = priv;
    gPrint  = print;

    return 0;
}

//设置日志输出等级，看需要，调试、错误、宕机等
int32_t EthSetLogLevel(LogEth level) {
    gLevel  = level;

    return 0;
}

int32_t EthEventRecvHandler(Eth *net, ModuleMessage *message) {
    switch (message->attr) {
        case ModuleDataAttr_TriggerRecv:
            {
                if (net->recv) {
                    ModuleMessage message;
                    int32_t length = sizeof(message);
                    int status = net->recv(gPriv, DataAttr_MqttToEth, &message, &length, 0);
                    if (!status) {
                        LogPrintf(LogEth_Info, "start test recv getEthConfig\n");
                        /*主要是设置Eth信号*/
                        switch (message.attr) {
/*
 *                             case ModuleDataAttr_GetEthConfig:
 *                                 {
 *                                     if (net->send) {
 *                                         LogPrintf(LogEth_Info, "start test ack getEthConfig\n");
 *                                         net->send(gPriv, DataAttr_EthToMqtt, 
 *                                                 &net->message, sizeof(net->message), 0);
 *                                     }
 *                                     break;
 *                                 }
 *                             case ModuleDataAttr_SetEthConfig:
 *                                 {
 *                                     if (0 == ((uint8_t *)message.ethConfig.ip)[0]
 *                                             && 0 == ((int8_t *)message.ethConfig.ip)[1]
 *                                             && 0 == ((int8_t *)message.ethConfig.ip)[2]
 *                                             && 0 == ((int8_t *)message.ethConfig.ip)[3]) {
 *                                         if (ESP_OK == esp_netif_dhcps_stop(net->ethif)) {
 *                                             char ip[32];
 *                                             esp_netif_ip_info_t info;
 *                                             memset(&info, 0x0, sizeof(info));
 *                                             IpUInt32ToString(message.ethConfig.ip, ip, sizeof(ip));
 *                                             ipaddr_aton((const char *)ip, (ip_addr_t *)&info.ip);
 *                                             IpUInt32ToString(message.ethConfig.netmask, ip, sizeof(ip));
 *                                             ipaddr_aton((const char *)ip, (ip_addr_t *)&info.netmask);
 *                                             IpUInt32ToString(message.ethConfig.gateway, ip, sizeof(ip));
 *                                             ipaddr_aton((const char *)ip, (ip_addr_t *)&info.gw);
 *                                             esp_netif_set_ip_info(net->ethif, &info);
 *                                         }
 *                                         else {
 *                                             LogPrintf(LogEth_Error, "eth esp_netif_dhcps_stop failure\n");
 *                                         }
 *                                     }
 *                                     else {
 *                                         if (ESP_OK != esp_netif_dhcps_start(net->ethif)) {
 *                                             LogPrintf(LogEth_Error, "eth esp_netif_dhcps_start failure\n");
 *                                         }
 *                                     }
 * 
 *                                     break;
 *                                 }
 */
                            default:break;
                        }
                    }
                }
                break;
            }
        default:break;
    }

    return 0;
}

typedef enum {
    MYETHERNET_EVENT_START,
} myeth_event_t;
ESP_EVENT_DEFINE_BASE(MYETH_EVENT);
#define MYETH_EVENT_ETH_ANY_ID (-1)

#define EXAMPLE_ESP_MAXIMUM_RETRY  5
void EthEventHandler(void* arg, esp_event_base_t event_base,
        int32_t event_id, void *event_data) {
    Eth *eth = (Eth *)arg;

    LogPrintf(LogEth_Info, "eventBase:%s eventId:%d\n", 
            event_base, event_id);

    if (event_base == ETH_EVENT) {
        switch (event_id) {
            case ETHERNET_EVENT_CONNECTED:
                {
    /* we can get the ethernet driver handle from event data */
                    uint8_t macAddr[6] = {0};
                    esp_eth_handle_t ethHandle = *(esp_eth_handle_t *)event_data;
                    esp_eth_ioctl(ethHandle, ETH_CMD_G_MAC_ADDR, macAddr);
                    LogPrintf(LogEth_Info, "Ethernet Link Up");
                    LogPrintf(LogEth_Info, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                            macAddr[0], macAddr[1], macAddr[2],
                            macAddr[3], macAddr[4], macAddr[5]);
                    break;
                }
            case ETHERNET_EVENT_DISCONNECTED:
                {
                    eth->ethSok = 0;
                    LogPrintf(LogEth_Error ,"Ethernet Link Down!\n");
                    break;
                }
            case ETHERNET_EVENT_START:
                {
                    LogPrintf(LogEth_Info, "Ethernet Started");
                    break;
                }
            case ETHERNET_EVENT_STOP:
                {
                    LogPrintf(LogEth_Info, "Ethernet Stopped");
                    break;
                }
            case ETHERNET_EVENT_MAX:
                {
                    EthEventRecvHandler(eth, (ModuleMessage *)event_data);
                    break;
                }
            default:break;
        }
    }
    else if (event_base == MYETH_EVENT) {
        switch (event_id) {
            case MYETHERNET_EVENT_START:
                {
                    EthEventRecvHandler(eth, (ModuleMessage *)event_data);
                    break;
                }
            default:break;
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        eth->ethSok = 1;
        LogPrintf(LogEth_Info, "got ip:" IPSTR "\n", IP2STR(&event->ip_info.ip));
        if (eth->send) {
            //report
            /*
             * eth->message.ethConfig.attr      = ModuleDataAttr_GetEthConfig;
             * eth->message.ethConfig.ip        = event->ip_info.ip.addr;
             * eth->message.ethConfig.netmask   = event->ip_info.netmask.addr;
             * eth->message.ethConfig.gateway   = event->ip_info.gw.addr;
             */
            /* eth->send(gPriv, DataAttr_Eth, &eth->message, 40); */
        }
        eth->retryNum = 0;
    }
    //暂不考虑IPV6
}

/*
 * static void timer_cb(void *arg) {
 *     ModuleMessage message;
 *     message.attr = ModuleDataAttr_TriggerRecv;
 *     esp_event_post(ETH_EVENT, ETHERNET_EVENT_MAX, &message, sizeof(message), 0);
 * }
 */

int32_t EthTriggerRecv(void *arg) {
    ModuleMessage message;
    message.attr = ModuleDataAttr_TriggerRecv;
    esp_event_post(ETH_EVENT, ETHERNET_EVENT_MAX, &message, sizeof(message), 0);

    return 0;
}

void *EthInit(EthConfig *config) {
    esp_err_t status                = ESP_FAIL;        
    /* esp_eth_handle_t ret = NULL; */
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();

    // Init common MAC and PHY configs to default
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

    Eth *eth = (Eth *) malloc (sizeof(*eth));
    ERRP(NULL == eth, return NULL, 1, "malloc Eth Instance failure\n");
    memset(eth, 0x0, sizeof(*eth));


    esp_log_level_set("esp.emac", ESP_LOG_VERBOSE);
    esp_log_level_set("esp_eth", ESP_LOG_VERBOSE);
    esp_log_level_set("eth_phy_802_3", ESP_LOG_VERBOSE);


    eth->send  = config->send;
    eth->recv  = config->recv;

    // Update PHY config based on board specific configuration
    phy_config.phy_addr = 2;//1;//CONFIG_EXAMPLE_ETH_PHY_ADDR;
    phy_config.reset_gpio_num = 5;//CONFIG_EXAMPLE_ETH_PHY_RST_GPIO;
    // Init vendor specific MAC config to default
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    // Update vendor specific MAC config based on board configuration
    esp32_emac_config.smi_gpio.mdc_num = 23;//CONFIG_EXAMPLE_ETH_MDC_GPIO;
    esp32_emac_config.smi_gpio.mdio_num = 18;//CONFIG_EXAMPLE_ETH_MDIO_GPIO;

    printf ("%s %d\n", __func__, __LINE__);
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);
    ERRP(NULL == mac, goto ERR0, 1, "eth esp_eth_mac_new_esp32 failure\n");
    printf ("%s %d\n", __func__, __LINE__);

    // Create new PHY instance based on board configuration
    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);
    ERRP(NULL == phy, goto ERR1, 1, "eth esp_eth_phy_new_lan87xx failure\n");
    printf ("%s %d\n", __func__, __LINE__);

    esp_eth_config_t ethConfig = ETH_DEFAULT_CONFIG(mac, phy);

    printf ("%s %d\n", __func__, __LINE__);
    status = esp_eth_driver_install(&ethConfig, &eth->ethHandle);
    ERRP(ESP_OK != status, goto ERR2, 1, "eth Ethernet driver install failed\n");
    printf ("%s %d\n", __func__, __LINE__);

    // Initialize TCP/IP network interface
    status = esp_netif_init();
    ERRP(ESP_OK != status, goto ERR3, 1, "eth esp_netif_init failure\n");
    printf ("%s %d\n", __func__, __LINE__);

    // Create default event loop that running in background
    /*
     * status = esp_event_loop_create_default();
     * ERRP(ESP_OK != status, 
     *         goto ERR4, 1, "eth esp_event_loop_create_default failure\n");
     */
    esp_event_loop_args_t loop_args = {
        .queue_size = CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE,
        .task_name = "update",
        .task_stack_size = ESP_TASKD_EVENT_STACK,
        .task_priority = ESP_TASKD_EVENT_PRIO,
        .task_core_id = 0
    };
    status = esp_event_loop_create(&loop_args, &eth->event);
    ERRP(ESP_OK != status, goto ERR000, 1, 
            "update esp_event_loop_create failure\n");
    printf ("%s %d\n", __func__, __LINE__);

    eth->ethif = esp_netif_new(&cfg);
    ERRP(NULL != eth->ethif, goto ERR5, 1, "eth esp_netif_new failure\n");
    printf ("%s %d\n", __func__, __LINE__);

    eth->ethNetifGlues = esp_eth_new_netif_glue(eth->ethHandle);
    ERRP(NULL == eth->ethNetifGlues, goto ERR6, 1, "eth esp_eth_new_netif_glue failure\n");
    printf ("%s %d\n", __func__, __LINE__);

    // Attach Ethernet driver to TCP/IP stack
    status = esp_netif_attach(eth->ethif, eth->ethNetifGlues);
    ERRP(ESP_OK != status, goto ERR7, 1, "eth esp_netif_attach failure\n");

    esp_event_handler_register_with(eth->event, ETH_EVENT, ESP_EVENT_ANY_ID, EthEventHandler, eth);
    esp_event_handler_register_with(eth->event, IP_EVENT, IP_EVENT_ETH_GOT_IP, EthEventHandler, eth);
    esp_event_handler_register_with(eth->event, MYETH_EVENT, MYETH_EVENT_ETH_ANY_ID, EthEventHandler, eth);

    status = esp_eth_start(eth->ethHandle);

    /*
     * const esp_timer_create_args_t timer_args = {
     *     timer_cb,
     *     eth,
     *     ESP_TIMER_TASK,
     *     "eth_timer",
     *     true,
     * };
     * esp_timer_create(&timer_args, &eth->timer);
     * esp_timer_start_periodic(eth->timer, 1000000);//10ms
     */

    while (!eth->ethSok) {
        vTaskDelay(pdMS_TO_TICKS(200));
        LogPrintf(LogEth_Info, "eth->ethSok:%d\n", eth->ethSok);
    }
    LogPrintf(LogEth_Info, "<<<eth->ethSok:%d>>>\n", eth->ethSok);

    return eth;
ERR7:
ERR6:
ERR5:
ERR3:
ERR2:
ERR1:
ERR0:
ERR000:
    free(eth);

    return NULL;
}


