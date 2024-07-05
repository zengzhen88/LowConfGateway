#ifndef __Spi_PROJECT_H__
#define __Spi_PROJECT_H__

#include <common.h>

#define SPI_DATA_LENGTH (64)

typedef enum {
    LogSpi_None = 0,  //关闭日志输出 
    LogSpi_Urgent,    //必须打的
    LogSpi_Fatal,     //致使级
    LogSpi_Error,     //错误级
    LogSpi_Warning,   //告警级
    LogSpi_Info,      //业务级
    LogSpi_Debug,     //调试级
    LogSpi_Trace,     //跟踪级
    LogSpi_Detail,    //详细级
    LogSpi_Cnt
} LogSpi;

typedef int32_t (*SpiPrint)(void *, const char *);
typedef int32_t (*SpiSigSend)(void *oObj,
        DataAttr attr, void *data, 
        int32_t fillLength, int32_t millis);
typedef int32_t (*SpiSigRecv)(void *oObj,
        DataAttr attr, void *data, 
        int32_t *fillLength, int32_t millis);
typedef int32_t (*SpiSigRequest)(void *oObj,
        DataAttr attr, void *data, int32_t fillLength);
typedef int32_t (*SpiSigRelease)(void *oObj,
        DataAttr attr, void *data);

int32_t SpiInitLog(void *priv, SpiPrint print);
int32_t SpiSetLogLevel(LogSpi level);

typedef struct {
    SpiSigSend send;
    SpiSigRecv recv;
    SpiSigRequest request;
    SpiSigRelease release;
} SpiConfig;

void *SpiInit(SpiConfig *config);
int32_t SpiTriggerRecv(void *arg);

#endif /* __Spi_PROJECT_H__ */

