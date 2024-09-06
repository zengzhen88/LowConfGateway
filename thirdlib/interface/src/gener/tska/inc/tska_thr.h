

#ifndef _TSKA_THR_H_
#define _TSKA_THR_H_

#include <tska.h>

#define TSKA_THR_PRI_MAX                 sched_get_priority_max(SCHED_RR)
#define TSKA_THR_PRI_MIN                 sched_get_priority_min(SCHED_RR)

#define TSKA_THR_PRI_DEFAULT             ( TSKA_THR_PRI_MIN + (TSKA_THR_PRI_MAX-TSKA_THR_PRI_MIN)/2 )

#define TSKA_THR_STACK_SIZE_DEFAULT      0

typedef void * (*TSKA_ThrEntryFunc)(void *);

typedef struct {

    pthread_t      hndl;

} TSKA_ThrHndl;

#if defined (__cplusplus)
extern "C" {
#endif

int TSKA_thrCreate(TSKA_ThrHndl *hndl, TSKA_ThrEntryFunc entryFunc, uint32_t pri, uint32_t stackSize, void *prm);
int TSKA_thrDelete(TSKA_ThrHndl *hndl);
int TSKA_thrJoin(TSKA_ThrHndl *hndl);
int TSKA_thrChangePri(TSKA_ThrHndl *hndl, uint32_t pri);
int TSKA_thrExit(void *returnVal);

#if defined (__cplusplus)
}
#endif

#endif /* _TSKA_THR_H_ */



