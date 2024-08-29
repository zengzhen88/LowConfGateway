#include <stdint.h>

#ifndef __MEMPOOL_H__
#define __MEMPOOL_H__

#if defined (__cplusplus)
extern "C" {
#endif

typedef void *(*MemReq)(void *, int32_t);
typedef void (*MemRls)(void *, void *);


typedef struct {
    int32_t uint;       /*申请内存块的单元大小*/
    int32_t uintNum;    /*申请内存块单元个数*/

    MemReq request;
    MemRls release;

    void *priv;

    int8_t name[64];
} MemPoolConfig;

int32_t MemPoolPut(void *handle, long mem);
void *MemPoolGet(void *handle);

void MemPoolDestory(void *handle);
int32_t MemPoolInit(void **handle, MemPoolConfig *config);
int32_t MemPoolSize(void *handle);

#if defined (__cplusplus)
}
#endif

#endif /* __MEMPOOL_H__*/
