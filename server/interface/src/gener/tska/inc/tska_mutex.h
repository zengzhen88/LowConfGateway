#ifndef _TSKA_MUTEX_H_
#define _TSKA_MUTEX_H_

#include <pthread.h>
#include <tska.h>

typedef struct {

    pthread_mutex_t lock;

} TSKA_MutexHndl;

#if defined (__cplusplus)
extern "C" {
#endif

int TSKA_mutexCreate(TSKA_MutexHndl *hndl);
int TSKA_mutexDelete(TSKA_MutexHndl *hndl);
int TSKA_mutexLock(TSKA_MutexHndl *hndl);
int TSKA_mutexUnlock(TSKA_MutexHndl *hndl);

#if defined (__cplusplus)
}
#endif

#endif /* _TSKA_MUTEX_H_ */

