#ifndef _TSKA_MSGQ_H_
#define _TSKA_MSGQ_H_

#include <tska.h>
#include <tska_que.h>

#define TSKA_MSGQ_LEN_MAX    320

struct TSKA_MsgHndl;

typedef struct {

    uint32_t curRd;
    uint32_t curWr;
    uint32_t len;
    uint32_t count;

    uint32_t lspedRd;
    uint32_t lspedWr;
    uint32_t lspedLen;
    uint32_t lspedCnt;

    struct TSKA_MsgHndl *queue[TSKA_MSGQ_LEN_MAX];
    struct TSKA_MsgHndl *lspedque[TSKA_MSGQ_LEN_MAX];

    pthread_mutex_t lock;
    pthread_cond_t  condRd;
    pthread_cond_t  condWr;

    TSKA_QueHndl memQue;    ///< mem mailbox
    struct TSKA_MsgHndl *mem[TSKA_MSGQ_LEN_MAX];

    //处理需ACK的消息
    struct TSKA_MsgHndl *msgq[TSKA_MSGQ_BATCH_MAX_NUM];

    //处理需定时触发消息(不能丢弃的消息)
    struct TSKA_MsgHndl *jitterMsgq[TSKA_MSGQ_BATCH_MAX_NUM];

    int32_t refer;
} TSKA_MsgqHndl;

typedef struct TSKA_MsgHndl {

    TSKA_MsgqHndl *pTo;
    TSKA_MsgqHndl *pFrom;  
    void         *pPrm[TSKA_MSGQ_BATCH_MAX_NUM];
    int32_t       numBufs;
    int           status;
    uint32_t      linkId;
    uint16_t        cmd;
    uint16_t        flags;
    uint16_t      isMalloc;

} TSKA_MsgHndl;

#define TSKA_msgGetCmd(msg)         ( (msg)->cmd )
#define TSKA_msgGetPrm(msg)         ( (msg)->pPrm[0])
#define TSKA_msgGetPrmNum(msg)      ( (msg)->numBufs)
#define TSKA_msgGetPrmEx(msg)       ( (msg)->pPrm)
#define TSKA_msgGetLinkId(msg)      ( (msg)->linkId )
#define TSKA_msgGetAckStatus(msg)   ( (msg)->status )

#if defined (__cplusplus)
extern "C" {
#endif

int TSKA_msgqCreate(TSKA_MsgqHndl *hndl);
int TSKA_msgqDelete(TSKA_MsgqHndl *hndl);
int TSKA_msgqRecvNobMsg(TSKA_MsgqHndl *hndl, TSKA_MsgHndl **msg, uint32_t timeout);
int TSKA_msgqSendNobMsg(TSKA_MsgqHndl *to, TSKA_MsgqHndl *from, uint16_t cmd, void *prm, uint16_t msgFlags, TSKA_MsgHndl **msg);
int TSKA_msgqSendLspedMsg(TSKA_MsgqHndl *to, TSKA_MsgqHndl *from, uint32_t linkId, uint16_t cmd, void *prm, uint16_t msgFlags, TSKA_MsgHndl **msg);
int TSKA_msgqSendMsg(TSKA_MsgqHndl *to, TSKA_MsgqHndl *from, uint32_t linkId, uint16_t cmd, void *prm, uint16_t msgFlags, TSKA_MsgHndl **msg);
int TSKA_msgqSendMsgLp(TSKA_MsgqHndl *to, TSKA_MsgqHndl *from, uint32_t linkId, uint16_t cmd, void *prm, uint16_t msgFlags, TSKA_MsgHndl **msg);
int TSKA_msgqSendMsgEx(TSKA_MsgqHndl *to, TSKA_MsgqHndl *from, 
        uint32_t linkId, uint16_t cmd, void *prm[], int32_t numBufs,
        uint16_t msgFlags);
int TSKA_msgqRecvMsg(TSKA_MsgqHndl *hndl, TSKA_MsgHndl **msg, uint32_t timeout);
int TSKA_msgqSendAck(TSKA_MsgHndl *msg, int ackRetVal);
int TSKA_msgqFreeMsgHndl(TSKA_MsgHndl *msg);
int TSKA_msgqPeek(TSKA_MsgqHndl *hndl, TSKA_MsgHndl **msg);
int TSKA_msgqHspedPeek(TSKA_MsgqHndl *hndl, TSKA_MsgHndl **msg);

int TSKA_msgqCount(TSKA_MsgqHndl *hndl, int* count);

#if defined (__cplusplus)
}
#endif

#endif /* _TSKA_MSGQ_H_ */



