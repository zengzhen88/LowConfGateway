#include <tska_mbx.h>
#include <tska_msgq.h>

int TSKA_mbxCreate(TSKA_MbxHndl *pMbxHndl)
{
    int status=TSKA_SOK;

    memset(pMbxHndl, 0x0, sizeof(TSKA_MbxHndl));

    status |= TSKA_msgqCreate(&pMbxHndl->rcvMbx);
    status |= TSKA_msgqCreate(&pMbxHndl->ackMbx);

    if(status!=TSKA_SOK)
        TSKA_ERROR("TSKA_mbxCreate() = %d \r\n", status);

    return status;
}

int TSKA_mbxDelete(TSKA_MbxHndl *pMbxHndl)
{
    TSKA_msgqDelete(&pMbxHndl->rcvMbx);
    TSKA_msgqDelete(&pMbxHndl->ackMbx);

    return TSKA_SOK;
}

int TSKA_mbxFreeMsg(TSKA_MsgHndl *pMsg)
{
    if(pMsg->flags & TSKA_MBX_FREE_PRM) {
        if(pMsg->pPrm!=NULL) {
            TSKA_memFree(pMsg->pPrm);
        }
    }

    TSKA_assertSuccess( TSKA_msgqFreeMsgHndl(pMsg) );

    return TSKA_SOK;
}

/*
 * int TSKA_mbxSendMsgEx(TSKA_MbxHndl *pMbxTo, TSKA_MbxHndl *pMbxFrom,
 *         uint32_t linkId, uint32_t cmd, void *pPrm[], int32_t numBufs)
 * {
 *     uint32_t flags = 0;
 *     TSKA_MsgqHndl *ackMbx;
 * 
 *     TSKA_assert(pMbxTo!=NULL);
 * 
 *     if(pMbxFrom==NULL) {
 *         if(flags & TSKA_MBX_WAIT_ACK) {
 *             // if from mail box is NULL, then cannot wait for ACK
 *             TSKA_assert(0);
 *         }
 *         ackMbx = NULL;
 *     } else {
 *         ackMbx = &pMbxFrom->ackMbx;
 *     }
 * 
 *     return TSKA_msgqSendMsgEx(&pMbxTo->rcvMbx, ackMbx, linkId, cmd, pPrm, numBufs, flags);
 * }
 */

int TSKA_mbxSendMsg(TSKA_MbxHndl *pMbxTo, TSKA_MbxHndl *pMbxFrom,
        uint32_t linkId, uint32_t cmd, void *pPrm, uint32_t flags)
{
    TSKA_MsgHndl *pSentMsg, *pRcvMsg;
    int32_t waitAck;
    int32_t retVal=TSKA_SOK;
    TSKA_MsgqHndl *ackMbx;

    TSKA_assert(pMbxTo!=NULL);

    if(pMbxFrom==NULL) {
        if(flags & TSKA_MBX_WAIT_ACK) {
            // if from mail box is NULL, then cannot wait for ACK
            TSKA_assert(0);
        }
        ackMbx = NULL;
    } else {
        ackMbx = &pMbxFrom->ackMbx;
    }

    /* retVal = TSKA_msgqSendMsg(&pMbxTo->rcvMbx, ackMbx, linkId, cmd, pPrm, flags, &pSentMsg); */
    retVal = TSKA_msgqSendLspedMsg(&pMbxTo->rcvMbx, ackMbx, linkId, cmd, pPrm, flags, &pSentMsg);
    if (TSKA_SOK == retVal) {
        if( (flags & TSKA_MBX_WAIT_ACK) && ackMbx != NULL ) {

            waitAck = 1;

            do {
                // wait for ACK
                retVal = TSKA_msgqRecvMsg(ackMbx, &pRcvMsg, TSKA_TIMEOUT_FOREVER);
                TSKA_assertSuccess(retVal);

                if(pRcvMsg==pSentMsg) {
                    // ACK received for sent MSG
                    waitAck = 0;
                    retVal  = TSKA_msgGetAckStatus(pRcvMsg);
                }
                else {
                    TSKA_printf("TSKA_MBX:MSG RECEIVED OUT OF ORDER.SERIOUS ERROR!!! Expected[%d]:Received[%d]",pSentMsg->cmd,pRcvMsg->cmd);
                    TSKA_assert(0);
                }

                TSKA_mbxFreeMsg(pRcvMsg);

            } while(waitAck);
        }
    }

    return retVal;
}

