#ifndef _TSKA_POSIX_TIMER_H_
#define _TSKA_POSIX_TIMER_H_

#include <signal.h>
#include <time.h>
#include <tska.h>

#if defined (__cplusplus)
extern "C" {
#endif
typedef void (*posixTimerTimeout)(union sigval sig);

int  TSKA_PosixTimerCreate(timer_t *timerId, posixTimerTimeout func,void *prm);
int  TSKA_PosixTimerSetTime(timer_t *timerId,unsigned int timeUSec);
int  TSKA_PosixTimerDelete(timer_t timerId);

#if defined (__cplusplus)
}
#endif



#endif /* _TSKA_BUF_H_ */



