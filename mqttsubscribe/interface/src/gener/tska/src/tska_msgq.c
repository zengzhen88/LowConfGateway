#include <tska_msgq.h>
#include <sys/time.h>

int TSKA_msgqCreate(TSKA_MsgqHndl *hndl)
{
    pthread_mutexattr_t mutex_attr;
    pthread_condattr_t cond_attr;
    int status=TSKA_SOK;
    int index = 0;

    status |= pthread_mutexattr_init(&mutex_attr);
    status |= pthread_condattr_init(&cond_attr);  

    status |= pthread_mutex_init(&hndl->lock, &mutex_attr);
    status |= pthread_cond_init(&hndl->condRd, &cond_attr);    
    status |= pthread_cond_init(&hndl->condWr, &cond_attr);  

    hndl->curRd = hndl->curWr = 0;
    hndl->count = 0;
    hndl->len   = TSKA_MSGQ_LEN_MAX;

    hndl->lspedRd  = hndl->lspedWr = 0;
    hndl->lspedCnt = 0;
    hndl->lspedLen = TSKA_MSGQ_LEN_MAX;

    status |= TSKA_queCreate(&hndl->memQue, TSKA_MSGQ_LEN_MAX);
    if(status!=TSKA_SOK)
        TSKA_ERROR("TSKA_msgqCreate() = %d \r\n", status);

    for (index = 0; index < TSKA_MSGQ_LEN_MAX; index++) {
        hndl->mem[index] = (struct TSKA_MsgHndl *) malloc (sizeof(struct TSKA_MsgHndl));
        if (NULL != hndl->mem[index]) {
            hndl->mem[index]->isMalloc = 1;
            status = TSKA_quePut(&hndl->memQue, (long)hndl->mem[index], TSKA_TIMEOUT_NONE);
            TSKA_assertSuccess(status);
        }
    }

    for (index = 0; index < TSKA_MSGQ_BATCH_MAX_NUM; index++) {
        hndl->msgq[index] = (struct TSKA_MsgHndl *) malloc (sizeof(struct TSKA_MsgHndl));
        if (NULL != hndl->msgq[index]) {
            hndl->msgq[index]->isMalloc = 0;
        }
    }


    pthread_condattr_destroy(&cond_attr);
    pthread_mutexattr_destroy(&mutex_attr);

    return status;
}

int TSKA_msgqDelete(TSKA_MsgqHndl *hndl)
{
    int index = 0;

    for (index = 0; index < TSKA_MSGQ_LEN_MAX; index++) {
        if (NULL != hndl->mem[index]) {
            free(hndl->mem[index]);
        }
    }

    for (index = 0; index < TSKA_MSGQ_BATCH_MAX_NUM; index++) {
        if (NULL != hndl->msgq[index]) {
            free(hndl->msgq[index]);
        }
    }

    TSKA_queDelete(&hndl->memQue);
    pthread_cond_destroy(&hndl->condRd);
    pthread_cond_destroy(&hndl->condWr);
    pthread_mutex_destroy(&hndl->lock);  

    return TSKA_SOK;
}

TSKA_MsgHndl *TSKA_msgqAllocLspedMsgHndl(TSKA_MsgqHndl *to, TSKA_MsgqHndl *from, uint32_t linkId, uint16_t cmd, void *prm, uint16_t msgFlags)
{
    TSKA_MsgHndl *msg = NULL;

    msg = from->msgq[0];
    if(msg!=NULL) {
        msg->pTo    = to;
        msg->pFrom  = from;
        msg->pPrm[0]= prm;
        msg->numBufs= 1;
        msg->status = TSKA_SOK;
        msg->cmd    = cmd;
        msg->flags  = msgFlags;
        msg->linkId = linkId;
    }

    return msg;
}

