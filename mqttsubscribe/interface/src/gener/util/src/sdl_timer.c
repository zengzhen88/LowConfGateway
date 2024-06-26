/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2020 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <tska_thr.h>
#include <semaphore.h>
#include <pthread.h>
#include "sdl_timer.h"

static void *gPriv = NULL;
static SDLTimerPrint gPrint;
static LogSDLTimer gLevel = LogSDLTimer_Info;

/*************************************************/ 
/**************LogPrintf**************************/
/*************************************************/ 

static int SDLTimerLogPrintf(LogSDLTimer level,
        const char *file, const char *func, 
        int line, const char *format, ...) {
    char logBuf[1024];
    va_list args;
    int funcLine        = 0;

    if (level > gLevel) return -1;

    snprintf (logBuf, sizeof(logBuf), "[%s][%s][%d]", file, func, line);
    funcLine = strlen(logBuf);

    /*va_list*/
    va_start(args, format);
    vsnprintf(&logBuf[funcLine], sizeof(logBuf) - funcLine, format, args);
    va_end(args);
    /*va_list*/

    if (gPrint) {
        return gPrint(gPriv, logBuf);
    }

    return -1;
}

#define LogPrintf(level, ...)\
        SDLTimerLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)     
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogSDLTimer_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogSDLTimer_Debug, __VA_ARGS__);
#endif

#ifndef ERRP
#define ERRP(con, ret, flag, ...)               \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif

/* #define DEBUG_TIMERS */
////////////////////系统API接口封装
/**
 *  Synchronization functions which can time out return this value
 *  if they time out.
 */
#define SDL_MUTEX_TIMEDOUT  1

/**
 *  This is the timeout value which corresponds to never time out.
 */
#define SDL_MUTEX_MAXWAIT   (~(uint32_t)0)
#define  SDL_TRUE (1)

typedef struct SDL_mutex
{
    pthread_mutex_t id;
} SDL_mutex;

typedef struct SDL_semaphore
{
    sem_t sem;
} SDL_semaphore;

typedef struct SDL_semaphore SDL_sem;

//SysTimer
static int32_t ticks_started = 0;
static int32_t has_monotonic_time = 0;
static struct timespec start_ts;
void SDL_TicksInit(void)
{
    if (ticks_started) {
        return;
    }
    ticks_started = SDL_TRUE;

    /* Set first ticks value */
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &start_ts) == 0) {
        has_monotonic_time = 1;
    } 
}

uint64_t SDL_GetTicks(void)
{
    uint64_t ticks;

    if (!ticks_started) {
        SDL_TicksInit();
    }

    if (has_monotonic_time) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC_RAW, &now);
        ticks = (uint64_t)((now.tv_sec - start_ts.tv_sec) * 1000ULL + (now.tv_nsec - start_ts.tv_nsec) / 1000000ULL);
    }

    return (ticks);
}

//////////Mutex
SDL_mutex *SDL_CreateMutex(void)
{
    SDL_mutex *mutex;
    pthread_mutexattr_t attr;

    /* Allocate the structure */
    mutex = (SDL_mutex *) calloc(1, sizeof(*mutex));
    if (mutex) {
        pthread_mutexattr_init(&attr);
        if (pthread_mutex_init(&mutex->id, &attr) != 0) {
            LogPrintf(LogSDLTimer_Error, "pthread_mutext_init failure\n");
            mutex = NULL;
        }
    }
    else {
        LogPrintf(LogSDLTimer_Error, "calloc sizeof(mutex) failure\n");
    }

    return (mutex);
}

void SDL_DestroyMutex(SDL_mutex * mutex)
{
    if (mutex) {
        pthread_mutex_destroy(&mutex->id);
        free(mutex);
    }
}

int SDL_LockMutex(SDL_mutex * mutex)
{
    ERRP (mutex == NULL, return -1, 1, "LockMutex failure\n");

    if (pthread_mutex_lock(&mutex->id) != 0) {
        LogPrintf(LogSDLTimer_Error, "pthread_mutex_lock failure\n");
        return -1;
    }

    return 0;
}

int SDL_TryLockMutex(SDL_mutex * mutex)
{
    int retval;
    int result;

    ERRP (mutex == NULL, return -1, 1, "SDL_TryLockMutex failure\n");

    result = pthread_mutex_trylock(&mutex->id);
    if (result != 0) {
        if (result == EBUSY) {
            retval = 1;
        } else {
            LogPrintf(LogSDLTimer_Error, "pthread_mutex_trylock failure\n");
            retval = -1;
        }
    }

    return retval;
}

