/*****************************************************************************
             Copyright (c) iFreecomm, Inc. All rights reserved.
 > Module: timer.h @ Common
 > Author: Forrest Yuan
 > Created Time: 2017年5月8日
 > Instruction: 定时器接口头文件
 > Modify History:
 *****************************************************************************/
#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef __cplusplus
extern "C"{
#endif /* _cplusplus */


typedef enum {
    LogKernelTimer_None = 0,  //关闭日志输出 
    LogKernelTimer_Urgent,    //必须打的
    LogKernelTimer_Fatal,     //致使级
    LogKernelTimer_Error,     //错误级
    LogKernelTimer_Warning,   //告警级
    LogKernelTimer_Info,      //业务级
    LogKernelTimer_Debug,     //调试级
    LogKernelTimer_Trace,     //跟踪级
    LogKernelTimer_Detail,    //详细级
    LogKernelTimer_Cnt
} LogKernelTimer;

typedef int (*KernelTimerPrint)(void *, const char *);
typedef int (*functionCb)(void *oObj);


// void init_timer(struct timer_list *timer);
int Kernel_TimerInit(KernelTimerPrint print, void *priv);
void Kernel_TimerOut(void);
int Kernel_AddTimer(unsigned int interval, void *oObj, functionCb TimerCallBackFn);
int kernel_RemoveTimer(unsigned int timerID);

#ifdef __cplusplus
}
#endif /* _cplusplus */

#endif
