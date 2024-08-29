#ifndef _TSKA_H_
#define _TSKA_H_

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>

#define TSKA_MSGQ_BATCH_MAX_NUM     (16)

#define TSKA_PRF_ENABLE // enable profiling APIs

#define TSKA_SOK      0  ///< Status : OK
#define TSKA_EFAIL   -1  ///< Status : Generic error

#ifndef _TI_STD_TYPES
#define _TI_STD_TYPES

typedef   void *  TSKA_PTR;

#define TSKA_SUSPEND     (0xFFFFFFFF)
#define TSKA_NO_SUSPEND  (0)


#endif /* _TI_STD_TYPES */

#ifndef KB
#define KB ((uint32_t)1024)
#endif

#ifndef MB
#define MB (KB*KB)
#endif

#define TSKA_TIMEOUT_NONE        ((uint32_t) 0)  // no timeout
#define TSKA_TIMEOUT_FOREVER     ((uint32_t)-1)  // wait forever

#define TSKA_memAlloc(size)      (void*)malloc((size))
#define TSKA_memFree(ptr)        free(ptr)

#define TSKA_align(value, align)   ((( (value) + ( (align) - 1 ) ) / (align) ) * (align) )

#define TSKA_floor(value, align)   (( (value) / (align) ) * (align) )
#define TSKA_ceil(value, align)    TSKA_align(value, align)

#define TSKA_SNPRINTF(sbuf,...)                                               \
                                do {                                           \
                                    snprintf (sbuf, sizeof(sbuf) - 1,          \
                                              __VA_ARGS__);                    \
                                    sbuf[sizeof(sbuf) - 1] = 0;                \
                                } while (0)

#define TSKA_ARRAYSIZE(array)             ((sizeof(array)/sizeof((array)[0])))

#define TSKA_ARRAYINDEX(elem,array)       ((elem) - &((array)[0]))

#define TSKA_ARRAYISVALIDENTRY(elem,array) ((TSKA_ARRAYINDEX(elem,array) <   \
                                             TSKA_ARRAYSIZE(array))           \
                                             ? 1                            \
                                             : 0)
#define TSKA_DIV(num,den)                  (((den) != 0) ? ((num)/(den)) : 0)

#define TSKA_ISERROR(status)               ((status < 0) ? 1 : 0)

#include <tska_debug.h>

#if defined (__cplusplus)
extern "C" {
#endif

uint64_t TSKA_getActualTimeInUsec();
uint32_t TSKA_getCurTimeInMsec(void);
uint64_t TSKA_getCurTimeInUsec();
uint64_t TSKA_getCurTimeInNsec();
void TSKA_waitUsecs(uint64_t usecs);
void   TSKA_waitMsecs(uint32_t msecs);
int    TSKA_attachSignalHandler(int sigId, void (*handler)(int ) );
//Int32 TSKA_mapMem(uint32_t physAddr, uint32_t memSize , Ptr *pMemVirtAddrPtr);
//Int32 TSKA_unmapMem(Ptr pMemVirtAddrPtr,uint32_t memSize);

int32_t xstrtois(int8_t *hex);

#if defined (__cplusplus)
}
#endif

#endif /* _TSKA_H_ */



