#ifndef __MY_SYSTEM_H__
#define __MY_SYSTEM_H__ 

typedef enum {
    LogMySystemPlatform_None = 0,  //关闭日志输出 
    LogMySystemPlatform_Urgent,    //必须打的
    LogMySystemPlatform_Fatal,     //致使级
    LogMySystemPlatform_Error,     //错误级
    LogMySystemPlatform_Warning,   //告警级
    LogMySystemPlatform_Info,      //业务级
    LogMySystemPlatform_Debug,     //调试级
    LogMySystemPlatform_Trace,     //跟踪级
    LogMySystemPlatform_Detail,    //详细级
    LogMySystemPlatform_Cnt
} LogMySystemPlatform;

typedef int32_t (*MySystemPlatformPrint)(void *, const char *);
typedef int32_t (*processs)(void *oObj, const char *args, char *formats);

int32_t systemArgs(void *oObj, const char *cmd, 
        const char *args, processs myprocess);
#endif /*__MY_SYSTEM_H__*/
