#if 0 ////20200604
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#endif

#include <sys/time.h>
#include <tska.h>


uint64_t TSKA_getCurTimeInNsec()
{
    static int32_t isInit = 0;
    static uint64_t myInitTime=0;
    struct timespec tv;


    if(isInit==0)
    {
        isInit = 1;

        if (clock_gettime(CLOCK_REALTIME, &tv) < 0)
            return 0;

        myInitTime = (uint64_t)(tv.tv_sec * 1000000000ULL + tv.tv_nsec*1ULL);
    }

    if (clock_gettime(CLOCK_REALTIME, &tv) < 0)
        return 0;

    return (uint64_t)(tv.tv_sec * 1000000000ULL + tv.tv_nsec*1ULL)-myInitTime;
}


uint64_t TSKA_getActualTimeInUsec()
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) < 0)
        return 0;

    return (uint64_t)(tv.tv_sec * 1000000ULL + tv.tv_usec*1ULL);
}

uint64_t TSKA_getCurTimeInUsec()
{
    static int32_t isInit = 0;
    static uint64_t myInitTime=0;
    struct timeval tv;

    if(isInit==0)
    {
        isInit = 1;

        if (gettimeofday(&tv, NULL) < 0)
            return 0;

      myInitTime = (uint64_t)(tv.tv_sec * 1000000ULL + tv.tv_usec*1ULL);
  }

    if (gettimeofday(&tv, NULL) < 0)
        return 0;

  return (uint64_t)(tv.tv_sec * 1000000ULL + tv.tv_usec*1ULL)-myInitTime;
}
uint32_t TSKA_getCurTimeInMsec()
{
    static int32_t isInit = 0;
    static uint32_t initTime=0;
    struct timeval tv;

    if(isInit==0)
    {
        isInit = 1;

        if (gettimeofday(&tv, NULL) < 0)
            return 0;

        initTime = (uint32_t)(tv.tv_sec * 1000u + tv.tv_usec/1000u);
  }

    if (gettimeofday(&tv, NULL) < 0)
        return 0;

    return (uint32_t)(tv.tv_sec * 1000u + tv.tv_usec/1000u)-initTime;
}

void TSKA_waitUsecs(uint64_t usecs)
{
    struct timespec delayTime, remainTime;
    int32_t ret;

    delayTime.tv_sec  = usecs/1000000;
    delayTime.tv_nsec = (usecs%1000000)*1000;

    do
    {
        ret = nanosleep(&delayTime, &remainTime);
        if(ret < 0 && remainTime.tv_sec > 0 && remainTime.tv_nsec > 0)
        {
            /* restart for remaining time */
            delayTime = remainTime;
        }
        else
        {
            break;
        }
    } while(1);
}

void TSKA_waitMsecs(uint32_t msecs)
{
    struct timespec delayTime, remainTime;
    int32_t ret;

    delayTime.tv_sec  = msecs/1000;
    delayTime.tv_nsec = (msecs%1000)*1000000;

    do
    {
        ret = nanosleep(&delayTime, &remainTime);
        if(ret < 0 && remainTime.tv_sec > 0 && remainTime.tv_nsec > 0)
        {
            /* restart for remaining time */
            delayTime = remainTime;
        }
        else
        {
            break;
        }
    } while(1);
}

static char xtods(char c) {
    if (c>='0' && c<='9') return c-'0';
    if (c>='A' && c<='F') return c-'A'+10;
    if (c>='a' && c<='f') return c-'a'+10;
    return c=0;        // not Hex digit
}

static int32_t HextoDecs(int8_t *hex, int32_t l)
{
    if (*hex==0)
        return(l);

    return HextoDecs(hex+1, l*16+xtods(*hex)); // hex+1?
}

int32_t xstrtois(int8_t *hex)      // hex string to integer
{
    return HextoDecs(hex,0);
}