int TSKA_mbxSendMsgNoAck(TSKA_MbxHndl *pMbxTo, TSKA_MbxHndl *pMbxFrom,
        uint32_t linkId, uint32_t cmd, void *pPrm, uint32_t flags)
{
    TSKA_MsgHndl *pSentMsg;//, *pRcvMsg;
    /* int32_t waitAck; */
    /* int32_t retVal=TSKA_SOK; */
    TSKA_MsgqHndl *ackMbx;

    TSKA_assert(pMbxTo!=NULL);

    if(pMbxFrom==NULL) {
        if(flags & TSKA_MBX_WAIT_ACK) {
            // if from mail box is NULL, then cannot wait for ACK
            TSKA_assert(0);
        }
        ackMbx = NULL;
    } else {
        ackMbx = &pMbxFrom->ackMbx;
    }

    return TSKA_msgqSendMsg(&pMbxTo->rcvMbx, ackMbx, linkId, cmd, pPrm, flags, &pSentMsg);
}

int TSKA_mbxBroadcastMsg(TSKA_MbxHndl *pMbxToList[], TSKA_MbxHndl *pMbxFrom, 
        uint32_t linkId, uint32_t cmd, void *pPrm, uint32_t flags)
{
    TSKA_MsgHndl *pSentMsgList[TSKA_MBX_BROADCAST_MAX];
    int32_t  ackList[TSKA_MBX_BROADCAST_MAX];
    TSKA_MsgHndl *pRcvMsg;
    int32_t waitAck, unknownAck;
    uint32_t i, numMsg;
    int32_t retVal=TSKA_SOK, ackRetVal = TSKA_SOK;
    TSKA_MsgqHndl *ackMbx;

    TSKA_assert(pMbxToList!=NULL);

    if(pMbxFrom==NULL) {
        if(flags & TSKA_MBX_WAIT_ACK) {
            // if from mail box is NULL, then cannot wait for ACK
            TSKA_assert(0);
        }
        ackMbx = NULL;
    } else {
        ackMbx = &pMbxFrom->ackMbx;
    }

    // count number of mailboxes to boadcast to
    numMsg=0;
    while(pMbxToList[numMsg]!=NULL) {
        numMsg++;
        if(numMsg>=TSKA_MBX_BROADCAST_MAX) {
            // cannot broadcast to more than TSKA_MBX_BROADCAST_MAX mailboxes
            TSKA_assert(0);
        }
    }

    if(numMsg == 0) {
        // no mailboxes in 'to' mailbox list
        return TSKA_SOK;
    }

    // Cannot broadcast with malloc'ed prm and not wait for ACK
    if(flags & TSKA_MBX_FREE_PRM) {
        if( !(flags & TSKA_MBX_WAIT_ACK) && numMsg > 1) {
            TSKA_assert(0);
        }
    }

    // reset sent MSG list and ACK list
    for(i=0; i<TSKA_MBX_BROADCAST_MAX; i++) {
        ackList[i]  = 0;
        pSentMsgList[i] = NULL;
    }

    // send MSG to all mailboxes
    for(i=0; i<numMsg; i++) {
        retVal = TSKA_msgqSendMsg(&pMbxToList[i]->rcvMbx, ackMbx,
                linkId, cmd, pPrm, flags, &pSentMsgList[i]);
        TSKA_assertSuccess(retVal);
    }

    if((flags & TSKA_MBX_WAIT_ACK) && ackMbx!=NULL ) {
        // wait for ACKs
        do {
            // wait ACK
            retVal = TSKA_msgqRecvMsg(ackMbx, &pRcvMsg, TSKA_TIMEOUT_FOREVER);
            TSKA_assertSuccess(retVal);

            unknownAck=1;

            // mark ACK as received for sent MSG
            for(i=0; i<numMsg; i++) {
                if(pRcvMsg == pSentMsgList[i] ) {
                    ackList[i]=1;
                    unknownAck=0;
                    if(ackRetVal==TSKA_SOK) {
                        ackRetVal  = TSKA_msgGetAckStatus(pRcvMsg);
                    }
                    break;
                }
            }

            // check if all ACKs received
            waitAck = 0;
            for(i=0; i<numMsg; i++) {
                if(ackList[i]==0 ) {
                    waitAck = 1;
                    break;
                }
            }

            if(unknownAck) {
                // ACK received is for some old message, hence free MSG and prm
                TSKA_mbxFreeMsg(pRcvMsg);
            } else {
                // only free MSG now, free prm after all ACKs are received
                TSKA_assertSuccess( TSKA_msgqFreeMsgHndl(pRcvMsg) );
            }

        } while(waitAck);

        if(flags & TSKA_MBX_FREE_PRM) {
            if(pPrm!=NULL) {
                TSKA_memFree(pPrm);
            }
        }

        retVal = ackRetVal;
    }

    return retVal;
}


