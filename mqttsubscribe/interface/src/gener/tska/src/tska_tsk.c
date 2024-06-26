#include <tska_tsk.h>
#include <tska_debug.h>

void *TSKA_fxnThrMain(void *pPrm)
{
    TSKA_TskHndl *pPrc;

    pPrc = (TSKA_TskHndl *)pPrm;
    TSKA_assert(pPrc != NULL);

    TSKTHRDSETPROCNAME(pPrc->tskHndlName);

    TSKA_assert(pPrc->fncMain != NULL);
    TSKA_assert(pPrc->appData != NULL);
    pPrc->fncMain(pPrc->appData, NULL);

    return NULL;
}

int TSKA_fxnCreate(TSKA_TskHndl *pPrc, TSKA_TskFncMain fncMain, uint32_t tskPri, uint32_t tskStackSize, uint32_t initState,
                  void *appData)
{
    pPrc->curState      = initState;
    pPrc->fncMain       = fncMain;
    pPrc->appData       = appData;

    TSKA_assert(pPrc->fncMain != NULL);

    TSKA_assertSuccess( TSKA_thrCreate(&pPrc->thrHndl, TSKA_fxnThrMain, tskPri, tskStackSize,  pPrc) );

    return TSKA_SOK;
}

void *TSKA_tskThrMain(void *pPrm)
{
  TSKA_MsgHndl *pMsg;
  TSKA_TskHndl *pPrc;

  pPrc = (TSKA_TskHndl *)pPrm;
  TSKA_assert(pPrc!=NULL);

  //TSKA_printf("TSKA_tskThrMain thread TID %d, PID %d\n", gettid(), getpid());
  TSKTHRDSETPROCNAME(pPrc->tskHndlName);

  while(1) {

    TSKA_assertSuccess( TSKA_tskWaitMsg(pPrc, &pMsg) );

    TSKA_assert(pPrc->fncMain!=NULL);
    pPrc->fncMain(pPrc, pMsg);
  }

  return NULL;
}

int TSKA_tskCreate(TSKA_TskHndl *pPrc, TSKA_TskFncMain fncMain, uint32_t tskPri, uint32_t tskStackSize, uint32_t initState,
                  void *appData)
{
    pPrc->curState      = initState;
    pPrc->fncMain       = fncMain;
    pPrc->appData       = appData;

    TSKA_assert(pPrc->fncMain      != NULL);

    TSKA_mbxCreate(&pPrc->mbxHndl);

    TSKA_assertSuccess( TSKA_thrCreate(&pPrc->thrHndl, TSKA_tskThrMain, tskPri, tskStackSize,  pPrc) );

    return TSKA_SOK;
}

int TSKA_tskDelete(TSKA_TskHndl *pPrc)
{
  TSKA_thrDelete(&pPrc->thrHndl);
  TSKA_mbxDelete(&pPrc->mbxHndl);
  TSKA_printf("%s:In progress...",__func__);

  pPrc->curState      = 0;
  pPrc->fncMain       = NULL;

  return TSKA_SOK;
}

int TSKA_tskSendMsg(TSKA_TskHndl *pPrcTo, TSKA_TskHndl *pPrcFrom, 
        uint32_t linkId, uint16_t cmd, void *pPrm, uint16_t flags)
{
  int retVal;
  TSKA_MbxHndl *pMbxFrom;

  TSKA_assert(pPrcTo!=NULL);

  if(pPrcFrom==NULL) {
    pMbxFrom = NULL;
  } else {
    pMbxFrom = &pPrcFrom->mbxHndl;
  }

  retVal = TSKA_mbxSendMsg(&pPrcTo->mbxHndl, pMbxFrom, linkId, cmd, pPrm, flags);

  return retVal;
}

