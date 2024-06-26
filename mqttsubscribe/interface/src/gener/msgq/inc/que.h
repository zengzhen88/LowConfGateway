#ifndef __YUERIN_QUEUE_H__
#define __YUERIN_QUEUE_H__

typedef struct {
    int32_t len;
    int32_t count;
    int32_t rd;
    int32_t wd;
    long *data;
} QueObj;

#ifdef __cplusplus
extern "C" {
#endif

void *QueInit(int32_t len);
void QueDestory(void *oObj);
int32_t QueDQ(void *oObj, long *data);
int32_t QueEQ(void *oObj, long data);
int32_t QueCount(void *oObj);

#ifdef __cplusplus
};
#endif

#endif /*__YUERIN_QUEUE_H__*/
