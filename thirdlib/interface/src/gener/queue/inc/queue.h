#ifndef __QUE_MANAGE_H__
#define __QUE_MANAGE_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    LogQUE_None = 0,  //关闭日志输出 
    LogQUE_Urgent,    //必须打的
    LogQUE_Fatal,     //致使级
    LogQUE_Error,     //错误级
    LogQUE_Warning,   //告警级
    LogQUE_Info,      //业务级
    LogQUE_Debug,     //调试级
    LogQUE_Trace,     //跟踪级
    LogQUE_Detail,    //详细级
    LogQUE_Cnt
} LogQUE;

typedef enum {
    SwitchCmd_getRunStu             = 0,
    SwitchCmd_getBitRate            = 1,    /*获取统计码率*/
    SwitchCmd_resetBitRate          = 2,    /*复位码率统计*/
    SwitchCmd_getFirstFrame         = 3,    /*第一帧状态*/
    SwitchCmd_bitRateError          = 4,    /*设置码率错误状态*/
    SwitchCmd_bitRateNormal         = 5,    /*设置码率正常状态*/
    SwitchCmd_timeStampAnomaly      = 6,    /*设置时间戳异常状态*/
    SwitchCmd_bitRateProbeing       = 7,    /*码率探测状态*/
    SwitchCmd_bitRateProbeSok       = 8,    /*码率探测状态有效*/
    SwitchCmd_bitRateProbeEfail     = 9,    /*码率探测状态无效*/
    SwitchCmd_dropPackageInvalid    = 10,   /*设置当前丢包无效*/
    SwitchCmd_outRequestExtMem      = 11,   /*输入请求外部内存*/
    SwitchCmd_outReturnExtMem       = 12,   /*输入归还外部内存*/
    SwitchCmd_inRequestExtMem       = 13,   /*输出请求外部内存*/
    SwitchCmd_inReturnExtMem        = 14,   /*输出归还外部内存*/
    SwitchCmd_configLocalConf       = 15,   /*配置本地会场*/
    SwitchCmd_startCaching          = 16,   /*开始缓存中*/
    SwitchCmd_getQueCount           = 17,   /*获取当前队列成员个数*/
    SwitchCmd_insertPakcage         = 18,   /*插帧处理*/
    SwitchCmd_startCached           = 19,   /*结束缓存*/
    SwitchCmd_getGlobalTimeStamp    = 20, 
    SwitchCmd_getGlobalSeqId        = 21,
    SwitchCmd_getGlobalTs64         = 22,


    SwitchCmd_getCurrentTimes       = 23,
    SwitchCmd_getInFull             = 24,
    SwitchCmd_putInEmpty            = 25,
    SwitchCmd_getOutEmpty           = 26,
    SwitchCmd_putOutFull            = 27,
    SwitchCmd_peekInFull            = 28,
    SwitchCmd_getVadPower           = 29,
    SwitchCmd_doRampOut             = 30,
    SwitchCmd_doRampIn              = 31,
    SwitchCmd_getInCount            = 32,
    SwitchCmd_check                 = 33,

    SwitchCmd_cnt,
} SwitchCmd;

typedef int (*put)(void *hndl, void *value);
typedef int (*get)(void *hndl, void **value);
typedef int (*getCount)(void *hndl);
typedef int (*queueDrop)(void *);
typedef int (*queueProc)(void *, void *, void *);
typedef int (*cmd)(void *hndl, void **value, SwitchCmd cmd);
typedef int (*printt)(void *hndl, const char *string);

typedef struct {
    char name[128];
    int enDrop;
    int timeMaxDelayMs;
    int bitRate;
    void *priv;
    queueDrop queDropPackage;
    cmd queSwitchCmdProc;
    printt print;

    int max;
    int min;
    int stb;
} QueConf;


void *QueInit(QueConf *config);
void QueDestory(void *stu);

void QueSetKeyQueue(void *stu);

int QueProcess(void *oObj);
void *QueGetFrame(void *oObj);

#ifdef __cplusplus
}
#endif

#endif /*__QUE_MANAGE_H__*/