int TSKA_tskBroadcastMsg(TSKA_TskHndl *pPrcToList[], TSKA_TskHndl *pPrcFrom,
        uint32_t linkId, uint16_t cmd, void *pPrm, uint16_t flags)
{
  int retVal;

  TSKA_MbxHndl *pMbxToList[TSKA_MBX_BROADCAST_MAX];
  uint32_t i, numMsg;
  TSKA_MbxHndl *pMbxFrom;

  TSKA_assert(pPrcToList!=NULL);

  if(pPrcFrom==NULL) {
    pMbxFrom = NULL;
  } else {
    pMbxFrom = &pPrcFrom->mbxHndl;
  }

  for(i=0; i<TSKA_MBX_BROADCAST_MAX; i++)
  {
    pMbxToList[i]=NULL;
  }

  numMsg = 0;
  while(pPrcToList[numMsg]!=NULL) {
    pMbxToList[numMsg] = &pPrcToList[numMsg]->mbxHndl;
    numMsg++;
    if(numMsg>=TSKA_MBX_BROADCAST_MAX) {
      // cannot broadcast to more than TSKA_mbx_BROADCAST_MAX mailboxes
      TSKA_assert(0);
    }
  }

  if(numMsg == 0) {
     // no mailboxes in 'to' mailbox list
     return TSKA_SOK;
  }

  retVal = TSKA_mbxBroadcastMsg(&pMbxToList[0], pMbxFrom, linkId, cmd, pPrm, flags);

  return retVal;
}


int TSKA_tskWaitMsg(TSKA_TskHndl *pPrc, TSKA_MsgHndl **pMsg)
{
  int retVal;

  retVal = TSKA_mbxWaitMsg(&pPrc->mbxHndl, pMsg);

  return retVal;
}

int TSKA_tskCheckMsg(TSKA_TskHndl *pPrc, TSKA_MsgHndl **pMsg)
{
  int retVal;

  retVal = TSKA_mbxCheckMsg(&pPrc->mbxHndl, pMsg);

  return retVal;
}

int TSKA_tskAckOrFreeMsg(TSKA_MsgHndl *pMsg, int ackRetVal)
{
  int retVal;

  retVal = TSKA_mbxAckOrFreeMsg(pMsg, ackRetVal);

  return retVal;
}

int TSKA_tskFlushMsg(TSKA_TskHndl *pPrc)
{
  int retVal;

  retVal = TSKA_mbxFlush(&pPrc->mbxHndl);

  return retVal;
}

int TSKA_tskWaitCmd(TSKA_TskHndl *pPrc, TSKA_MsgHndl **pMsg, uint16_t waitCmd)
{
  int retVal;

  retVal = TSKA_mbxWaitCmd(&pPrc->mbxHndl, pMsg, waitCmd);

  return retVal;
}

int TSKA_tskGetCount(TSKA_TskHndl *pPrc, int * count)
{
  int retVal;

  retVal = TSKA_mbxGetCount(&pPrc->mbxHndl, count);

  return retVal;
}

int TSKA_tskSetState(TSKA_TskHndl *pPrc, uint32_t curState)
{
  pPrc->curState = curState;
  return TSKA_SOK;
}

uint32_t TSKA_tskGetState(TSKA_TskHndl *pPrc)
{
  return pPrc->curState;
}

int32_t TSKA_tskFlushSpecifiedMsg(TSKA_TskHndl * pHndl, uint32_t *flushCmdId, uint32_t numCmds)
{
	TSKA_MsgHndl *pMsg;
	uint32_t i, cmd;
	int32_t status;
	int32_t done;
	
	do {
		status = TSKA_tskPeekMsg(pHndl, &pMsg);
		if (status != TSKA_SOK)
			break;
			
		cmd = TSKA_msgGetCmd(pMsg);

		done = 1;

		// search the commands that need to be flushed
		for(i = 0; i < numCmds; i++)
		{
			if(cmd == flushCmdId[i])
			{
				status =  TSKA_mbxCheckMsg(&pHndl->mbxHndl, &pMsg);
				if (TSKA_SOK != status)
                    break;
				TSKA_tskAckOrFreeMsg(pMsg, status);

				done = 0;
				break;
			}
		}
		
	} while(!done);

	return TSKA_SOK;
}