TSKA_MsgHndl *TSKA_msgqAllocMsgHndl(TSKA_MsgqHndl *to, TSKA_MsgqHndl *from, uint32_t linkId, uint16_t cmd, void *prm, uint16_t msgFlags)
{
    int status = TSKA_EFAIL;
    TSKA_MsgHndl *msg = NULL;

     /*
      * if (to->refer++ % 1000 == 0) {
      *    printf ("#########%s %d ==> to:%p <== linkId:0x%08x "
      *            "cmd:%d TSKA_msgqAllocMsgHndl failure from->memQue:%d lspQue:%d lQue:%d\n",
      *            __func__, __LINE__, to, linkId, cmd, 
      *            TSKA_queGetQueuedCount(&from->memQue), from->lspedCnt, from->count);
      * }
      */

    status = TSKA_queGet(&from->memQue, (long *)&msg, TSKA_TIMEOUT_NONE);
    if (TSKA_SOK == status) {
        if(msg!=NULL) {
            msg->pTo    = to;
            msg->pFrom  = from;
            msg->pPrm[0]= prm;
            msg->numBufs= 1;
            msg->status = TSKA_SOK;
            msg->cmd    = cmd;
            msg->flags  = msgFlags;
            msg->linkId = linkId;
        }
    }
    else {
        if (to->refer++ % 1000 == 0) {
            printf ("zzzz %s %d ==> to:%p <== linkId:0x%08x "
                    "cmd:%u TSKA_msgqAllocMsgHndl failure from->memQue:%u lspQue:%u lQue:%u to->memQue:%u lspQue:%u lQue:%u\n",
                    __func__, __LINE__, to, linkId, cmd, 
                    TSKA_queGetQueuedCount(&from->memQue), from->lspedCnt, from->count,
                    TSKA_queGetQueuedCount(&to->memQue), to->lspedCnt, to->count);
        }
    }

    return msg;
}

int TSKA_msgqSendLsped(TSKA_MsgqHndl *hndl, TSKA_MsgHndl *msg, uint32_t timeout)
{
    int status = TSKA_EFAIL;

    pthread_mutex_lock(&hndl->lock);

    while(1) {
        if( hndl->lspedCnt < hndl->lspedLen ) {
            hndl->lspedque[hndl->lspedWr] = msg;
            hndl->lspedWr = (hndl->lspedWr+1)%hndl->lspedLen;
            hndl->lspedCnt++;
            status = TSKA_SOK;
            pthread_cond_signal(&hndl->condRd);
            break;
        } 
        else {
            if(timeout == TSKA_TIMEOUT_NONE)
                break;

            status = pthread_cond_wait(&hndl->condWr, &hndl->lock);
        }
    }

    pthread_mutex_unlock(&hndl->lock);

    return status;
}

int TSKA_msgqSendLspedMsg(TSKA_MsgqHndl *to, TSKA_MsgqHndl *from, uint32_t linkId, 
        uint16_t cmd, void *prm, uint16_t msgFlags, TSKA_MsgHndl **msg)
{
    int status;
    TSKA_MsgHndl *data;

    data = TSKA_msgqAllocLspedMsgHndl(to, from, linkId, cmd, prm, msgFlags);
    if(data == NULL) {
        return TSKA_EFAIL;
    }

    status = TSKA_msgqSendLsped(to, data, TSKA_TIMEOUT_FOREVER);
    if(status == TSKA_SOK) {
        if(msg!= NULL)
            *msg = data;
    }
    else {
        /* TSKA_msgqFreeMsgHndl(data); */
    }

    return status;
}



int TSKA_msgqSend(TSKA_MsgqHndl *hndl, TSKA_MsgHndl *msg, uint32_t timeout)
{
    int status = TSKA_EFAIL;

    pthread_mutex_lock(&hndl->lock);

    while(1) {
        if( hndl->count < hndl->len ) {
            hndl->queue[hndl->curWr] = msg;
            hndl->curWr = (hndl->curWr+1)%hndl->len;
            hndl->count++;
            status = TSKA_SOK;
            pthread_cond_signal(&hndl->condRd);
            break;
        } else {
            if(timeout == TSKA_TIMEOUT_NONE)
                break;

            status = pthread_cond_wait(&hndl->condWr, &hndl->lock);
        }
    }

    pthread_mutex_unlock(&hndl->lock);

    return status;
}

