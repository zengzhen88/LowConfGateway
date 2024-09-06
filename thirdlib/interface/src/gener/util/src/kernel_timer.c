/*****************************************************************************
             Copyright (c) iFreecomm, Inc. All rights reserved.
 > Module: timer.c
 > Author: Forrest Yuan
 > Created Time: 2017年5月8日
 > Instruction: 定时器接口
 > Modify History:
 *****************************************************************************/
#define USE_KERNEL_TIMER
#ifdef USE_KERNEL_TIMER
#include <stdint.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <tska_thr.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <rbtree.h>

#include "listt.h"
#include "tska_mutex.h"
#include "kernel_timer.h"

static void *gPriv = NULL;
static KernelTimerPrint gPrint;
static LogKernelTimer gLevel = LogKernelTimer_Info;
static uint32_t timerID = 0;

/*************************************************/ 
/**************LogPrintf**************************/
/*************************************************/ 

static int KernelTimerLogPrintf(LogKernelTimer level,
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
        KernelTimerLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)     
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogKernelTimer_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogKernelTimer_Debug, __VA_ARGS__);
#endif

#ifndef ERRP
#define ERRP(con, ret, flag, ...)               \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif

#ifdef __cplusplus
extern "C"{
#endif /* _cplusplus */

//typedef char  int;

#define jiffies    raid6_jiffies()

#define TIMER_NOT_PINNED    0

#undef  HZ
#define HZ 1000
/*static inline unsigned long raid6_jiffies(void)
  {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec*1000 + tv.tv_usec/1000;
  }*/
static inline unsigned long raid6_jiffies(void)
{
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return tv.tv_sec*1000 + tv.tv_nsec/1000000;
}


#define TASK_COMM_LEN 16

/*
 * per-CPU timer vector definitions:
 */
#define CONFIG_BASE_SMALL 0

#define TVN_BITS (CONFIG_BASE_SMALL ? 4 : 6)
#define TVR_BITS (CONFIG_BASE_SMALL ? 6 : 8)
#define TVN_SIZE (1 << TVN_BITS)
#define TVR_SIZE (1 << TVR_BITS)
#define TVN_MASK (TVN_SIZE - 1)
#define TVR_MASK (TVR_SIZE - 1)
#define MAX_TVAL ((unsigned long)((1ULL << (TVR_BITS + 4*TVN_BITS)) - 1))

#define TIMER_DEFERRABLE    0x00100000

#define TIMER_ENTRY_STATIC  ((void *) 0x74737461)

#define typecheck(type,x) \
    ({  type __dummy; \
     typeof(x) __dummy2; \
     (void)(&__dummy == &__dummy2); \
     1; \
     })


#define raw_smp_processor_id()          0

#define smp_processor_id() raw_smp_processor_id()


#define time_after(a,b)     \
    (typecheck(unsigned long, a) && \
     typecheck(unsigned long, b) && \
     ((long)((b) - (a)) < 0))
#define time_before(a,b)    time_after(b,a)

#define time_after_eq(a,b)  \
    (typecheck(unsigned long, a) && \
     typecheck(unsigned long, b) && \
     ((long)((a) - (b)) >= 0))
#define time_before_eq(a,b) time_after_eq(b,a)

/*
 * Calculate whether a is in the range of [b, c].
 */
#define time_in_range(a,b,c) \
    (time_after_eq(a,b) && \
     time_before_eq(a,c))

/*
 * Calculate whether a is in the range of [b, c).
 */
#define time_in_range_open(a,b,c) \
    (time_after_eq(a,b) && \
     time_before(a,c))

#ifdef CONFIG_ILLEGAL_POINTER_VALUE
# define POISON_POINTER_DELTA _AC(CONFIG_ILLEGAL_POINTER_VALUE, UL)
#else
# define POISON_POINTER_DELTA 0
#endif
#define LIST_POISON2  ((void *) 0x200 + POISON_POINTER_DELTA)

//typedef char                  bool;

struct tvec_base;

struct timer_list {
    /*
     * All fields that change during normal runtime grouped to the
     * same cacheline
     */
    struct list_head_     entry;
    unsigned long        expires;    //毫秒 触发定时的实际时间
    unsigned long        prevExpires;    //毫秒 上一次触发定时的实际时间
    functionCb           function;
    unsigned long        data;
    unsigned int         flags;
    // unsigned int         aliveFlag;
    // int                  runFlag;
    // unsigned int         context;
    unsigned int         intervalTime;  //定时周期
    unsigned int         timerID;

    //#ifdef CONFIG_TIMER_STATS
    // unsigned int         index;
    // unsigned int         event;
    // char                 start_comm[16];
    //#endif
};

struct tvec {
    struct list_head_ vec[TVN_SIZE];
};

struct tvec_root {
    struct list_head_ vec[TVR_SIZE];
};

struct tvec_base {
    struct timer_list *running_timer;
    unsigned long timer_jiffies;
    unsigned long next_timer;
    unsigned long active_timers;
    struct tvec_root tv1;
    struct tvec tv2;
    struct tvec tv3;
    struct tvec tv4;
    struct tvec tv5;
};

#define true  1
#define false 0

static struct tvec_base boot_tvec_bases;
static TSKA_MutexHndl g_timer_lock;
static TSKA_ThrHndl g_thread;
static struct rb_roots gRoots = RB_ROOTS;

static void run_timer_softirq(void);
static int  timer_pending(const struct timer_list * timer);
static void detach_timer(struct timer_list *timer, int clear_pending);
static int  mod_timer(struct timer_list *timer, unsigned long expires);

static void init_timer(struct timer_list *timer)
{
    timer->entry.prev = NULL;
    timer->flags = raw_smp_processor_id();
    /* timer->runFlag = 0; */
    /* timer->aliveFlag = 0; */
    //#ifdef CONFIG_TIMER_STATS
    /* timer->event = 0; */
    /* timer->index = 0; */
    /* memset(timer->start_comm, 0, TASK_COMM_LEN); */
    //#endif
}

static int timer_pending(const struct timer_list * timer)
{
    return timer->entry.prev != NULL;
}


static inline unsigned int tbase_get_deferrable(struct tvec_base *base)
{
    return ((unsigned int)(unsigned long)base & TIMER_DEFERRABLE);
}

static void __internal_add_timer(struct tvec_base *base, struct timer_list *timer)
{
    unsigned long expires = timer->expires;
    unsigned long idx = expires - base->timer_jiffies;
    struct list_head_ *vec;

    if (idx < TVR_SIZE) {
        int i = expires & TVR_MASK;
        vec = base->tv1.vec + i;
    } else if (idx < 1 << (TVR_BITS + TVN_BITS)) {
        int i = (expires >> TVR_BITS) & TVN_MASK;
        vec = base->tv2.vec + i;
    } else if (idx < 1 << (TVR_BITS + 2 * TVN_BITS)) {
        int i = (expires >> (TVR_BITS + TVN_BITS)) & TVN_MASK;
        vec = base->tv3.vec + i;
    } else if (idx < 1 << (TVR_BITS + 3 * TVN_BITS)) {
        int i = (expires >> (TVR_BITS + 2 * TVN_BITS)) & TVN_MASK;
        vec = base->tv4.vec + i;
    } else if ((signed long) idx < 0) {
        /*
         * Can happen if you add a timer with expires == jiffies,
         * or you set a timer to go off in the past
         */
        vec = base->tv1.vec + (base->timer_jiffies & TVR_MASK);
    } else {
        int i;
        /* If the timeout is larger than MAX_TVAL (on 64-bit
         * architectures or with CONFIG_BASE_SMALL=1) then we
         * use the maximum timeout.
         */
        if (idx > MAX_TVAL) {
            idx = MAX_TVAL;
            expires = idx + base->timer_jiffies;
        }
        i = (expires >> (TVR_BITS + 3 * TVN_BITS)) & TVN_MASK;
        vec = base->tv5.vec + i;
    }
    /*
     * Timers are FIFO:
     */
    list_add_tail_(&timer->entry, vec);

}

static void internal_add_timer(struct tvec_base *base, struct timer_list *timer)
{
    __internal_add_timer(base, timer);
    /*
     * Update base->active_timers and base->next_timer
     */
    if (time_before(timer->expires, base->next_timer))
        base->next_timer = timer->expires;
    base->active_timers++;
}

static int cascade(struct tvec_base *base, struct tvec *tv, int index)
{
    /* cascade all the timers from tv up one level */
    struct timer_list *timer, *tmp;
    struct list_head_ tv_list;

    list_replace_init_(tv->vec + index, &tv_list);

    /*
     * We are removing _all_ timers from the list, so we
     * don't have to detach them individually.
     */
    list_for_each_entry_safe_(timer, tmp, &tv_list, entry) {
        /* No accounting, while moving them */
        __internal_add_timer(base, timer);
    }

    return index;
}

static void detach_expired_timer(struct timer_list *timer, struct tvec_base *base)
{
    detach_timer(timer, true);
    if (!(timer->flags & TIMER_DEFERRABLE))
        base->active_timers--;
}

static int detach_if_pending(struct timer_list *timer, struct tvec_base *base, int clear_pending)
{
    if (!timer_pending(timer))
        return 0;
    detach_timer(timer, clear_pending);
    if (!(timer->flags & TIMER_DEFERRABLE)) {
        base->active_timers--;
        if (timer->expires == base->next_timer)
            base->next_timer = base->timer_jiffies;
    }
    /* If this was the last timer, advance base->jiffies */
    return 1;
}

static void call_timer_fn(struct timer_list *timer/* , int (*fn)(unsigned long, unsigned int, unsigned int, unsigned int), */
        /* unsigned long data, unsigned int event, unsigned int context, unsigned int index */)
{

#ifdef CONFIG_LOCKDEP
    /*
     * It is permissible to free the timer from inside the
     * function that is called from it, this we need to take into
     * account for lockdep too. To avoid bogus "held lock freed"
     * warnings as well as problems when looking into
     * timer->lockdep_map, make a copy and use that here.
     */
    //  struct lockdep_map lockdep_map;

    //  lockdep_copy_map(&lockdep_map, &timer->lockdep_map);
#endif
    /*
     * Couple the lock chain with the lock chain at
     * del_timer_sync() by acquiring the lock_map around the fn()
     * call here and in del_timer_sync().
     */
    //  lock_map_acquire(&lockdep_map);

    //  trace_timer_expire_entry(timer);
    /* int runFlag =  */timer->function((void *)timer->data);//, event, context, index);
    /* if (timer->runFlag) { timer->runFlag = runFlag; } */
    //  trace_timer_expire_exit(timer);

    //  lock_map_release(&lockdep_map);
}

#define INDEX(N) ((base->timer_jiffies >> (TVR_BITS + (N) * TVN_BITS)) & TVN_MASK)

static void update_jiffies(struct timer_list *timer) {
    timer->prevExpires  = timer->expires;
    timer->expires      += timer->intervalTime;
}

/**
 * __run_timers - run all expired timers (if any) on this CPU.
 * @base: the timer vector to be processed.
 *
 * This function cascades all vectors and executes all expired timer
 * vectors.
 */
static void __run_timers(struct tvec_base *base)
{
    struct timer_list *timer;
    //获取当前的实际时间
    unsigned long cur_jiffies = jiffies;

    //检测时间是否到了
    while (time_after_eq(cur_jiffies, base->timer_jiffies)) {
        struct list_head_ work_list;
        struct list_head_ *head = &work_list;
        int index = base->timer_jiffies & TVR_MASK;

        /*
         * Cascade timers:
         */
        if (!index &&
                (!cascade(base, &base->tv2, INDEX(0))) &&
                (!cascade(base, &base->tv3, INDEX(1))) &&
                !cascade(base, &base->tv4, INDEX(2)))
            cascade(base, &base->tv5, INDEX(3));
        ++base->timer_jiffies;
        list_replace_init_(base->tv1.vec + index, &work_list);
        while (!list_empty_(head)) {
            //void (*fn)(unsigned long data, unsigned int event, unsigned int context, unsigned int timerId);

            timer = list_first_entry_(head, struct timer_list,entry);

            base->running_timer = timer;

            detach_expired_timer(timer, base);

            call_timer_fn(timer);//, timer->function, timer->data, timer->event, timer->context, timer->index);
            /* if (timer->runFlag) { */
            /* mod_timer(timer, jiffies + timer->intervalTime); */
            update_jiffies(timer);
            mod_timer(timer, timer->expires);
            /* } */
        }
    }
    base->running_timer = NULL;
}

static void run_timer_softirq(void)
{
    struct tvec_base *base = &boot_tvec_bases;

    if (time_after_eq(jiffies, base->timer_jiffies))
        __run_timers(base);
}

static void detach_timer(struct timer_list *timer, int clear_pending)
{
    struct list_head_ *entry = &timer->entry;
    if(!list_empty_(entry))
    {
        __list_del_(entry->prev, entry->next);
        if (clear_pending)
            entry->next = NULL;
        //entry->prev = (list_head_ *)LIST_POISON2;
        entry->prev = NULL;
        /* timer->aliveFlag = 0; */
    }
}

static int __mod_timer(struct timer_list *timer, unsigned long expires,
        int pending_only, int pinned)
{
    struct tvec_base *base;
    int ret = 0;

    base = &boot_tvec_bases;

    ret = detach_if_pending(timer, base, false);
    if (!ret && pending_only)
        goto out_unlock;

    /*if (!ret && pending_only)
      goto out_unlock;
      */
    timer->expires = expires;
    /* timer->aliveFlag = 1; */
    internal_add_timer(base, timer);

out_unlock:

    return ret;
}

/**
 * mod_timer - modify a timer's timeout
 * @timer: the timer to be modified
 * @expires: new timeout in jiffies
 *
 * mod_timer() is a more efficient way to update the expire field of an
 * active timer (if the timer is inactive it will be activated)
 *
 * mod_timer(timer, expires) is equivalent to:
 *
 *     del_timer(timer); timer->expires = expires; add_timer(timer);
 *
 * Note that if there are multiple unserialized concurrent users of the
 * same timer, then mod_timer() is the only safe way to modify the timeout,
 * since add_timer() cannot modify an already running timer.
 *
 * The function returns whether it has modified a pending timer or not.
 * (ie. mod_timer() of an inactive timer returns 0, mod_timer() of an
 * active timer returns 1.)
 */
static int mod_timer(struct timer_list *timer, unsigned long expires)
{
    /*
     * This is a common optimization triggered by the
     * networking code - if the timer is re-modified
     * to be the same thing then just return:
     */
    if (timer_pending(timer) && timer->expires == expires)
        return 1;

    return __mod_timer(timer, expires, false, TIMER_NOT_PINNED);
}


static void scan_timer(void)
{
    TSKA_mutexLock(&g_timer_lock);
    run_timer_softirq();
    TSKA_mutexUnlock(&g_timer_lock);
}

static void *Kernel_TimerThread(void *data) {
    TSKTHRDSETPROCNAME("KernelTimer");

    while (1) {
        scan_timer();
        TSKA_waitMsecs(5);//最小颗粒度为5ms
    }

    return NULL;
}

int Kernel_TimerInit(KernelTimerPrint print, void *priv)
{
    int j;
    int status = -1;
    struct tvec_base *base;

    base = &boot_tvec_bases;

    gPrint  = print;
    gPriv   = priv;
    gLevel  = LogKernelTimer_Info;

    TSKA_mutexCreate(&g_timer_lock);

    for (j = 0; j < TVN_SIZE; j++) {
        INIT_LIST_HEAD_(base->tv5.vec + j);
        INIT_LIST_HEAD_(base->tv4.vec + j);
        INIT_LIST_HEAD_(base->tv3.vec + j);
        INIT_LIST_HEAD_(base->tv2.vec + j);
    }
    for (j = 0; j < TVR_SIZE; j++)
        INIT_LIST_HEAD_(base->tv1.vec + j);

    //获取当前的实际时间
    base->timer_jiffies = jiffies;
    base->next_timer = base->timer_jiffies;
    base->active_timers = 0;

    status = TSKA_thrCreate(&g_thread, Kernel_TimerThread, 99, 0, base);
    ERRP(0 != status, goto ERR0, 1, "TSKA_thrCreate data->thread failure\n");

    return 0;
ERR0:
    TSKA_mutexDelete(&g_timer_lock);

    return -1;
}

void Kernel_TimerOut(void) {
    TSKA_thrDelete(&g_thread);
    TSKA_mutexDelete(&g_timer_lock);
}

static int32_t key_compute(void *v_key, void *u_key) {
    struct timer_list *key = ((struct timer_list *)v_key);
    struct timer_list *_key = ((struct timer_list *)u_key);

    if (key->timerID > _key->timerID) return 1;
    else if (key->timerID < _key->timerID) return -1;
    else return 0;
}

int Kernel_AddTimer(unsigned int interval, void *oObj, functionCb TimerCallBackFn)
{
    struct timer_list *timer    = NULL;
    rbtree_manages *manage      = NULL;

    timer = (struct timer_list *) malloc (sizeof(*timer));
    ERRP(NULL == timer, goto ERR0, 1, "malloc struct timer_list failure\n");
    memset(timer, 0x0, sizeof(*timer));

    manage = (rbtree_manages *) malloc (sizeof(*manage));
    ERRP(NULL == manage, goto ERR1, 1, "malloc rbtree_manages failure\n");
    memset(manage, 0x0, sizeof(*manage));

    TSKA_mutexLock(&g_timer_lock);
    init_timer(timer);  

    timer->timerID          = timerID++;
    timer->data             = (unsigned long)oObj;
    timer->intervalTime     = interval;                         //定时器定时间隔
    timer->prevExpires      = jiffies;                          //上一次定时器触发实际时间 
    timer->expires          = timer->prevExpires + interval;    //获取当前实际时间+定时器定时间隔，也即实际的下一次触发时间
    timer->function         = TimerCallBackFn;
    mod_timer(timer, timer->expires);

    manage->key = (void *)timer;
    inserts((rbtree_manages *)manage, &gRoots, key_compute);

    TSKA_mutexUnlock(&g_timer_lock);

    return timer->timerID;
ERR1:
    if (timer) {
        free(timer);
    }
ERR0:
    TSKA_mutexUnlock(&g_timer_lock);

    return -1;
}

int kernel_RemoveTimer(unsigned int timerID)
{
    struct timer_list *timer       = NULL;
    rbtree_manages *manage  = NULL;
    struct timer_list node;
    struct tvec_base *base;

    TSKA_mutexLock(&g_timer_lock);

    node.timerID   = timerID;
    manage = (rbtree_manages *)searchs(&node, &gRoots, key_compute);
    ERRP(NULL == manage, goto ERR0, 1, "TimerId:%u not found?\n", timerID);
    
    timer = (struct timer_list *)manage->key;

    erases(manage, &gRoots);

    free(manage);
    manage = NULL;

    if (timer_pending(timer)) {
        base = &boot_tvec_bases;
        detach_if_pending(timer, base, true);
    }

    free(timer);
    timer = NULL;

    TSKA_mutexUnlock(&g_timer_lock);

    return 0;
ERR0:
    TSKA_mutexUnlock(&g_timer_lock);

    return -1;
}

#ifdef __cplusplus
}
#endif /* _cplusplus */
#endif
