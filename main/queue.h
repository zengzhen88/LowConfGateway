#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct {
    long *data;
    int head;	// 头
    int tail;	// 尾
    int k;		// 队列长度
} Queue;

Queue *CreateQueue(int k);
int32_t EnQueue(Queue *obj, long value);
int32_t DeQueue(Queue *obj);
int32_t QueueIsEmpty(Queue *obj);
int32_t QueueIsFull(Queue *obj);
void QueueFree(Queue *obj);
#endif
