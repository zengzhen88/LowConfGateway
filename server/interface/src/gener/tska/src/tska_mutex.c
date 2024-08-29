#include <tska_mutex.h>

int TSKA_mutexCreate(TSKA_MutexHndl *hndl)
{
    pthread_mutexattr_t mutex_attr;
    int status = TSKA_SOK;

    status |= pthread_mutexattr_init(&mutex_attr);
    status |= pthread_mutex_init(&hndl->lock, &mutex_attr);

    if(status != TSKA_SOK)
        TSKA_ERROR("TSKA_mutexCreate() = %d \r\n", status);

    pthread_mutexattr_destroy(&mutex_attr);

    return status;
}

int TSKA_mutexDelete(TSKA_MutexHndl *hndl)
{
    pthread_mutex_destroy(&hndl->lock);

    return TSKA_SOK;
}

int TSKA_mutexLock(TSKA_MutexHndl *hndl)
{
    return pthread_mutex_lock(&hndl->lock);
}

int TSKA_mutexUnlock(TSKA_MutexHndl *hndl)
{
    return pthread_mutex_unlock(&hndl->lock);
}


