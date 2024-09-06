#include <tska_thr.h>

int TSKA_thrCreate(TSKA_ThrHndl *hndl, TSKA_ThrEntryFunc entryFunc, uint32_t pri, uint32_t stackSize, void *prm)
{
  int status=TSKA_SOK;
  pthread_attr_t thread_attr;
#ifndef MAKE_CHIP_INTEL
  struct sched_param schedprm;
#endif

  // initialize thread attributes structure
  status = pthread_attr_init(&thread_attr);
  
  if(status!=TSKA_SOK) {
    TSKA_ERROR("TSKA_thrCreate() - Could not initialize thread attributes\n");
    return status;
  }
  
  if(stackSize!=TSKA_THR_STACK_SIZE_DEFAULT)
    pthread_attr_setstacksize(&thread_attr, stackSize);

  status |= pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
#ifndef MAKE_CHIP_INTEL
	status |= pthread_attr_setschedpolicy(&thread_attr, SCHED_RR);
  
	if(pri>TSKA_THR_PRI_MAX)   
	  pri=TSKA_THR_PRI_MAX;
	else 
	if(pri<TSKA_THR_PRI_MIN)   
	  pri=TSKA_THR_PRI_MIN;
	  
	schedprm.sched_priority = pri;
	status |= pthread_attr_setschedparam(&thread_attr, &schedprm);
  
	if(status != TSKA_SOK) {
	  TSKA_ERROR("TSKA_thrCreate() - Could not initialize thread attributes\n");
	  goto error_exit;
	}
#endif

  status = pthread_create(&hndl->hndl, &thread_attr, entryFunc, prm);
  
  if(status != TSKA_SOK) {
    TSKA_ERROR("TSKA_thrCreate() - Could not create thread [%d]errno[%s]\n", status, strerror(status));
    TSKA_assert(status == TSKA_SOK);
  }
  
#ifndef MAKE_CHIP_INTEL
error_exit:
#endif

  pthread_attr_destroy(&thread_attr);

  return status;
}

int TSKA_thrJoin(TSKA_ThrHndl *hndl)
{
  int status=TSKA_SOK;
  void *returnVal;
    
  status = pthread_join(hndl->hndl, &returnVal); 
  
  return status;    
}

int TSKA_thrDelete(TSKA_ThrHndl *hndl)
{
  int status = TSKA_SOK;

  /* if(&hndl->hndl != NULL) */
  /* { */
      status = pthread_cancel(hndl->hndl); 
      status = TSKA_thrJoin(hndl);
  /* } */
  
  return status;    
}

int TSKA_thrChangePri(TSKA_ThrHndl *hndl, uint32_t pri)
{
  int status = TSKA_SOK;
  struct sched_param schedprm;  

  if(pri>TSKA_THR_PRI_MAX)   
    pri=TSKA_THR_PRI_MAX;
  else 
  if(pri<TSKA_THR_PRI_MIN)   
    pri=TSKA_THR_PRI_MIN;
  
  schedprm.sched_priority = pri;  
  status |= pthread_setschedparam(hndl->hndl, SCHED_RR, &schedprm);
  
  return status;
}

int TSKA_thrExit(void *returnVal)
{
  pthread_exit(returnVal);
  return TSKA_SOK;
}

