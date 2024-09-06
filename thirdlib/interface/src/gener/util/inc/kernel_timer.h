/*****************************************************************************
             Copyright (c) iFreecomm, Inc. All rights reserved.
 > Module: timer.h @ Common
 > Author: Forrest Yuan
 > Created Time: 2017��5��8��
 > Instruction: ��ʱ���ӿ�ͷ�ļ�
 > Modify History:
 *****************************************************************************/
#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef __cplusplus
extern "C"{
#endif /* _cplusplus */


typedef enum {
    LogKernelTimer_None = 0,  //�ر���־��� 
    LogKernelTimer_Urgent,    //������
    LogKernelTimer_Fatal,     //��ʹ��
    LogKernelTimer_Error,     //����
    LogKernelTimer_Warning,   //�澯��
    LogKernelTimer_Info,      //ҵ��
    LogKernelTimer_Debug,     //���Լ�
    LogKernelTimer_Trace,     //���ټ�
    LogKernelTimer_Detail,    //��ϸ��
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