int SDL_UnlockMutex(SDL_mutex * mutex)
{
    ERRP (mutex == NULL, return -1, 1, "SDL_LockMutex failure\n");

    if (pthread_mutex_unlock(&mutex->id) != 0) {
        LogPrintf(LogSDLTimer_Error, "pthread_mutex_unlock() failed");
        return -1;
    }

    return 0;
}

//////sem_t
#define SDL_TICKS_PASSED(A, B)  ((int32_t)((B) - (A)) <= 0)
SDL_sem *SDL_CreateSemaphore(uint32_t initial_value)
{
    SDL_sem *sem = (SDL_sem *) malloc(sizeof(SDL_sem));
    if (sem) {
        if (sem_init(&sem->sem, 0, initial_value) < 0) {
            LogPrintf(LogSDLTimer_Error, "sem_init() failed");
            free(sem);
            sem = NULL;
        }
    } 
    else {
        LogPrintf(LogSDLTimer_Error, "malloc sizeof(SDL_sem) failure\n");
    }

    return sem;
}

void SDL_DestroySemaphore(SDL_sem * sem)
{
    if (sem) {
        sem_destroy(&sem->sem);
        free(sem);
    }
}

int SDL_SemTryWait(SDL_sem * sem)
{
    int retval = 1;

    ERRP (!sem, return -1, 1, "Passed a NULL semaphore");

    if (sem_trywait(&sem->sem) == 0) {
        retval = 0;
    }

    return retval;
}

int SDL_SemWait(SDL_sem * sem)
{
    int retval = 1;

    ERRP (!sem, return -1, 1, "Passed a NULL semaphore");

    do {
        retval = sem_wait(&sem->sem);
    } while (retval < 0 && errno == EINTR);

    if (retval < 0) {
        LogPrintf(LogSDLTimer_Error, "sem_wait() failed");
    }
    return retval;
}

void SDL_Delay(uint64_t ms)
{
    int was_error;

    struct timespec elapsed, tv;

    /* Set the timeout interval */
    elapsed.tv_sec = ms / 1000;
    elapsed.tv_nsec = (ms % 1000) * 100000;
    do {
        errno = 0;

        tv.tv_sec = elapsed.tv_sec;
        tv.tv_nsec = elapsed.tv_nsec;
        was_error = nanosleep(&tv, &elapsed);
    } while (was_error && (errno == EINTR));
}

int SDL_SemWaitTimeout(SDL_sem * sem, uint64_t timeout)
{
    int retval = 1;
    uint32_t end;

    ERRP (!sem, return -1, 1, "Passed a NULL semaphore");

    /* Try the easy cases first */
    if (timeout == 0) {
        return SDL_SemTryWait(sem);
    }
    if (timeout == SDL_MUTEX_MAXWAIT) {
        return SDL_SemWait(sem);
    }

    end = SDL_GetTicks() + timeout;
    while ((retval = SDL_SemTryWait(sem)) == SDL_MUTEX_TIMEDOUT) {
        if (SDL_TICKS_PASSED(SDL_GetTicks(), end)) {
            break;
        }
        SDL_Delay(1);
    }

    return retval;
}

uint32_t SDL_SemValue(SDL_sem * sem)
{
    int ret = 0;
    if (sem) {
        sem_getvalue(&sem->sem, &ret);
        if (ret < 0) {
            ret = 0;
        }
    }

    return (uint32_t) ret;
}

int SDL_SemPost(SDL_sem * sem)
{
    int retval = 1;

    ERRP (!sem, return -1, 1, "Passed a NULL semaphore");

    retval = sem_post(&sem->sem);
    if (retval < 0) {
        LogPrintf(LogSDLTimer_Error, "sem_post() failed");
    }

    return retval;
}


////////////////////////////////////////////////



//////SDL_Timer
typedef struct _SDL_Timer
{
    int timerID;
    SDL_TimerCallback callback;
    void *param;
    uint64_t interval;
    uint64_t scheduled;
    int32_t canceled;
    struct _SDL_Timer *next;
} SDL_Timer;

typedef struct _SDL_TimerMap
{
    int timerID;
    SDL_Timer *timer;
    struct _SDL_TimerMap *next;
} SDL_TimerMap;

/* The timers are kept in a sorted list */
typedef struct {
    /* Data used by the main thread */
    /* SDL_Thread *thread; */
    TSKA_ThrHndl thread;
    /* SDL_atomic_t nextID; */
    uint32_t nextID;
    SDL_TimerMap *timermap;
    SDL_mutex *timermap_lock;

    /* Padding to separate cache lines between threads */
    /* char cache_pad[SDL_CACHELINE_SIZE]; */

    /* Data used to communicate with the timer thread */
    /* SDL_SpinLock lock; */
    SDL_mutex *lock;
    SDL_sem *sem;
    SDL_Timer *pending;
    SDL_Timer *freelist;
    /* SDL_atomic_t active; */

    /* List of timers - this is only touched by the timer thread */
    SDL_Timer *timers;
} SDL_TimerData;

