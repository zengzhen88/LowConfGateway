

#ifndef _TSKA_MBX_H_
#define _TSKA_MBX_H_

#include <tska_msgq.h>

#define TSKA_MBX_WAIT_ACK      0x0002   ///< Message Flag : Wait for ACK
#define TSKA_MBX_FREE_PRM      0x0004   ///< Message Flag : Message parameters are malloc'ed and need to be free'ed

#define TSKA_MBX_BROADCAST_MAX     10   ///< Max message queues/PRC's that can be broadcast to

/**
  \brief Mailbox handle
*/
typedef struct {

  TSKA_MsgqHndl rcvMbx;    ///< Receive mailbox
  TSKA_MsgqHndl ackMbx;    ///< ACK mailbox

} TSKA_MbxHndl;

#if defined (__cplusplus)
extern "C" {
#endif

int TSKA_mbxCreate(TSKA_MbxHndl *pHndl);
int TSKA_mbxDelete(TSKA_MbxHndl *pHndl);
int TSKA_mbxSendMsg(TSKA_MbxHndl *pTo, TSKA_MbxHndl *pFrom,
        uint32_t linkId, uint32_t cmd, void *pPrm, uint32_t flags);
int TSKA_mbxSendMsgNoAck(TSKA_MbxHndl *pMbxTo, TSKA_MbxHndl *pMbxFrom,
        uint32_t linkId, uint32_t cmd, void *pPrm, uint32_t flags);
/*
 * int TSKA_mbxSendMsgEx(TSKA_MbxHndl *pMbxTo, TSKA_MbxHndl *pMbxFrom,
 *         uint32_t linkId, uint32_t cmd, void *pPrm[], int32_t numBufs);
 */
int TSKA_mbxSendNobMsg(TSKA_MbxHndl *pTo, TSKA_MbxHndl *pFrom, 
        uint32_t linkId, uint32_t cmd, void *pPrm, uint32_t flags);
int TSKA_mbxSendLspedMsg(TSKA_MbxHndl *pTo, TSKA_MbxHndl *pFrom,
        uint32_t linkId, uint32_t cmd, void *pPrm, uint32_t flags);
int TSKA_mbxBroadcastMsg(TSKA_MbxHndl *pToList[], TSKA_MbxHndl *pFrom, 
        uint32_t linkId, uint32_t cmd, void *pPrm, uint32_t flags);
int TSKA_mbxAckOrFreeMsg(TSKA_MsgHndl *pMsg, int ackRetVal);
int TSKA_mbxWaitMsg(TSKA_MbxHndl *pHndl, TSKA_MsgHndl **pMsg);
int TSKA_mbxCheckMsg(TSKA_MbxHndl *pHndl, TSKA_MsgHndl **pMsg);
int TSKA_mbxWaitCmd(TSKA_MbxHndl *pHndl, TSKA_MsgHndl **pMsg, uint16_t waitCmd);
int TSKA_mbxFlush(TSKA_MbxHndl *pHndl);
int TSKA_mbxPeekMsg(TSKA_MbxHndl * pMbxHndl, TSKA_MsgHndl ** pMsg);
int TSKA_mbxPeekHspedMsg(TSKA_MbxHndl * pMbxHndl, TSKA_MsgHndl ** pMsg);

int TSKA_mbxGetCount(TSKA_MbxHndl *pMbxHndl, int* count);

#if defined (__cplusplus)
}
#endif

#endif /* _TSKA_MBX_H_ */



