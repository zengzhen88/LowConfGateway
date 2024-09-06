
#ifndef __SDL_TIMERCTRL_H__
#define __SDL_TIMERCTRL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LogSDLTimer_None = 0,  //关闭日志输出 
    LogSDLTimer_Urgent,    //必须打的
    LogSDLTimer_Fatal,     //致使级
    LogSDLTimer_Error,     //错误级
    LogSDLTimer_Warning,   //告警级
    LogSDLTimer_Info,      //业务级
    LogSDLTimer_Debug,     //调试级
    LogSDLTimer_Trace,     //跟踪级
    LogSDLTimer_Detail,    //详细级
    LogSDLTimer_Cnt
} LogSDLTimer;

typedef int (*SDLTimerPrint)(void *, const char *);
typedef void (*SDL_TimerCallback) (uint64_t interval, void *param);


int32_t SDL_RemoveTimer(uint32_t id);
uint64_t SDL_AddTimer(uint64_t interval, SDL_TimerCallback callback, void *param);
void SDL_TimerQuit(void);
int SDL_TimerInit(SDLTimerPrint print, void *priv);

/* 音量控制打印等级调节接口
 * 参数1：音量控制句柄
 * 参数2：音量控制打印的最低基本
 * 返回值：0
*/
int VolCtrlLogSDLTimerevel(void *pObj, LogSDLTimer level);

#ifdef __cplusplus
}
#endif

#endif /*__SDL_TIMER_H__*/