int TSKA_msgqSendMsg(TSKA_MsgqHndl *to, TSKA_MsgqHndl *from, 
        uint32_t linkId, uint16_t cmd, void *prm, uint16_t msgFlags, TSKA_MsgHndl **msg)
{
    int status;
    TSKA_MsgHndl *data;

    data = TSKA_msgqAllocMsgHndl(to, from, linkId, cmd, prm, msgFlags);
    if(data==NULL)
        return TSKA_EFAIL;


    status = TSKA_msgqSend(to, data, TSKA_TIMEOUT_FOREVER);
    if(status==TSKA_SOK) {
        if(msg!=NULL)
            *msg = data;
    }
    else {
        TSKA_msgqFreeMsgHndl(data);
    }

    return status;
}

/*
 * int TSKA_msgqSendMsgEx(TSKA_MsgqHndl *to, TSKA_MsgqHndl *from, 
 *         uint32_t linkId, uint16_t cmd, void *prm[], int32_t numBufs,
 *         uint16_t msgFlags)
 * {
 *     int status;
 *     TSKA_MsgHndl *data;
 * 
 *     data = TSKA_msgqAllocMsgHndlEx(to, from, linkId, cmd, prm, numBufs, msgFlags);
 *     if(data==NULL)
 *         return TSKA_EFAIL;
 * 
 *     status = TSKA_msgqSend(to, data, TSKA_TIMEOUT_FOREVER);
 *     if(status==TSKA_SOK) {
 *     }
 *     else {
 *         TSKA_msgqFreeMsgHndl(data);
 *     }
 * 
 *     return status;
 * }
 */

int TSKA_msgqRecvMsg(TSKA_MsgqHndl *hndl, TSKA_MsgHndl **msg, uint32_t timeout)
{
    int status = TSKA_EFAIL;

    pthread_mutex_lock(&hndl->lock);

    while(1) {
        //优先级高
        if(hndl->lspedCnt > 0 ) {
            if(msg!=NULL) {
                *msg = hndl->lspedque[hndl->lspedRd];
            }

            hndl->lspedRd = (hndl->lspedRd+1)%hndl->lspedLen;
            hndl->lspedCnt--;
            status = TSKA_SOK;
            pthread_cond_signal(&hndl->condWr);
            break;
        }
        else if(hndl->count > 0 ) {

            if(msg!=NULL) {
                *msg = hndl->queue[hndl->curRd];
            }

            hndl->curRd = (hndl->curRd+1)%hndl->len;
            hndl->count--;
            status = TSKA_SOK;
            pthread_cond_signal(&hndl->condWr);
            break;
        } 
        else {
            if(timeout == TSKA_TIMEOUT_NONE)
                break;
            status = pthread_cond_wait(&hndl->condRd, &hndl->lock);
        }
    }

    pthread_mutex_unlock(&hndl->lock);

    return status;
}

int TSKA_msgqSendAck(TSKA_MsgHndl *msg, int ackRetVal)
{
    int status;

    msg->status = ackRetVal;

    status = TSKA_msgqSend(msg->pFrom, msg, TSKA_TIMEOUT_FOREVER);

    return status;
}

int TSKA_msgqFreeMsgHndl(TSKA_MsgHndl *msg)
{
    int32_t status = TSKA_EFAIL;

    /* TSKA_memFree(msg); */
    if (msg->pFrom) {
        if (msg->isMalloc) {
            status = TSKA_quePut(&msg->pFrom->memQue, (long)msg, TSKA_TIMEOUT_NONE);
            TSKA_assertSuccess(status);
        }
    }

    return TSKA_SOK;
}

int TSKA_msgqPeek(TSKA_MsgqHndl *hndl, TSKA_MsgHndl **msg)
{
    int status = TSKA_EFAIL;
    pthread_mutex_lock(&hndl->lock);
    if(hndl->count > 0 ) {
        if(msg!=NULL) {
            *msg = hndl->queue[hndl->curRd];
            status = TSKA_SOK;
        }
    }  
    pthread_mutex_unlock(&hndl->lock);

    return status;
}

int TSKA_msgqCount(TSKA_MsgqHndl *hndl, int* count)
{
    int status = TSKA_EFAIL;

    if(count!= NULL) {
        *count  = hndl->count;
        status = TSKA_SOK;
    }

    return status;
}
