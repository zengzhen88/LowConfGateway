#ifndef __STREAM_BUFFER_H__
#define __STREAM_BUFFER_H__ 

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************Buffer*************************/ 
#define TASK_BUF_MAX_NUMFS  (16)

typedef struct {
    uint32_t    fillLength;                   //实时数据大小
    uint32_t    timeStamp;                    //timestamp
    uint32_t    highTimes;                    //times high gettimeofday 
    uint32_t    lowTimes;                     //times low  gettimeofday
	uint16_t    payload; 
    uint16_t    seqId;
    int32_t     reffer;                       //引用计数      //做内存复用使用
	void        *handle;                      //存储私有指针

    uint32_t    stuLength;                    //底层维护不允许修改
    uint32_t    size;                         //实时申请的数据包总大小 //底层维护不允许修改
    uint32_t    linkId;                       //LinkId，唯一标识  （注意：是指本环节的linkId，强制原则,这个参数由配置设置，业务层不允许修改）
    uint32_t    nextId;                       //LinkId，唯一标识   (注意：是指下一级的linkId，原则上这个参数由配置设置，精力层不允许修改)
    uint32_t    logicChan;                   //逻辑通道，私有，外部不允许修改(强制原则，这个参数不允许修改，由底层维护)
	uint32_t    numChannel;                   //下一级环节对应的逻辑通道（注意是指下一级的，原则上这个参数由配置设置，业务层不允许设置）
    void *addr;                             //具体存储数据的内存地址（强制原则，不允许修改)
} TaskBuf;

typedef struct {
    uint32_t  numBufs;                      //TaskBuf能被承载的实时个数
    TaskBuf *bufs[TASK_BUF_MAX_NUMFS];      //TaskBuf承载数组指针
} TaskBufList;

#ifdef __cplusplus
}
#endif

#endif /*__STREAM_BUFFER_H__*/
