

#ifndef _TSKA_QUE_H_
#define _TSKA_QUE_H_

#include <tska.h>

typedef struct {

  uint32_t curRd;
  uint32_t curWr;
  uint32_t len;
  uint32_t count;

  long *queue;

  pthread_mutex_t lock;
  pthread_cond_t  condRd;
  pthread_cond_t  condWr;
  
} TSKA_QueHndl;

#if defined (__cplusplus)
extern "C" {
#endif

#define TSKA_MAX_QUE_NUM    (384)

int TSKA_queCreate(TSKA_QueHndl *hndl, uint32_t maxLen);
int TSKA_queDelete(TSKA_QueHndl *hndl);
int TSKA_quePut(TSKA_QueHndl *hndl, long  value, uint32_t timeout);
int TSKA_queGet(TSKA_QueHndl *hndl, long *value, uint32_t timeout);
int TSKA_queGetPlus(TSKA_QueHndl *hndl, long *value, uint32_t timeout, uint32_t *pQueuedCounts);
int TSKA_quePeek(TSKA_QueHndl *hndl, long *value);
uint32_t TSKA_queGetQueuedCount(TSKA_QueHndl *hndl);
int32_t TSKA_queIsEmpty(TSKA_QueHndl *hndl);

#if defined (__cplusplus)
}
#endif

#endif /* _TSKA_QUE_H_ */



