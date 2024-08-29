#ifndef _TSKA_TSK_H_
#define _TSKA_TSK_H_

#include <tska_thr.h>
#include <tska_mbx.h>

struct TSKA_TskHndl;

typedef int (*TSKA_TskFncMain)(struct TSKA_TskHndl *pTsk, TSKA_MsgHndl *pMsg);

#define TSKA_TSK_NAME_LEN	16
/**
  \brief Task Handle
*/
typedef struct TSKA_TskHndl {

  TSKA_MbxHndl mbxHndl;      ///< Mailbox handle
  TSKA_ThrHndl thrHndl;      ///< OS thread handle

  uint32_t curState;          ///< Task state as defined by user
  TSKA_TskFncMain fncMain;   ///< Task Main, this function is entered when a message is received by the process
  
  void *appData;
  char tskHndlName[TSKA_TSK_NAME_LEN];    
} TSKA_TskHndl;

#if defined (__cplusplus)
extern "C" {
#endif

int TSKA_fxnCreate(TSKA_TskHndl *pTsk, TSKA_TskFncMain fncMain, uint32_t tskPri, uint32_t tskStackSize, uint32_t initState,
                  void *appData);
int TSKA_tskCreate(TSKA_TskHndl *pTsk, TSKA_TskFncMain fncMain, uint32_t tskPri, uint32_t tskStackSize, uint32_t initState,
                  void *appData);
int TSKA_tskDelete(TSKA_TskHndl *pTsk);
int TSKA_tskSendMsg(TSKA_TskHndl *pTskTo, TSKA_TskHndl *pTskFrom, 
        uint32_t linkId, uint16_t cmd, void *pPrm, uint16_t flags);
int TSKA_tskBroadcastMsg(TSKA_TskHndl *pTskToList[], TSKA_TskHndl *pTskFrom, uint32_t linkId, uint16_t cmd, void *pPrm, uint16_t flags);
int TSKA_tskAckOrFreeMsg(TSKA_MsgHndl *pMsg, int ackRetVal);
int TSKA_tskWaitMsg(TSKA_TskHndl *pTsk, TSKA_MsgHndl **pMsg);
int TSKA_tskCheckMsg(TSKA_TskHndl *pTsk, TSKA_MsgHndl **pMsg);
int TSKA_tskWaitCmd(TSKA_TskHndl *pTsk, TSKA_MsgHndl **pMsg, uint16_t waitCmd);
int TSKA_tskFlushMsg(TSKA_TskHndl *pTsk);

int    TSKA_tskGetCount(TSKA_TskHndl *pPrc, int * count);
int    TSKA_tskSetState(TSKA_TskHndl *pPrc, uint32_t curState);
uint32_t TSKA_tskGetState(TSKA_TskHndl *pPrc);

static inline int32_t
TSKA_tskPeekMsg(TSKA_TskHndl * pHndl, TSKA_MsgHndl ** pMsg)
{
    return TSKA_mbxPeekMsg(&pHndl->mbxHndl, pMsg);
}

static inline int32_t
TSKA_tskPeekHspedMsg(TSKA_TskHndl * pHndl, TSKA_MsgHndl ** pMsg)
{
    return TSKA_mbxPeekHspedMsg(&pHndl->mbxHndl, pMsg);
}

int32_t TSKA_tskFlushSpecifiedMsg(TSKA_TskHndl * pHndl, uint32_t *flushCmdId, uint32_t numCmds);

#if defined (__cplusplus)
}
#endif

#endif /* _TSKA_TSK_H_ */




