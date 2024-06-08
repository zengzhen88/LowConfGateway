#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>

#ifndef __RBTREE_H__
#define __RBTREE_H__

#if defined (__cplusplus)
extern "C" {
#endif



/******************interface********************/

typedef int (*computes)(void *key, void *_key);
typedef void (*prints)(int64_t key);


/*new interface*/
void *rbtreeCreate(void);
void *rbtreeSearchs(void *key, void *root, computes callback_compute);
int32_t rbtreeInserts(void *key, void *root, computes callback_compute);
int32_t rbtreeErases(void *key, void *root, computes callback_compute);

#if defined (__cplusplus)
}
#endif

#endif /* __RBTREE_H__*/
