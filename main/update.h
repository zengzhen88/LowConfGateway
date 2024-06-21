#ifndef __Update_PROJECT_H__
#define __Update_PROJECT_H__

#include <common.h>

typedef enum {
    LogUpdate_None = 0,  //关闭日志输出 
    LogUpdate_Urgent,    //必须打的
    LogUpdate_Fatal,     //致使级
    LogUpdate_Error,     //错误级
    LogUpdate_Warning,   //告警级
    LogUpdate_Info,      //业务级
    LogUpdate_Debug,     //调试级
    LogUpdate_Trace,     //跟踪级
    LogUpdate_Detail,    //详细级
    LogUpdate_Cnt
} LogUpdate;

typedef int32_t (*UpdatePrint)(void *, const char *);
typedef int32_t (*UpdateSigSend)(void *oObj,
        DataAttr attr, void *data, 
        int32_t fillLength, int32_t millis);
typedef int32_t (*UpdateSigRecv)(void *oObj,
        DataAttr attr, void *data,
        int32_t *fillLength, int32_t millis);

int32_t UpdateInitLog(void *priv, UpdatePrint print);
int32_t UpdateSetLogLevel(LogUpdate level);

typedef struct {
    UpdateSigSend send;
    UpdateSigRecv recv;
} UpdateConfig;

void *UpdateInit(UpdateConfig *config);

#endif /* __Update_PROJECT_H__ */
