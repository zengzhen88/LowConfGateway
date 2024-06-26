#include <tska_que.h>

int TSKA_queCreate(TSKA_QueHndl *hndl, uint32_t maxLen)
{
    pthread_mutexattr_t mutex_attr;
    pthread_condattr_t cond_attr;
    int32_t status = TSKA_SOK;

    hndl->curRd = hndl->curWr = 0;
    hndl->count = 0;
    hndl->len   = maxLen;
    hndl->queue = malloc (sizeof(long)*hndl->len);
    if(hndl->queue == NULL) {
        TSKA_ERROR("TSKA_queCreate() = %d \r\n", status);
        return TSKA_EFAIL;
    }

    memset(hndl->queue, 0x0, sizeof(long) * hndl->len);

    status |= pthread_mutexattr_init(&mutex_attr);
    status |= pthread_condattr_init(&cond_attr);  

    status |= pthread_mutex_init(&hndl->lock, &mutex_attr);
    status |= pthread_cond_init(&hndl->condRd, &cond_attr);    
    status |= pthread_cond_init(&hndl->condWr, &cond_attr);  

    if(status != TSKA_SOK)
        TSKA_ERROR("TSKA_queCreate() = %d \r\n", status);

    pthread_condattr_destroy(&cond_attr);
    pthread_mutexattr_destroy(&mutex_attr);

    return status;
}

int TSKA_queDelete(TSKA_QueHndl *hndl)
{
    if(hndl->queue!=NULL)
        free(hndl->queue);

    pthread_cond_destroy(&hndl->condRd);
    pthread_cond_destroy(&hndl->condWr);
    pthread_mutex_destroy(&hndl->lock);  

    return TSKA_SOK;
}

static long long tm_to_ns(struct timespec tm)
{
	return tm.tv_sec * 1000000000LL + tm.tv_nsec*1LL;
}

static struct timespec ns_to_tm(long long ns)
{
	struct timespec tm;
	tm.tv_sec = ns / 1000000000LL;
	tm.tv_nsec = ns % 1000000000LL;//ns - (tm.tv_sec * 1000000000LL);
	return tm;
}

int TSKA_quePut(TSKA_QueHndl *hndl, long value, uint32_t timeout)
{
    int32_t status = TSKA_EFAIL;

    pthread_mutex_lock(&hndl->lock);

    while(1) {
        if( hndl->count < hndl->len ) {
            hndl->queue[hndl->curWr] = value;
            hndl->curWr = (hndl->curWr+1)%hndl->len;
            hndl->count++;
            status = TSKA_SOK;
            pthread_cond_signal(&hndl->condRd);
            break;
        } else {
            if(timeout == TSKA_TIMEOUT_NONE)
                break;
            else if (timeout == TSKA_TIMEOUT_FOREVER) {
                pthread_cond_wait(&hndl->condWr, &hndl->lock);
            }
            else {
                struct timespec start_tm;
                struct timespec end_tm;
                clock_gettime(CLOCK_MONOTONIC, &start_tm);
                end_tm = ns_to_tm(tm_to_ns(start_tm) + timeout*1000000LL);
                status = pthread_cond_timedwait(&hndl->condWr, &hndl->lock, &end_tm);
                if (status) break;
            }
        }
    }

    pthread_mutex_unlock(&hndl->lock);

    return status;
}


int TSKA_queGet(TSKA_QueHndl *hndl, long *value, uint32_t timeout)
{
    int32_t status = TSKA_EFAIL;

    pthread_mutex_lock(&hndl->lock);

    while(1) {
        if(hndl->count > 0 ) {

            if(value!=NULL) {
                *value = hndl->queue[hndl->curRd];
            }

            hndl->curRd = (hndl->curRd+1)%hndl->len;
            hndl->count--;
            status = TSKA_SOK;
            pthread_cond_signal(&hndl->condWr);
            break;
        } else {
            if(timeout == TSKA_TIMEOUT_NONE)
                break;
            else if (timeout == TSKA_TIMEOUT_FOREVER) 
                pthread_cond_wait(&hndl->condRd, &hndl->lock);
            else {
                struct timespec start_tm;
                struct timespec end_tm;
                clock_gettime(CLOCK_REALTIME, &start_tm);
                end_tm = ns_to_tm(tm_to_ns(start_tm) + timeout*1000000LL);
                status = pthread_cond_timedwait(&hndl->condRd, &hndl->lock, &end_tm);
                if (status) break;
            }

        }
    }

    pthread_mutex_unlock(&hndl->lock);

    return status;
}

int TSKA_queGetPlus(TSKA_QueHndl *hndl, long *value, uint32_t timeout, uint32_t *pQueuedCounts)
{
    int32_t status = TSKA_EFAIL;

    pthread_mutex_lock(&hndl->lock);

    while(1) {
        if(hndl->count > 0 ) {

            if(value!=NULL) {
                *value = hndl->queue[hndl->curRd];
            }

            hndl->curRd = (hndl->curRd+1)%hndl->len;
            hndl->count--;

            if (pQueuedCounts) *pQueuedCounts = hndl->count;

            status = TSKA_SOK;
            pthread_cond_signal(&hndl->condWr);
            break;
        } else {
            if(timeout == TSKA_TIMEOUT_NONE) {
                if (pQueuedCounts)*pQueuedCounts = 0;
                break;
            }

            pthread_cond_wait(&hndl->condRd, &hndl->lock);
        }
    }

    pthread_mutex_unlock(&hndl->lock);

    return status;
}

uint32_t TSKA_queGetQueuedCount(TSKA_QueHndl *hndl)
{
    uint32_t queuedCount = 0;

    pthread_mutex_lock(&hndl->lock);
    queuedCount = hndl->count;
    pthread_mutex_unlock(&hndl->lock);
    return queuedCount;
}

int32_t TSKA_quePeek(TSKA_QueHndl *hndl, long *value)
{
    int32_t status = TSKA_EFAIL;
    pthread_mutex_lock(&hndl->lock);
    if(hndl->count > 0 ) {
        if(value!=NULL) {
            *value = hndl->queue[hndl->curRd];
            status = TSKA_SOK;
        }
    }
    pthread_mutex_unlock(&hndl->lock);

    return status;
}

int32_t TSKA_queIsEmpty(TSKA_QueHndl *hndl)
{
    int32_t isEmpty;

    pthread_mutex_lock(&hndl->lock);
    if (hndl->count == 0)
    {
        isEmpty = 1;
    }
    else
    {
        isEmpty = 0;
    }
    pthread_mutex_unlock(&hndl->lock);

    return isEmpty;
}