static SDL_TimerData SDL_timer_data;
static int32_t running = 0;

/* The idea here is that any thread might add a timer, but a single
 * thread manages the active timer queue, sorted by scheduling time.
 *
 * Timers are removed by simply setting a canceled flag
 */

static void
SDL_AddTimerInternal(SDL_TimerData *data, SDL_Timer *timer)
{
    SDL_Timer *prev, *curr;

    prev = NULL;
    for (curr = data->timers; curr; prev = curr, curr = curr->next) {
        if ((int32_t)(timer->scheduled-curr->scheduled) < 0) {
            break;
        }
    }

    /* Insert the timer here! */
    if (prev) {
        prev->next = timer;
    } else {
        data->timers = timer;
    }
    timer->next = curr;
}

#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

static void *SDL_TimerThread(void *_data)
{
    SDL_TimerData *data = (SDL_TimerData *)_data;
    SDL_Timer *pending;
    SDL_Timer *current;
    SDL_Timer *freelist_head = NULL;
    SDL_Timer *freelist_tail = NULL;
    uint64_t tick, now, interval, delay;
    
    TSKTHRDSETPROCNAME("SDLTimer");

    /* Threaded timer loop:
     *  1. Queue timers added by other threads
     *  2. Handle any timers that should dispatch this cycle
     *  3. Wait until next dispatch time or new timer arrives
     */
    for ( ; ; ) {
        /* Pending and freelist maintenance */
        //检测是否存在添加新的定时器
        SDL_LockMutex(data->lock);
        /* Get any timers ready to be queued */
        pending = data->pending;
        data->pending = NULL;
        SDL_UnlockMutex(data->lock);

        /* Sort the pending timers into our list */
        while (pending) {
            //存在新的定时器节点
            current = pending;
            pending = pending->next;
            //将新的定时器添加到定时器链表节点
            SDL_AddTimerInternal(data, current);
        }
        freelist_head = NULL;
        freelist_tail = NULL;

        /* Initial delay if there are no timers */
        delay = SDL_MUTEX_MAXWAIT;

        //获取当前时间
        tick = SDL_GetTicks();

        /* Process all the pending timers for this tick */
        while (data->timers) {
            current = data->timers;

            //检测定时器定时是否到时
            if ((int32_t)(tick-current->scheduled) < 0) {
                /* Scheduled for the future, wait a bit */
                //未到时，计算需要延时值
                delay = (current->scheduled - tick);
                break;
            }

            //链表头移位到下一个定时节点
            /* We're going to do something with this timer */
            data->timers = current->next;

            //检测当前定时器节点需要删除
            if (!current->canceled) {
                //处理定时器回调
                current->callback(current->interval, current->param);
                //重新设置触发时间点
                current->scheduled  = tick + current->interval;
                //新加入到定时器中
                SDL_AddTimerInternal(data, current);
                /* LogPrintf(LogSDLTimer_Info, "timerId:%lu running interval:%lu\n", current->timerID, current->interval); */
            }
            else {
                //将需要删除的节点临时保存到临时释放链表
                if (!freelist_head) {
                    freelist_head = current;
                }
                if (freelist_tail) {
                    freelist_tail->next = current;
                }
                freelist_tail = current;

                current->canceled = 1;
            }

        }

        /* Adjust the delay based on processing time */
        now = SDL_GetTicks();
        interval = (now - tick);
        if (interval > delay) {
            delay = 0;
        } 
        else {
            delay -= interval;
        }

        /* Note that each time a timer is added, this will return
           immediately, but we process the timers added all at once.
           That's okay, it just means we run through the loop a few
           extra times.
           */
        SDL_SemWaitTimeout(data->sem, delay);
    }

    return 0;
}

