#ifndef __ID_MAP_HH__
#define __ID_MAP_HH__

#include <stdint.h>


typedef enum {
    LogIdMap_None = 0,  //�ر���־��� 
    LogIdMap_Urgent,    //������
    LogIdMap_Fatal,     //��ʹ��
    LogIdMap_Error,     //����
    LogIdMap_Warning,   //�澯��
    LogIdMap_Info,      //ҵ��
    LogIdMap_Debug,     //���Լ�
    LogIdMap_Trace,     //���ټ�
    LogIdMap_Detail,    //��ϸ��
    LogIdMap_Cnt
} LogIdMap;

typedef int32_t (*idMapPrint)(void *userHndl, const char *);


typedef struct {
    int32_t idNum;
    idMapPrint print;
    void *priv;
} IdMapConfig;

#ifdef __cplusplus
extern "C" {
#endif

int32_t idMapPut(void *oObj, int32_t id);
int32_t idMapGet(void *oObj, int32_t key);
int32_t idMapSearch(void *oObj, int32_t key);
void *idMapInit(IdMapConfig *config);
void idMapExit(void *oObj);

void SetIdmapLevel(idMapPrint print, void *priv, LogIdMap level);

#ifdef __cplusplus
}
#endif 
#endif