int TSKA_mbxWaitMsg(TSKA_MbxHndl *pMbxHndl, TSKA_MsgHndl **pMsg)
{
    int32_t retVal;

    retVal = TSKA_msgqRecvMsg(&pMbxHndl->rcvMbx, pMsg, TSKA_TIMEOUT_FOREVER);
    TSKA_assertSuccess(retVal);

    return retVal;
}

int TSKA_mbxCheckMsg(TSKA_MbxHndl *pMbxHndl, TSKA_MsgHndl **pMsg)
{
    int32_t retVal;

    retVal = TSKA_msgqRecvMsg(&pMbxHndl->rcvMbx, pMsg, TSKA_TIMEOUT_NONE);

    return retVal;
}

int TSKA_mbxGetCount(TSKA_MbxHndl *pMbxHndl, int32_t* count)
{
    int32_t retVal;

    retVal = TSKA_msgqCount(&pMbxHndl->rcvMbx, count);

    return retVal;
}

int TSKA_mbxAckOrFreeMsg(TSKA_MsgHndl *pMsg, int32_t ackRetVal)
{
    int32_t retVal=TSKA_SOK;

    if(pMsg==NULL)
        return TSKA_EFAIL;

    if(pMsg->flags & TSKA_MBX_WAIT_ACK) {
        // ACK message
        retVal = TSKA_msgqSendAck(pMsg, ackRetVal);
        TSKA_assertSuccess(retVal);
        if (TSKA_SOK != retVal) {
            TSKA_mbxFreeMsg(pMsg);
        }
    } else {
        // FREE message and prm
        TSKA_mbxFreeMsg(pMsg);
    }

    return retVal;
}

int TSKA_mbxFlush(TSKA_MbxHndl *pMbxHndl)
{
    int32_t retVal;
    TSKA_MsgHndl *pMsg;

    // flush receive mailbox
    do {
        retVal = TSKA_mbxCheckMsg(pMbxHndl, &pMsg);
        if(retVal==TSKA_SOK) {
            TSKA_mbxAckOrFreeMsg(pMsg, TSKA_SOK);
        }
    }while(retVal==TSKA_SOK);

    // flush ACK mailbox
    do {
        retVal = TSKA_msgqRecvMsg(&pMbxHndl->ackMbx, &pMsg, TSKA_TIMEOUT_NONE);
        if(retVal==TSKA_SOK) {
            TSKA_mbxFreeMsg(pMsg);
        }
    } while(retVal==TSKA_SOK);

    return retVal;
}


int32_t TSKA_mbxWaitCmd(TSKA_MbxHndl *pMbxHndl, TSKA_MsgHndl **pMsg, uint16_t waitCmd)
{
    TSKA_MsgHndl *pRcvMsg;

    while(1) {
        TSKA_mbxWaitMsg(pMbxHndl, &pRcvMsg);
        if(TSKA_msgGetCmd(pRcvMsg)==waitCmd)
            break;
        TSKA_mbxAckOrFreeMsg(pRcvMsg, TSKA_SOK);
    }

    if(pMsg==NULL) {
        TSKA_mbxAckOrFreeMsg(pRcvMsg, TSKA_SOK);
    } else {
        *pMsg = pRcvMsg;
    }

    return TSKA_SOK;
}

int TSKA_mbxPeekMsg(TSKA_MbxHndl * pMbxHndl, TSKA_MsgHndl ** pMsg)
{
    int32_t retVal;

    /* wait for message to arrive */
    //retVal = TSKA_quePeek(&pMbxHndl->recvQue, (Ptr *) pMsg);
    retVal = TSKA_msgqPeek(&pMbxHndl->rcvMbx, /* (Ptr *)  */pMsg);

    return retVal;
}

