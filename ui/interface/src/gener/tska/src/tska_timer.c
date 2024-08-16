#include <tska_timer.h>
#include <string.h>

int32_t TSKA_PosixTimerCreate(timer_t *timerId, posixTimerTimeout func, void *prm)
{  
	struct sigevent sev;  

    memset(&sev, 0x0, sizeof(struct sigevent));
  
	sev.sigev_notify 	        = SIGEV_THREAD;  
	sev.sigev_signo 	        = SIGRTMIN;  
	sev.sigev_value.sival_ptr 	= prm;  
	sev.sigev_notify_function 	= func;  
	sev.sigev_notify_attributes = NULL;  
  
	/* create timer */	
    fprintf (stderr, "[%s:%s:%d]timer_create start.\n", __FILE__, __FUNCTION__, __LINE__);
	if (timer_create (CLOCK_REALTIME, &sev, timerId) == -1)  
	{  
		fprintf (stderr, "[%s:%s:%d]timer_create, error\n", __FILE__, __FUNCTION__, __LINE__);  
		return -1;  
	}  
  
	fprintf(stderr, "[%s:%s:%d]timer_create end.\n", __FILE__, __FUNCTION__, __LINE__);

	return 0;
}  
 
int32_t TSKA_PosixTimerSetTime(timer_t *timerId, uint32_t timeUSec)
{
	struct itimerspec its;	

	if (timerId == NULL) {
        fprintf(stderr, "[%s:%s:%d]timer_settime input param timerId(NULL) error!!\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
   }

	/* Start the timer */  
	its.it_value.tv_sec         = timeUSec / 1000000; 
	its.it_value.tv_nsec        = (timeUSec % 1000000) * 1000;  

	its.it_interval.tv_sec      = timeUSec / 1000000; 
	its.it_interval.tv_nsec     = (timeUSec % 1000000) * 1000;  

	if (timer_settime (*timerId, 0/* TIMER_ABSTIME */, &its, NULL) == -1)	
	{  
		fprintf (stderr, "[%s:%s:%d]timer_settime error\n", __FILE__, __FUNCTION__, __LINE__);  
	}  

	fprintf (stderr, "[%s:%s:%d]call timer_settime reset timer done.\n", __FILE__, __FUNCTION__, __LINE__);  

	return 0;
}

int32_t  TSKA_PosixTimerDelete(timer_t timerId)
{
	timer_delete(timerId);  

	return 0;
}