int SDL_TimerInit(SDLTimerPrint print, void *priv)
{
    int32_t status      = -1;
    SDL_TimerData *data = &SDL_timer_data;

    if (NULL == print) {
        printf ("SDL_TimerInit print:%p priv:%p\n", print, priv);
        return -1;
    }

    gPrint  = print;
    gPriv   = priv;
    gLevel  = LogSDLTimer_Info;

    /* if (!SDL_AtomicGet(&data->active)) { */
    if (!running) {
        /* const char *name = "SDLTimer"; */
        data->timermap_lock = SDL_CreateMutex();
        ERRP(NULL == data->timermap_lock, return -1, 1, "SDL_CreateMutex timermap_lock failure\n");

        data->lock = SDL_CreateMutex();
        ERRP(NULL == data->lock, goto ERR0, 1, "SDL_CreateMutex lock failure\n");

        data->sem = SDL_CreateSemaphore(0);
        ERRP(NULL == data->sem, goto ERR1, 1, "SDL_CreateSemaphore sem failure\n");

        /* SDL_AtomicSet(&data->active, 1); */
        running = 1;

        /* Timer threads use a callback into the app, so we can't set a limited stack size here. */
        /* data->thread = SDL_CreateThreadInternal(SDL_TimerThread, name, 0, data); */
        status = TSKA_thrCreate(&data->thread, SDL_TimerThread, 99, 0, data);
        ERRP(0 != status, goto ERR2, 1, "TSKA_thrCreate data->thread failure\n");

        /* SDL_AtomicSet(&data->nextID, 1); */
        data->nextID = 1;
    }

    return 0;
ERR2:
    SDL_DestroySemaphore(data->sem);
ERR1:
    if (data->lock)
        SDL_DestroyMutex(data->lock);
ERR0:
    if(data->timermap_lock)
        SDL_DestroyMutex(data->timermap_lock);

    return -1;
}

void
SDL_TimerQuit(void)
{
    SDL_TimerData *data = &SDL_timer_data;
    SDL_Timer *timer;
    SDL_TimerMap *entry;

    if (running) {  /* active? Move to inactive. */
        /* Shutdown the timer thread */
        SDL_SemPost(data->sem);
        TSKA_thrDelete(&data->thread);
        /* SDL_WaitThread(data->thread, NULL); */
        /* data->thread = NULL; */

        SDL_DestroySemaphore(data->sem);
        data->sem = NULL;

        /* Clean up the timer entries */
        while (data->timers) {
            timer = data->timers;
            data->timers = timer->next;
            free(timer);
        }

        while (data->freelist) {
            timer = data->freelist;
            data->freelist = timer->next;
            free(timer);
        }

        while (data->timermap) {
            entry = data->timermap;
            data->timermap = entry->next;
            free(entry);
        }

        SDL_DestroyMutex(data->lock);
        SDL_DestroyMutex(data->timermap_lock);
        data->timermap_lock = NULL;
        data->lock = NULL;
    }
}

uint64_t SDL_AddTimer(uint64_t interval, SDL_TimerCallback callback, void *param)
{
    SDL_TimerData *data = &SDL_timer_data;
    SDL_Timer *timer;
    SDL_TimerMap *entry;

    SDL_LockMutex(data->lock);
    //检测释放链表头
    timer = data->freelist;
    if (timer) {
        //检测释放链表头有效,从中取一个节点重复使用
        data->freelist = timer->next;
    }
    SDL_UnlockMutex(data->lock);

    if (timer) {
        //删除该定时器ID，同时准备重复使用该节点
        SDL_RemoveTimer(timer->timerID);
    } 
    else {
        timer = (SDL_Timer *)malloc(sizeof(*timer));
        ERRP(NULL == timer, return -1, 1, "malloc sizeof(timer) failure\n");
    }

    timer->timerID      = data->nextID++;
    timer->callback     = callback;
    timer->param        = param;
    timer->interval     = interval;
    timer->scheduled    = SDL_GetTicks() + interval;
    timer->canceled     = 0;

    LogPrintf(LogSDLTimer_Info, "TimeId:%u interval:%lu\n", timer->timerID, timer->interval);

    entry = (SDL_TimerMap *)malloc(sizeof(*entry));
    if (!entry) {
        free(timer);
        LogPrintf(LogSDLTimer_Error, "malloc sizeof(entry) failure\n");
        return -1;
    }
    entry->timer        = timer;
    entry->timerID      = timer->timerID;

    SDL_LockMutex(data->timermap_lock);
    entry->next         = data->timermap;
    data->timermap      = entry;
    SDL_UnlockMutex(data->timermap_lock);

    /* Add the timer to the pending list for the timer thread */
    SDL_LockMutex(data->lock);
    timer->next = data->pending;
    data->pending = timer;
    SDL_UnlockMutex(data->lock);

    /* Wake up the timer thread if necessary */
    SDL_SemPost(data->sem);

    return entry->timerID;
}

int32_t SDL_RemoveTimer(uint32_t id)
{
    SDL_TimerData *data = &SDL_timer_data;
    SDL_TimerMap *prev, *entry;

    /* Find the timer */
    SDL_LockMutex(data->timermap_lock);
    prev = NULL;
    for (entry = data->timermap; entry; prev = entry, entry = entry->next) {
        if (entry->timerID == id) {
            if (prev) {
                prev->next = entry->next;
            } else {
                data->timermap = entry->next;
            }
            break;
        }
    }
    SDL_UnlockMutex(data->timermap_lock);

    if (entry) {
        entry->timer->canceled = 1;
        free(entry);
    }

    return 0;
}

/* vi: set ts=4 sw=4 expandtab: */
