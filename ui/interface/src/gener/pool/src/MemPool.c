#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MemPool.h>
#include <tska_que.h>
#include <tska.h>

typedef struct {    
    TSKA_QueHndl handle;

    MemReq request;
    MemRls release;

    int32_t uint;
    int32_t uintNum;

    long *base;
    long *alignBase;

    void *priv;

    int8_t name[64];
} MemPool;

int32_t MemPoolInit(void **handle, MemPoolConfig *config) {
    int32_t index       = 0;
    int32_t status      = TSKA_EFAIL;
    MemPool *pool       = NULL;
    long buf            = 0;

    if (NULL == handle || NULL == config) return TSKA_EFAIL;

    pool = (MemPool *) malloc (sizeof(MemPool));
    if (NULL == pool) {
        fprintf (stderr, "[%s:%s:%d]malloc pool instance %s failure\n", 
                __FILE__, __func__, __LINE__,
                config->name);
        return TSKA_EFAIL;
    }
    printf ("[%s:%s:%d]name:%s pool:%p malloc\n", 
            __FILE__, __func__, __LINE__,
            config->name, pool);

    pool->request   = config->request;
    pool->release   = config->release;
    pool->uint      = TSKA_align(config->uint, 128); /*128 align*/
    pool->uintNum   = config->uintNum + 1;  /*add align*/
    pool->priv      = config->priv;

    strncpy((char *)pool->name, (char *)config->name, sizeof(pool->name) - 1);
    pool->name[sizeof(pool->name) - 1] = '\0';

    if (pool->request) {
        pool->base = (long *)pool->request(pool->priv, config->uint * config->uintNum + 128);
        if (!pool->base) {
            fprintf (stderr, "[%s:%s:%d]%s request from user func failure\n", 
                    __FILE__, __func__, __LINE__,
                    config->name);
            goto ERR0;
        }
    }
    else {
        pool->base = (long *) malloc (config->uint * config->uintNum + 128);
        if (!pool->base) {
            fprintf (stderr, "[%s:%s:%d]%s request from user func failure\n", 
                    __FILE__, __func__, __LINE__,
                    config->name);
            goto ERR0;
        }
    }

    pool->alignBase = (long *)TSKA_align((unsigned long)pool->base, 128);

    fprintf (stderr, "[%s:%s:%d] uint:%d uintNum:%d\n", 
            __FILE__, __FUNCTION__, __LINE__, config->uint, config->uintNum);
    fprintf (stderr, "[%s:%s:%d]pool size:%d base:%p align to size:%d alignBase:%p\n", 
            __FILE__, __func__, __LINE__,
            config->uint * config->uintNum, 
            pool->base, 
            pool->uint * pool->uintNum, 
            pool->alignBase);

    status = TSKA_queCreate(&pool->handle, config->uintNum);
    if (TSKA_SOK != status) {
        fprintf (stderr, "[%s:%s:%d]TSKA_queCreate %s failure\n", 
                __FILE__, __func__, __LINE__,
                config->name);
        goto ERR1;
    }

    buf = (long)((long)pool->alignBase);
    for (index = 0; index < config->uintNum; index++) {
        status = TSKA_quePut(&pool->handle, (long)buf, TSKA_TIMEOUT_NONE);
        if (TSKA_SOK != status) {
            fprintf (stderr, "[%s:%s:%d]%s que put index:%d failure\n", 
                    __FILE__, __func__, __LINE__,
                    config->name, index);
        }

        buf += pool->uint;
    }

    *handle = (void *)pool;

    return TSKA_SOK;
ERR1:
    if (pool->request) {
        pool->release(pool->priv, (void *)pool->base);
    }
    else {
        free((void *)pool->base);
    }
ERR0:
    free(pool);
    return TSKA_EFAIL;
}

void MemPoolDestory(void *handle) {
    MemPool *pool       = NULL;

    pool = (MemPool *) handle;

    TSKA_queDelete(&pool->handle);

    if (pool->request) {
        pool->release(pool->priv, (void *)pool->base);
    }
    else {
        free((void *)pool->base);
    }

    printf ("[%s:%s:%d]pool:%p destory\n",
            __FILE__, __func__, __LINE__,
            pool);
    free(pool);
}

void *MemPoolGet(void *handle) {
    MemPool *pool       = NULL;
    int32_t status      = TSKA_EFAIL;
    long mem         = 0;

    pool = (MemPool *) handle;

    status = TSKA_queGet(&pool->handle, (long *)&mem, TSKA_TIMEOUT_NONE);
    if (TSKA_SOK != status) {
        return NULL;
    }

    return (void *)((unsigned long)mem);
}

int32_t MemPoolPut(void *handle, long mem) {
    MemPool *pool       = NULL;

    pool = (MemPool *) handle;

    return TSKA_quePut(&pool->handle, (long)mem, TSKA_TIMEOUT_NONE);
}

int32_t MemPoolSize(void *handle) {
    MemPool *pool       = NULL;

    pool = (MemPool *) handle;

    return TSKA_queGetQueuedCount(&pool->handle);
}
