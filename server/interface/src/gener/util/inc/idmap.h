#ifndef __ID_MAP_HH__
#define __ID_MAP_HH__

#include <stdint.h>


typedef enum {
    LogIdMap_None = 0,  //关闭日志输出 
    LogIdMap_Urgent,    //必须打的
    LogIdMap_Fatal,     //致使级
    LogIdMap_Error,     //错误级
    LogIdMap_Warning,   //告警级
    LogIdMap_Info,      //业务级
    LogIdMap_Debug,     //调试级
    LogIdMap_Trace,     //跟踪级
    LogIdMap_Detail,    //详细级
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
