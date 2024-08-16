#ifndef __UI_PROJECT_H__
#define __UI_PROJECT_H__

typedef enum {
    LogUI_None = 0,  //关闭日志输出 
    LogUI_Urgent,    //必须打的
    LogUI_Fatal,     //致使级
    LogUI_Error,     //错误级
    LogUI_Warning,   //告警级
    LogUI_Info,      //业务级
    LogUI_Debug,     //调试级
    LogUI_Trace,     //跟踪级
    LogUI_Detail,    //详细级
    LogUI_Cnt
} LogUI;

typedef int32_t (*UIPrint)(void *, const char *);

int32_t UIInitLog(void *priv, UIPrint print);
int32_t UISetLogLevel(LogUI level);

void *UIInit(void);

#endif /* __UI_PROJECT_H__ */

