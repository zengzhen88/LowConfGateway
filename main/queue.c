#include <stdio.h>
#include <stdlib.h>
#include <queue.h>

Queue *CreateQueue(int k) {
    // 创建队列结构体
    Queue *cq = (Queue *)malloc(sizeof(Queue));
    
    // 为队列里的数组创建k+1空间
    cq->data = (long *)malloc((k+1) * sizeof(long));
    cq->head = cq->tail = 0;
    cq->k = k;
    
    return cq;
}

int32_t EnQueue(Queue *obj, long value) {
    // 队列满了，就不能插入数据
    if (QueueIsFull(obj))
        return -1;

    obj->data[obj->tail] = value;
    obj->tail++;
    
    // 插入数据会导致tail下标越界，越界处理：越界了就回到开始位置
    obj->tail %= obj->k+1;

    return 0;
}

int32_t DeQueue(Queue *obj, long *value) {
    // 队列为空，不能删除数据
    if (QueueIsEmpty(obj))
        return -1;

    *value = oObj->data[obj->head++];
    // 删除数据会导致head下标越界，越界处理：越界了就回到开始位置
    obj->head %= obj->k+1;

    return 0;
}

int32_t QueueIsEmpty(Queue *obj) {
    // head和tail相等就是空
    return obj->head == obj->tail;
}

int32_t QueueIsFull(Queue *obj) {
    // tail+1取余k+1，等于head，就是满
    return (obj->tail+1) % (obj->k+1) == obj->head; 
}

void QueueFree(Queue *obj) {
    free(obj->data);
    obj->head = obj->tail = obj->k = 0;
    free(obj);
}
