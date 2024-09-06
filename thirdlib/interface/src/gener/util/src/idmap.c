#include <stdint.h>
#include <tska.h>
#include <stdarg.h>
#include "idmap.h"
#include "rbtree.h"

static void *LogIdMapPriv               = NULL;
static idMapPrint LogIdMapPrint         = NULL;
static LogIdMap LogIdMapLevel           = LogIdMap_Info;

static int32_t IdMapProcPrintf(LogIdMap level, 
        const char *func, int32_t line, const char *format, ...) {
    char logBuf[1024];
    va_list args;
    int32_t funcLine            = 0;

    if (NULL == LogIdMapPriv 
            || NULL == LogIdMapPrint) {
        return TSKA_EFAIL;
    }

    if (level > LogIdMapLevel) return -1;

    snprintf (logBuf, sizeof(logBuf), "[%s][%d]", func, line);
    funcLine = strlen(logBuf);

    /*va_list*/
    va_start(args, format);
    vsnprintf(&logBuf[funcLine], sizeof(logBuf) - funcLine, format, args);
    va_end(args);
    /*va_list*/

    if (LogIdMapPrint) {
        return LogIdMapPrint(LogIdMapPriv, logBuf);
    }

    return TSKA_EFAIL;
}

#define LogPrintf(level, ...)\
    IdMapProcPrintf(level, __FUNCTION__, __LINE__, __VA_ARGS__);


typedef struct {
    int32_t index;
    int32_t key;
} KeyD;

typedef struct {
    int32_t idNum;

    rbtree_manages *gRbChanNode;
    KeyD *key;

    struct rb_roots gRbChanFullRoots;
    struct rb_roots gRbChanEmptyRoots;
} IdMapObj;

static int32_t idMap_key_compute(void *v_key, void *u_key) {
    KeyD * key = ((KeyD *)v_key);
    KeyD * _key = ((KeyD *)u_key);

    LogPrintf (LogIdMap_Detail, "v_key:%p u_key:%p\n", v_key, u_key);
    LogPrintf (LogIdMap_Detail, "key->key:%d _key->key:%d\n", 
            key->key, _key->key);
    LogPrintf (LogIdMap_Detail, "key->index:%d _key->index:%d\n", 
            key->index, _key->index);
    if (key->key > _key->key) return 1;
    else if (key->key < _key->key) return -1;
    else return 0;
}

static int32_t idMap_index_compute(void *v_key, void *u_key) {
    KeyD * key = ((KeyD *)v_key);
    KeyD * _key = ((KeyD *)u_key);

    LogPrintf (LogIdMap_Detail, "v_key:%p u_key:%p\n", v_key, u_key);
    LogPrintf (LogIdMap_Detail, "key->key:%d _key->key:%d\n", 
            key->key, _key->key);
    LogPrintf (LogIdMap_Detail, "key->index:%d _key->index:%d\n", 
            key->index, _key->index);

    if (key->index > _key->index) return 1;
    else if (key->index < _key->index) return -1;
    else return 0;
}

void SetIdmapLevel(idMapPrint print, void *priv, LogIdMap level) {
    if (NULL == LogIdMapPriv 
            || NULL == LogIdMapPrint) {
        LogIdMapPriv        = priv;
        LogIdMapPrint       = print;
        LogIdMapLevel       = level;
    }
}

void *idMapInit(IdMapConfig *config) {
    int32_t index               = 0;
    IdMapObj *pObj              = NULL;
    struct rb_roots *root       = NULL;
    rbtree_manages *arrayNode   = NULL;


    pObj = (IdMapObj *) malloc (sizeof(IdMapObj));
    if (NULL == pObj) {
        return NULL;
    }

    pObj->idNum                 = config->idNum;
    pObj->gRbChanFullRoots      = RB_ROOTS;
    pObj->gRbChanEmptyRoots     = RB_ROOTS;

    pObj->gRbChanNode = (rbtree_manages *) malloc (config->idNum * sizeof(rbtree_manages));
    if (NULL == pObj->gRbChanNode) {
        free(pObj);
        return NULL;
    }

    pObj->key   = (KeyD *) malloc (config->idNum * sizeof(KeyD));
    if (NULL == pObj->key) {
        free(pObj->gRbChanNode);
        free(pObj);
        return NULL;
    }

    root = &pObj->gRbChanEmptyRoots;
    for (index = 0; index < pObj->idNum; index++) {
        arrayNode               = &pObj->gRbChanNode[index];
        pObj->key[index].index  = index;
        arrayNode->key          = (void *)(&pObj->key[index]);
        inserts((rbtree_manages *)arrayNode, root, idMap_index_compute);
    }

    return pObj;
}

void idMapExit(void *oObj) {
    IdMapObj *pObj              = NULL;

    pObj    = (IdMapObj *)oObj;

    if (pObj) {
        free(pObj->key);
        free(pObj->gRbChanNode);
        free(pObj);
    }
}

int32_t idMapGet(void *oObj, int32_t key) {
    int32_t id                  = 0;
    struct rb_roots *eRoot      = NULL;
    struct rb_roots *fRoot      = NULL;
    struct rb_nodes *rbNode     = NULL;
    rbtree_manages *nd          = NULL;
    IdMapObj *pObj              = oObj;

    eRoot       = &pObj->gRbChanEmptyRoots;
    fRoot       = &pObj->gRbChanFullRoots;

    rbNode = eRoot->rb_node;
    if (NULL == rbNode) {
        return TSKA_EFAIL;
    }

    nd = (rbtree_manages *)rb_entrys(rbNode, rbtree_manages, rb);

    //从空红黑树中申请
    erases(nd, eRoot);

    ((KeyD *)nd->key)->key = key;
    id = ((KeyD *)nd->key)->index;

    //存储到满红黑树
    inserts(nd, fRoot, idMap_key_compute);

    return id;
}

int32_t idMapPut(void *oObj, int32_t id) {
    KeyD node;
    rbtree_manages *rbNode      = NULL;
    IdMapObj *pObj              = oObj;

    node.key   = id;

    rbNode = (rbtree_manages *)searchs((void *)(&node), &pObj->gRbChanFullRoots, idMap_key_compute);
    if (NULL == rbNode) {
        return TSKA_EFAIL;
    }

    erases(rbNode, &pObj->gRbChanFullRoots);

    inserts(rbNode, &pObj->gRbChanEmptyRoots, idMap_index_compute);

    return TSKA_SOK;
}

int32_t idMapSearch(void *oObj, int32_t key) {
    KeyD node;
    rbtree_manages *rbNode      = NULL;
    IdMapObj *pObj              = oObj;

    node.key   = key;

    rbNode = (rbtree_manages *)searchs((void *)(&node), &pObj->gRbChanFullRoots, idMap_key_compute);
    if (NULL == rbNode) {
        return TSKA_EFAIL;
    }

    return ((KeyD *)rbNode->key)->index;
}
