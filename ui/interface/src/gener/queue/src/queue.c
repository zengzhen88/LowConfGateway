#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "queue.h"

/**********************void***********************/
/*该方案试用于接收与发送在同一个线程处理，
 * 特点就是如果等数据没有来就得continue，
 * 重新扫描接收队列； 
 *
 * 另一方案是接收与发送不在同一个线程处理，
 * 特点就是如果等数据没有来就sleep，
 * 直到等待超时结束,由于代码设计该方案没采用，
 * 与设计不合，后续如果有这方面的需要，
 * 可以做一定修改*/

/*出现了TIME_EXCEED_DELAY_MAX_TIMES次网络抖动情况，考虑重新统计当前网络状况*/
#define TIME_EXCEED_DELAY_MAX_TIMES                 (10)

/*出现了连续TIME_EXCEED_DELAY_MAX_TIMES次网络抖动情况，考虑停止该数据*/
#define TIME_EXCEED_DELAY_ANALY_MAX_TIMES           (500)

/*出现时间戳不对齐连续TIME_STAMP_ANOMALY_MAX_TIMES次，考虑该队列不参与队列管理*/
#define TIME_STAMP_ANOMALY_MAX_TIMES                (20)

/*连续处理CONTINUE_FRAME_PROCESS_VALID_TIMES帧有效，初步分析通路正常*/
#define CONTINUE_FRAME_PROCESS_VALID_TIMES          (20)
#define CONTINUE_FRAME_PROCESS_VALID_TIMES_I        (50)

#define CONTINUE_FRAME_TIMESTAMP_XXX_THAN_TIMES     (5)

#define FRAME_NUM                                   (3)
#define FRAME_NUM_MIN                               (2)
#define FRAME_NUM_MAX                               (10)
#define FRAME_NUM_SUPER_MAX                         (20)
/*超过FRAME_NUM_OF_RECOVER则说明周期性
 * 丢包无法解决该通道累包高速情况，考虑
 * 大范围处理掉多余的包*/
#define FRAME_NUM_OF_RECOVER                        (3)
#define FRAME_EXCEED_THREE_NUM_MAX_TIMES            (700)      /*统计30s*/

#define DROP_FRAME_CHECK_MAX_TIMES                  (375)      /*统计15s*/

/*码率偏差超过QUEUE_BITRATE_ERROR_OFFSET,该队列不再处理*/
#define QUEUE_BITRATE_ERROR_OFFSET                  (0.2)
/*码率偏差超过QUEUE_BITRATE_IMPROPE_OFFSET,该队列抖动统计不列入队列管理中*/
#define QUEUE_BITRATE_IMPROPE_OFFSET                (0.1)
/*码率错误时1分钟进行一次码率检查*/
#define QUEUE_BITRATE_IMPROPE_CHECK_TIME            (60000)
/*码率错误时1分钟进行一次码率检查*/
#define QUEUE_BITRATE_IMPROPE_RECOVER_CHECK_TIME    (10000)
/*统计时1秒探测一次*/
#define QUEUE_BITRATE_PROBE_TIME                    (1200)

/*关停队列的条件*/
#define TIME_EXCEED_DELAY_MAX_TIMES_OF_ANALY_TIME  (180000)


#define LOG_DEBUG_LENGTH                            (2048)

#define QUEUE_SUPER_MAX_NUMS                        (40)
#define QUEUE_MIN_NUMS                              (3)
#define QUEUE_STAB_NUMS                             (7)
#define QUEUE_MAX_NUMS                              (30)
#define QUEUE_SAMPLE_PACKET_US                      (20)

#define QUEUE_MAX_NUMS_ADDS                         (5)     /*如果出现缓存被吃光的情况，下次缓存最大值增量为5帧*/

#define QUEUE_BITRATE_PROBE_VALID_NUMS              (1)

#define VAD_POWER_CHECK_CYCLES              (10)
#define VAD_MAX_DROP_VALUE                  (5)

static const char *SwitchCmdStr[SwitchCmd_cnt] = {
    "SwitchCmd_getRunStu",//             = 0,
    "SwitchCmd_getBitRate",//            = 1,    /*获取统计码率*/                  
    "SwitchCmd_resetBitRate",//          = 2,    /*复位码率统计*/                                 
    "SwitchCmd_getFirstFrame",//         = 3,    /*第一帧状态*/    
    "SwitchCmd_bitRateError",//          = 4,    /*设置码率错误状态*/                             
    "SwitchCmd_bitRateNormal",//         = 5,    /*设置码率正常状态*/                             
    "SwitchCmd_timeStampAnomaly",//      = 6,    /*设置时间戳异常状态*/
    "SwitchCmd_bitRateProbeing",//       = 7,    /*码率探测状态*/
    "SwitchCmd_bitRateProbeSok",//       = 8,    /*码率探测状态有效*/
    "SwitchCmd_bitRateProbeEfail",//     = 9,    /*码率探测状态无效*/
    "SwitchCmd_dropPackageInvalid",//     = 10,   /*设置当前丢包无效*/
    "SwitchCmd_outRequestExtMem",//      = 11,   /*输出请求外部内存*/
    "SwitchCmd_outReturnExtMem",//       = 12,   /*输出归还外部内存*/
    "SwitchCmd_inRequestExtMem",//       = 13,   /*输入请求外部内存*/
    "SwitchCmd_inReturnExtMem",//        = 14,   /*输入归还外部内存*/
    "SwitchCmd_configLocalConf",//       = 15,   /*配置本地会场*/
    "SwitchCmd_startCaching",//          = 16,   /*开始缓存中*/
    "SwitchCmd_getQueCount",//           = 17,   /*获取当前队列成员个数*/
    "SwitchCmd_insertPakcage",//         = 18,   /*插帧处理*/
    "SwitchCmd_startCached",//           = 19,   /*结束缓存*/        
    "SwitchCmd_getGlobalTimeStamp",                                  
    "SwitchCmd_getGlobalSeqId",                         
    "SwitchCmd_getGlobalTs64",                                      
    "SwitchCmd_getCurrentTimes",//       = 23,
    "SwitchCmd_getInFull",//             = 24,
    "SwitchCmd_putInEmpty",//            = 25,
    "SwitchCmd_getOutEmpty",//           = 26,
    "SwitchCmd_putOutFull",//            = 27,
    "SwitchCmd_peekInFull",//            = 28,
    "SwitchCmd_getVadPower",//           = 29,
    "SwitchCmd_doRampOut",//             = 30,
    "SwitchCmd_doRampIn",//              = 31,
    "SwitchCmd_getInCount",//            = 32,
    "SwitchCmd_check",//                 = 33,
};  
    
typedef struct QueueObj {
    int byCreate;/*队列管理结构是否创建*/
    put QueGetCurrentTimer;                 /*获取当前时间*/
    queueDrop QueDropPackage;               /*队列丢包处理*/

    cmd QueSwitchCmdProc;                   /*每次添加一功能就加一个回调，太麻烦，写一统一接口*/

    printt print;
    LogQUE level;

    queueProc QueCheckPackage;

    long initStu;                            /*初始状态位*/
    long normalStu;                          /*常规处理状态位*/
    long analyStu;                           /*统计状态位*/
    long stabStu;                            /*稳定状态位*/
    long probeStu;                           /*探测码率状态*/
    long validTimeStamp;                /*时间戳是否有效*/
    long timeStampOverturn;             /*时间戳翻转次数*/

    long validSeqTimeStamp;             /*seq时间戳是否有效*/
    long seqTimeStampOverturn;          /*seq时间戳翻转次数*/


    long timeMaxDelay;                  /*设置的最大抖动时间*/

    long timeExceedDelayRef;            /*出现超过统计延时抖动次数*/
    long beforeAnalyTimeExceedDelayRef; /*统计时出现超过最大延时抖动次数*/
    long frameContinueValidAfterTimeExceedDealyRef;
                                            /*出现超过统计延时抖动后帧连续有效次数*/
    long timeExceedDelayAnalyTimer;     /*统计超过延时抖动异常次数时间间隔*/

    long timeStampAnomalyRef;           /*时间戳连续异常次数统计*/

    long continueProcValidRef;          /*连续处理有效次数，用于异常恢复处理*/

    long frameExceedThreeContinueRef;   /*帧数超过3帧的连续次数统计*/

    long timeStampGreaterThanIntervalRef;    
                                            /*时间戳大于统计间隔值连续次数*/
    long timeStampLessThanIntervalRef;  /*时间戳小于统计间隔值连续次数*/

    long timeExceedDelayValidRef;    
    long timeExceedDelayValid;               /*出现超过统计延时抖动情况，按异常处理*/
    long beforeAnalyTimeExceedDelayValid;    /*统计时出现超过统计延时抖动情况，按异常处理*/
    long timeStampAnomalyValid;              /*时间戳异常统计有效，按异常处理*/
    long timeExceedDelayContinueValid;       /*连续没有收到数据，按异常处理*/
    long timeExceedDelayValidAndRecover;     /*出现超过统计延时抖动情况多次，考虑重新统计*/

    long timeStart;                     /*开始时间*/
    long timeLast;                      /*上一帧的时间*/
    long timeCurrent;                   /*当前帧的时间*/

    long timeDelay;                         /*统计的最大抖动时间*/
    long timeMDelay;

    long isActiveDrop;                       /*主动丢包标识*/

    long isPassiveDrop;                      /*被动丢包累加*/
    long isPassiveDropT;                     /*被动丢包累加临时值*/

    long isInsert;                           /*是否插帧补偿*/
    long frameInsertContinueRef;        /*连续插帧次数统计*/
    long frameInsertContinueRef1;        /*连续插帧次数统计*/

    long bitRate;
    long bitRateValidCheck;                  /*该会场出现过码率异常*/
    long bitRateInvalidNum;               
    long bitRateValidCheckTimer;             /*码率检测起始时间*/

    long timeDelayInvalid;                   /*因为异常，统计得到的延时值无效*/

    long bitRateProbeValidCheck;             /*该会场出现过码率异常*/
    long bitRateProbeValidNum;               /*该会场出现过码率异常*/
    long bitRateProbeCheckTimer;             /*码率探测起始时间*/

    long isDrop;                             /*是否丢包，对部分丢包提供标识给外部使用*/

    long startCacheing;                      /*开始缓存*/
    long startCacheStu;                      /*开始缓存*/

    long queMinNum;                          /*队列低峰预设值*/
    long queMaxNum;                          /*队列高峰预设值*/
    long queStabNum;

    long queMinNums;                         /*队列低峰统计值*/
    long queMaxNums;                         /*队列高峰统计值*/

    long addRefNum;                          

    long checkSuperMax;      

    long key;                                /*关键会场，基于该会场进行处理*/

    long checkMinStu;                        /*最小值状态统计，统计时间为一分钟*/

    char title[128];

    void *queManageStu;                     /*队列管理实例*/

    /*priv*/
    void *priv;                             /*私有数据，目前指向会场实例*/

    SwitchCmd runStu;

    long *input;

    long vadPowerCheckCycles;

    int enDrop;
} QueueObj;

typedef enum {
    QUE_ERR = -1,
    QUE_SOK = 0,
} QUE;


/*************************************************/ 
/**************LogPrintf**************************/
/*************************************************/ 

static int IQueManageLogPrintf(void *oObj, LogQUE level,
        const char *func, int line, const char *format, ...) {
    char logBuf[1024];
    va_list args;
    int funcLine        = 0;
    QueueObj *queue  = (QueueObj *)oObj;

    if (NULL == queue) return 0;

    if (level > queue->level) return -1;

    snprintf (logBuf, sizeof(logBuf), "[%s][%d][0x%p]", func, line, queue->priv);
    funcLine = strlen(logBuf);

    /*va_list*/
    va_start(args, format);
    vsnprintf(&logBuf[funcLine], sizeof(logBuf) - funcLine, format, args);
    va_end(args);
    /*va_list*/

    if (queue->print) {
        return queue->print(queue->priv, logBuf);
    }

    return -1;
}

#define LogPrintf(level, ...)\
        IQueManageLogPrintf(queue, level, __FUNCTION__, __LINE__, __VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)     
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogQUE_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogQUE_Debug, __VA_ARGS__);
#endif

#ifndef ERRP
#define ERRP(con, ret, flag, ...)               \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif

/******************Queue***********************/
void *QueInit(QueConf *config) {
    QueueObj *queue = (QueueObj *) malloc (sizeof(QueueObj));
    if (NULL == queue) {
        fprintf (stderr, "malloc QueueObj failure\n");
        return NULL;
    }

    memset(queue, 0x0, sizeof(*queue));

    queue->QueDropPackage             = config->queDropPackage;
    queue->QueSwitchCmdProc           = config->queSwitchCmdProc;

    queue->timeMaxDelay               = config->timeMaxDelayMs;

    queue->priv                       = config->priv;
    queue->level                      = LogQUE_Info;
    queue->print                      = config->print;
    queue->runStu                     = SwitchCmd_bitRateProbeing;
    queue->bitRate                    = config->bitRate;
    queue->enDrop                     = config->enDrop;
    queue->queMaxNum                  = config->max;
    queue->queMinNum                  = config->min;
    queue->queStabNum                 = config->stb;

    strncpy(queue->title, config->name, sizeof(queue->title));
    queue->title[sizeof(queue->title) - 1] = '\0';
    LogPrintf(LogQUE_Info, "timeDelay:%ld timeMDelay:%ld\n", 
            queue->timeDelay, queue->timeMDelay);

    queue->byCreate                   = 1;

    return queue;
}

void QueDestory(void *stu) {
    if (NULL == stu) return;

    free(stu);
}

int Queue_isInsertPackage(void *oObj) {
    QueueObj *stu = (QueueObj *) oObj;
    int isInsert = -1;
    if (!stu->byCreate) return -1;

    isInsert = stu->isInsert;
    stu->isInsert = 0;

    return !isInsert;
}

int Queue_isPassiveDropPackage(void *oObj) {
    QueueObj *stu = (QueueObj *) oObj;
    int isPassiveDrop = -1;
    if (!stu->byCreate) return -1;

    isPassiveDrop = stu->isPassiveDrop;
    stu->isPassiveDrop = 0;

    return !isPassiveDrop;
}

int Queue_isDropPackage(void *oObj) {
    QueueObj *stu = (QueueObj *) oObj;
    int isDrop = -1;
    if (!stu->byCreate) return -1;

    isDrop = stu->isDrop;
    stu->isDrop = 0;

    return !isDrop;
}

int Queue_SwitchCmdProc(void *oObj, void **value, SwitchCmd cmd) {
    QueueObj *queue = (QueueObj *) oObj;
    if (!queue->byCreate) return -1;

    switch (cmd) {
        case SwitchCmd_bitRateNormal:                                                             
        case SwitchCmd_timeStampAnomaly:                                                          
        case SwitchCmd_bitRateError:
        case SwitchCmd_bitRateProbeSok:                                                           
        case SwitchCmd_bitRateProbeEfail:
        case SwitchCmd_bitRateProbeing:
        case SwitchCmd_startCaching:
        case SwitchCmd_startCached:  
            LogPrintf(LogQUE_Info, "[%s]switch runStu from [%s] to [%s]\n",                   
                    queue->title, SwitchCmdStr[queue->runStu], SwitchCmdStr[cmd]);
            queue->runStu = cmd;
            break;
        default:
            break;
    }

    return queue->QueSwitchCmdProc(queue->priv, value, cmd);
}

void QueSetKeyQueue(void *oObj) {
    QueueObj *stu = (QueueObj *) oObj;
    if (!stu->byCreate) return;

    stu->key = 1; 
    stu->runStu = SwitchCmd_bitRateNormal;
}

static int QueManage_queDropPackage(QueueObj *queue) {
    int ret             = 0;
    long vadPower       = 0;
    long *drop          = NULL;
    long count          = 0;
    long *pCount        = NULL;

    pCount = &count;
    Queue_SwitchCmdProc(queue, (void **)&pCount, SwitchCmd_getInCount);

    if (count > queue->queMinNum) {
        if (!Queue_isPassiveDropPackage(queue)) {
            return queue->QueDropPackage((void *)queue->priv);
        }
        else {
            if (queue->enDrop) {
                if (!Queue_isDropPackage(queue)) {
                    queue->vadPowerCheckCycles = VAD_POWER_CHECK_CYCLES;
                }

                /*must have two frame*/
                if (count > 2) {
                    /*check have queue valid*/
                    ret = Queue_SwitchCmdProc(queue, (void **)&drop, SwitchCmd_peekInFull);
                    if (!ret) {
                        ret = -1;
                        /*vad check drop*/
                        vadPower= Queue_SwitchCmdProc(queue, (void **)&drop, SwitchCmd_getVadPower);
                        /*found low power frame*/
                        if (vadPower <= VAD_MAX_DROP_VALUE || queue->vadPowerCheckCycles < 0) {
                            if (queue->input) {
                                /*fade out to input frame*/
                                /* RampOut((short *)queue->input->addr, queue->input->fillLength); */
                                Queue_SwitchCmdProc(queue, (void **)&queue->input, SwitchCmd_doRampOut);
                            }

                            /*drop low power frame*/
                            ret = Queue_SwitchCmdProc(queue, (void **)&drop, SwitchCmd_getInFull);
                            if (!ret) {
                                LogPrintf(LogQUE_Warning, "trigger low power package refNum:%ld\n", 
                                        queue->vadPowerCheckCycles);
                                Queue_SwitchCmdProc(queue, (void **)&drop, SwitchCmd_putInEmpty);
                                ret = 0;
                            }

                            /*check the next frame of the drop frame exist*/
                            ret = Queue_SwitchCmdProc(queue, (void **)&drop, SwitchCmd_peekInFull);
                            if (!ret) {
                                /*frame valid, so fade in*/
                                if (drop) {
                                    Queue_SwitchCmdProc(queue, (void **)&drop, SwitchCmd_doRampIn);
                                }
                            }
                        }
                        else {
                            queue->vadPowerCheckCycles--;
                        }
                    }
                    else {
                        ret = Queue_SwitchCmdProc(queue, (void **)&drop, SwitchCmd_getInFull);
                        if (!ret) {
                            LogPrintf(LogQUE_Warning, "trigger low power package \n");
                            Queue_SwitchCmdProc(queue, (void **)&drop, SwitchCmd_putInEmpty);
                            ret = 0;
                        }
                    }
                }
            }
        }
    }

    Queue_SwitchCmdProc(queue, NULL, SwitchCmd_check);

    return ret;
}

static int QueManage_queActiveDropPackage(QueueObj *queue) {
    if (queue->isActiveDrop) {
        if (!QueManage_queDropPackage(queue)) {
            queue->isActiveDrop     = 0;
            LogPrintf(LogQUE_Warning, "[%s] one Active drop package is required???\n",
                    queue->title);
        }
    }

    return 0;
}

static int QueManage_quePassiveDropPackage(QueueObj *queue) {
    if (queue->isPassiveDrop) {
        if (!QueManage_queDropPackage(queue)) {
            queue->isPassiveDrop     = 0;
            LogPrintf(LogQUE_Warning, "[%s] one Passive drop package is required???\n",
                    queue->title);
        }
    }

    return 0;
}

static void queManage_queRecoverDefault(QueueObj *queue, int reset) {
    queue->stabStu                          = 0;
    queue->analyStu                         = 0;
    queue->probeStu                         = 0;
    queue->normalStu                        = 0;
    queue->isActiveDrop                     = 0;
    queue->isPassiveDrop                    = 0;
    queue->isPassiveDropT                   = 0;
    queue->bitRateValidCheck                = 0;
    queue->bitRateProbeValidNum             = 0;
    queue->timeExceedDelayValid             = 0;
    queue->timeExceedDelayValidRef          = 0;
    queue->bitRateValidCheckTimer           = 0;
    queue->bitRateProbeValidCheck           = 0;
    queue->bitRateProbeCheckTimer           = queue->timeCurrent;

    if (reset) {
        queue->timeLast                         = queue->timeCurrent;
        queue->timeStart                        = queue->timeCurrent;
        queue->timeExceedDelayRef               = 0;
        queue->timeExceedDelayValidAndRecover   = 0;

        Queue_SwitchCmdProc(queue, NULL, SwitchCmd_resetBitRate);
    }
}


static int queManage_queBitRateProbe(QueueObj *queue) {
    long *pBitRate       = NULL;
    long bitRate         = 0;

    if (queue->bitRateProbeValidNum >= QUEUE_BITRATE_PROBE_VALID_NUMS 
            && queue->bitRateProbeValidCheck) {
        /*探测成功返回0*/
        return 0;
    }
    else if (queue->bitRateProbeValidNum >= QUEUE_BITRATE_PROBE_VALID_NUMS) {
        /*探测失败返回1*/
        return 1;
    }

    if (!queue->probeStu) {
        queue->probeStu = 1;
        /*开始探测，切换到探测状态*/
        Queue_SwitchCmdProc(queue, NULL, SwitchCmd_bitRateProbeing);
    }

    LogPrintf (LogQUE_Detail, ">>>>>>>>>>>>>>>>timeCurrent:%ld bitRateProbeCheckTimer:%ld diff:%ld\n", 
            queue->timeCurrent, queue->bitRateProbeCheckTimer, 
            queue->timeCurrent - queue->bitRateProbeCheckTimer);
    if (queue->timeCurrent - queue->bitRateProbeCheckTimer 
            > QUEUE_BITRATE_PROBE_TIME) {
        queue->bitRateProbeValidNum++;
        pBitRate = &bitRate;
        Queue_SwitchCmdProc(queue, (void **)&pBitRate, SwitchCmd_getBitRate);
        LogPrintf(LogQUE_Info, "runBitRate:%ld\n", bitRate);
        bitRate = queue->bitRate - bitRate;
        LogPrintf(LogQUE_Info, "[%s]******queue->bitRate:%ld bitRate:%ld offsetErr:%ld offsetnor:%ld\n", 
                queue->title,
                queue->bitRate, bitRate, 
                (int)(QUEUE_BITRATE_ERROR_OFFSET * queue->bitRate), 
                (int)(QUEUE_BITRATE_IMPROPE_OFFSET * queue->bitRate));
        if (bitRate > -((int)(QUEUE_BITRATE_IMPROPE_OFFSET  * queue->bitRate))
                && bitRate < (int)(QUEUE_BITRATE_IMPROPE_OFFSET * queue->bitRate)) {
            /*码率在正常波动范围内，按正常流程处理*/
            queue->bitRateProbeValidCheck = 1;
        }
        else if (bitRate > -((int)(QUEUE_BITRATE_ERROR_OFFSET * queue->bitRate))
                && bitRate < (int)(QUEUE_BITRATE_ERROR_OFFSET * queue->bitRate)) {
            /*码率不在正常波动范围内，但初步看可以接接受，参与管理，部分受限*/
            queue->bitRateProbeValidCheck = 1;
        }

        queue->bitRateProbeCheckTimer = queue->timeCurrent;

        if (bitRate) {
            /*探测一次完成，复位下一次探测*/
            Queue_SwitchCmdProc(queue, NULL, SwitchCmd_resetBitRate);
        }
    }

    LogPrintf (LogQUE_Detail, "bitRateProbeValidCheck:%ld bitRateProbeValidNum:%ld\n", 
            queue->bitRateProbeValidCheck, 
            queue->bitRateProbeValidNum);
    if (queue->bitRateProbeValidNum >= QUEUE_BITRATE_PROBE_VALID_NUMS
            && queue->bitRateProbeValidCheck) {
        /*探测成功，切换成功状态*/
        Queue_SwitchCmdProc(queue, NULL, SwitchCmd_bitRateProbeSok);
        return 0;
    }
    else if (queue->bitRateProbeValidNum >= QUEUE_BITRATE_PROBE_VALID_NUMS) {
        /*探测失败，切换失败状态*/
        Queue_SwitchCmdProc(queue, NULL, SwitchCmd_bitRateProbeEfail);
        queManage_queRecoverDefault(queue, 1);

        return 1;
    }

    return 1;
}


static int queManage_queBitRateStab(QueueObj *queue) {
    long *pBitRate       = NULL;
    long bitRate         = 0;

    queue->normalStu = 1;

    if (!queue->bitRateValidCheck) {
        pBitRate = &bitRate;
        Queue_SwitchCmdProc(queue, (void *)&pBitRate, SwitchCmd_getBitRate);
        bitRate = queue->bitRate - bitRate;
        LogPrintf(LogQUE_Debug, "[%s]queue->bitRate:%ld bitRate:%ld offsetErr:%ld offsetnor:%ld\n", 
                queue->title,
                queue->bitRate, bitRate, 
                (int)(QUEUE_BITRATE_ERROR_OFFSET * queue->bitRate), 
                (int)(QUEUE_BITRATE_IMPROPE_OFFSET * queue->bitRate));
        if (bitRate > -((int)(QUEUE_BITRATE_IMPROPE_OFFSET * queue->bitRate))
                && bitRate < (int)(QUEUE_BITRATE_IMPROPE_OFFSET * queue->bitRate)) {
            /*码率在正常波动范围内，按正常流程处理*/
            queue->stabStu = 1;
            LogPrintf(LogQUE_Info, "[%s]switch to normal bitrate model???\n", queue->title);

            LogPrintf(LogQUE_Info, "Updata stu [%s] timeDelay frome:%ld to %ld???\n", 
                    queue->title, queue->timeMDelay, 
                    queue->timeDelay > queue->timeMDelay ? queue->timeDelay : queue->timeMDelay);
            queue->timeMDelay = queue->timeDelay > queue->timeMDelay ? queue->timeDelay : queue->timeMDelay;
            if (queue->timeMDelay > queue->timeMaxDelay) {
                queue->timeMDelay = queue->timeMaxDelay;
            }
            Queue_SwitchCmdProc(queue, NULL, SwitchCmd_bitRateNormal);
        }
        else if (bitRate > -((int)(QUEUE_BITRATE_ERROR_OFFSET * queue->bitRate))
                && bitRate < (int)(QUEUE_BITRATE_ERROR_OFFSET * queue->bitRate)) {
            /*码率不在正常波动范围内，但初步看可以接接受，参与管理，部分受限*/
            LogPrintf(LogQUE_Info, "[%s]switch to imprope bitrate model???\n", queue->title);
            queue->stabStu = 1;
            Queue_SwitchCmdProc(queue, NULL, SwitchCmd_bitRateNormal);
        }
        else {
            LogPrintf(LogQUE_Warning, "[%s]queue->bitRate:%ld bitRate:%ld offsetErr:%ld offsetnor:%ld\n", 
                    queue->title,
                    queue->bitRate, bitRate, 
                    (int)(QUEUE_BITRATE_ERROR_OFFSET * queue->bitRate), 
                    (int)(QUEUE_BITRATE_IMPROPE_OFFSET * queue->bitRate));
            LogPrintf(LogQUE_Warning, "[%s]switch to imprope error model???\n", queue->title);
            queue->bitRateValidCheck = 1;
            queue->timeDelayInvalid = 1;
            queue->stabStu = 0;
            Queue_SwitchCmdProc(queue, NULL, SwitchCmd_bitRateError);
        }
    }

    queue->bitRateValidCheckTimer = queue->timeCurrent;
    Queue_SwitchCmdProc(queue, NULL, SwitchCmd_resetBitRate);

    return 0;
}

static int queManage_queBitRateNotStab(QueueObj *queue) {
    long *pBitRate       = NULL;
    long bitRate         = 0;

    if (queue->timeCurrent - queue->bitRateValidCheckTimer 
            > QUEUE_BITRATE_IMPROPE_CHECK_TIME
            && queue->bitRateValidCheck) {
        queue->bitRateInvalidNum = 0;
        pBitRate = &bitRate;
        Queue_SwitchCmdProc(queue, (void **)&pBitRate, SwitchCmd_getBitRate);
        bitRate = queue->bitRate - bitRate;
        LogPrintf(LogQUE_Debug, "[%s]queue->bitRate:%ld bitRate:%ld offsetErr:%ld offsetnor:%ld\n", 
                queue->title,
                queue->bitRate, bitRate, 
                (int)(QUEUE_BITRATE_ERROR_OFFSET * queue->bitRate), 
                (int)(QUEUE_BITRATE_IMPROPE_OFFSET * queue->bitRate));
        if (bitRate > -((int)(QUEUE_BITRATE_IMPROPE_OFFSET  * queue->bitRate))
                && bitRate < (int)(QUEUE_BITRATE_IMPROPE_OFFSET * queue->bitRate)) {
            /*码率在正常波动范围内，按正常流程处理*/
            LogPrintf(LogQUE_Info, "[%s]switch to normal bitrate model???\n", queue->title);

            if (queue->timeCurrent - queue->timeLast > queue->timeDelay) {
                LogPrintf(LogQUE_Info, "timeCurrent:%ld timeLast:%ld Updata [%s] "
                        "timeDelay frome:%ld to %ld???\n", 
                        queue->timeCurrent, queue->timeLast,
                        queue->title, queue->timeDelay, 
                        queue->timeCurrent - queue->timeLast);
                queue->timeDelay = queue->timeCurrent - queue->timeLast;
                if (queue->timeDelay > queue->timeMDelay) {
                    queue->timeDelay = queue->timeMDelay;
                }
            }

            queManage_queRecoverDefault(queue, 1);
            Queue_SwitchCmdProc(queue, NULL, SwitchCmd_bitRateNormal);

            return 0;
        }
        else if (bitRate > -((int)(QUEUE_BITRATE_ERROR_OFFSET * queue->bitRate))
                && bitRate < (int)(QUEUE_BITRATE_ERROR_OFFSET * queue->bitRate)) {
            /*码率不在正常波动范围内，但初步看可以接接受，参与管理，部分受限*/
            LogPrintf(LogQUE_Warning, "[%s]switch to imprope bitrate model???\n", queue->title);
            queManage_queRecoverDefault(queue, 1);
            Queue_SwitchCmdProc(queue, NULL, SwitchCmd_bitRateNormal);

            return 0;
        }
        else {
            LogPrintf(LogQUE_Warning, "[%s]queue->bitRate:%ld bitRate:%ld offsetErr:%ld offsetnor:%ld\n", 
                    queue->title,
                    queue->bitRate, bitRate, 
                    (int)(QUEUE_BITRATE_ERROR_OFFSET * queue->bitRate), 
                    (int)(QUEUE_BITRATE_IMPROPE_OFFSET * queue->bitRate));
            LogPrintf(LogQUE_Warning, "[%s]switch to error bitrate model???\n", queue->title);
            queue->stabStu = 0;
            queue->bitRateValidCheck = 1;
            queue->bitRateValidCheckTimer = queue->timeCurrent;
            Queue_SwitchCmdProc(queue, NULL, SwitchCmd_resetBitRate);
            Queue_SwitchCmdProc(queue, NULL, SwitchCmd_bitRateError);
            return 1;
        }
    }
    else if (queue->timeCurrent - queue->bitRateValidCheckTimer 
            > QUEUE_BITRATE_IMPROPE_CHECK_TIME) {
        pBitRate = &bitRate;
        Queue_SwitchCmdProc(queue, (void **)&pBitRate, SwitchCmd_getBitRate);
        bitRate = queue->bitRate - bitRate;
        LogPrintf(LogQUE_Debug, "[%s]queue->bitRate:%ld bitRate:%ld offsetErr:%ld offsetnor:%ld\n", 
                queue->title,
                queue->bitRate, bitRate, 
                (int)(QUEUE_BITRATE_ERROR_OFFSET * queue->bitRate), 
                (int)(QUEUE_BITRATE_IMPROPE_OFFSET * queue->bitRate));
        if (!(bitRate > -((int)(QUEUE_BITRATE_ERROR_OFFSET * queue->bitRate))
                    && bitRate < (int)(QUEUE_BITRATE_ERROR_OFFSET * queue->bitRate))) {
            /*码率不在正常波动范围内，但初步看可以接接受，参与管理，部分受限*/
            LogPrintf(LogQUE_Warning, "[%s]queue->bitRate:%ld bitRate:%ld offsetErr:%ld offsetnor:%ld\n", 
                    queue->title,
                    queue->bitRate, bitRate, 
                    (int)(QUEUE_BITRATE_ERROR_OFFSET * queue->bitRate), 
                    (int)(QUEUE_BITRATE_IMPROPE_OFFSET * queue->bitRate));
            LogPrintf(LogQUE_Warning, "[%s]switch to error bitrate model???\n", queue->title);
            if (queue->bitRateInvalidNum++ >= 3) {
                queue->stabStu = 0;
                queue->bitRateValidCheck = 1;
                /*切换到码率错误模式*/
                Queue_SwitchCmdProc(queue, NULL, SwitchCmd_bitRateError);
            }

            return 1;
        }
        else {
            queue->bitRateInvalidNum = 0;
        }

        queue->bitRateValidCheckTimer = queue->timeCurrent;
        Queue_SwitchCmdProc(queue, NULL, SwitchCmd_resetBitRate);

        return 0;
    }

    return 1;
}

static int QueManage_queProcAnalySok(QueueObj *queue) {
    if (queue->normalStu) { 
        queManage_queBitRateNotStab(queue);
    }
    else {
        queManage_queBitRateStab(queue);
    }

    return 0;
}

static int QueManage_queProcAnalyEfail(QueueObj *queue) {
    queue->startCacheing    = 0;
    queue->startCacheStu    = 0;
    LogPrintf(LogQUE_Error, "%s queue not package exist, update to initStu\n", queue->title);


    return 1;
}

static int QueManage_queProcessCacheing(QueueObj *queue) {
    long count = 0;
    long *pCount = NULL;

    pCount = &count;

    Queue_SwitchCmdProc(queue, (void **)&pCount, SwitchCmd_getInCount);

    if (!queue->startCacheStu) {
        LogPrintf(LogQUE_Info, "%s ##### queue:%p\n", queue->title, queue);
        queue->queMinNums       = queue->queMaxNum;
        queue->queMaxNums       = 0;
        /* queue->queMinNum        = QUEUE_MIN_NUMS; */
        /* queue->queMaxNum        = QUEUE_MAX_NUMS; */
        /* queue->queStabNum       = QUEUE_STAB_NUMS;// + queue->addRefNum * QUEUE_MAX_NUMS_ADDS; */

        queue->checkMinStu      = 0;
        queue->startCacheStu    = 1;
        queue->frameExceedThreeContinueRef  = 0;
        Queue_SwitchCmdProc(queue, NULL, SwitchCmd_startCaching);
    }

    if (count >= queue->queStabNum) {
        queue->startCacheing = 1;
        Queue_SwitchCmdProc(queue, NULL, SwitchCmd_startCached);
        LogPrintf(LogQUE_Info, "%s start process queMinNum:%ld queStabNum:%ld queuMaxNum:%ld\n", 
                queue->title, queue->queMinNum, queue->queStabNum, queue->queMaxNum);
    }

    return 0;
}

static int QueManage_queProcess(QueueObj *queue) {
    long count = 0;
    long *pCount = NULL;

    pCount = &count;

    Queue_SwitchCmdProc(queue, (void **)&pCount, SwitchCmd_getInCount);

    if (!queue->startCacheing) {
        LogPrintf(LogQUE_Info, "startCacheingflag[%d]...[%s] max:%ld min:%ld stab:%ld curNum:%ld maxs:%ld mins:%ld\n",
                queue->startCacheing,
                queue->title, queue->queMaxNum, queue->queMinNum, queue->queStabNum, 
                count,
                queue->queMaxNums, queue->queMinNums);
        return QueManage_queProcessCacheing(queue);
    }

    if (count) {
        return QueManage_queProcAnalySok(queue);
    }

    return QueManage_queProcAnalyEfail(queue);
}

static int QueManage_queAnalyProc(QueueObj *queue) {
    if (!queManage_queBitRateProbe(queue)) {
        return QueManage_queProcess(queue);
    }
    else {
        return 1;
    }
}

static int QueManage_queProcInitStu(QueueObj *queue) {
    long count = 0;
    long *pCount = NULL;

    pCount = &count;

    /*接收到第一帧，记录为开始时间戳, 第一帧未收到前不做任何处理*/
    Queue_SwitchCmdProc(queue, (void **)&pCount, SwitchCmd_getInCount);
    if (!count) return -1;

    LogPrintf(LogQUE_Info, "[%s] switch to initStu???\n", queue->title);
   
    queue->initStu                  = 1;
    queue->stabStu                  = 0;
    LogPrintf(LogQUE_Info, "[%s] get the first frame\n", queue->title);

    queManage_queRecoverDefault(queue, 1);

    LogPrintf(LogQUE_Info, "curTs:%ld checkTs:%ld\n", 
            queue->timeCurrent, queue->bitRateProbeCheckTimer);

    /*获取到第一帧*/
    Queue_SwitchCmdProc(queue, NULL, SwitchCmd_getFirstFrame);

    return 0;
}

static void QueManage_queCheckFixedDelay(QueueObj *queue) {
    long count = 0;
    long *pCount = NULL;

    pCount = &count;
    Queue_SwitchCmdProc(queue, (void **)&pCount, SwitchCmd_getInCount);
    if (count > queue->queMaxNums) {
        LogPrintf(LogQUE_Debug, "[%s] queMaxNums %ld to %ld\n",
                queue->title, queue->queMaxNums, count);
        queue->queMaxNums = count;
    }

    if (count < queue->queMinNums) {
        LogPrintf(LogQUE_Debug, "[%s] queMinNums %ld to %ld\n", 
                queue->title, queue->queMinNums, count);
        queue->queMinNums = count;
    }

    if (!queue->checkMinStu) {
        if (queue->queMinNums > queue->queMinNum) {
            queue->frameExceedThreeContinueRef++;
            if (queue->frameExceedThreeContinueRef >= FRAME_EXCEED_THREE_NUM_MAX_TIMES) {
                queue->checkMinStu = 1;
                LogPrintf(LogQUE_Info, "[%s] check queue stab status complete!!!\n", queue->title);
                queue->queMinNums       = queue->queMaxNum;
                queue->queMaxNums       = 0;
            }
        }
        else {
            queue->addRefNum++;
            if (queue->addRefNum > 3) queue->addRefNum = 3; /*max 3*/
            queue->startCacheing    = 0;
            queue->startCacheStu    = 0;
            LogPrintf(LogQUE_Warning, "[%s] check queue stab status not complete!!!, "
                    "need to recover check addRefNum:%d\n", queue->title, queue->addRefNum);
        }
    }
    else {
        if (queue->frameExceedThreeContinueRef >= DROP_FRAME_CHECK_MAX_TIMES) {
            /*可能需要主动丢帧*/
            LogPrintf(LogQUE_Info, "@@@@@startCacheingflag[%ld]...[%s] max:%ld min:%ld stab:%ld curNum:%ld maxs:%ld mins:%ld\n",
                    queue->startCacheing,
                    queue->title, queue->queMaxNum, queue->queMinNum, queue->queStabNum, 
                    count,
                    queue->queMaxNums, queue->queMinNums);
            queue->isDrop       = 1;
            queue->isActiveDrop = 1;
            queue->frameInsertContinueRef = 0;
            queue->frameInsertContinueRef1 = 0;
            queue->frameExceedThreeContinueRef = 0;
            queue->queMinNums       = queue->queMaxNum;
            LogPrintf(LogQUE_Info, "%s trigger A isActiveDrop\n", queue->title);
            if (queue->checkSuperMax) {
                if (count > queue->queMaxNum + 5) {
                    queue->checkSuperMax = 0;
                    queue->isPassiveDrop = 1;
                    queue->queMaxNums    = 0;
                    LogPrintf(LogQUE_Warning, "%s trigger A isPassiveDrop\n", queue->title);
                    LogPrintf(LogQUE_Info, "startCacheingflag[%ld]...[%s] max:%ld min:%ld stab:%ld curNum:%ld maxs:%ld mins:%ld\n",
                            queue->startCacheing,
                            queue->title, queue->queMaxNum, queue->queMinNum, queue->queStabNum, 
                            count,
                            queue->queMaxNums, queue->queMinNums);
                }
            }
        }
        else {
            if (queue->queMinNums >= queue->queMinNum) {
                queue->checkSuperMax = 1;
                queue->frameInsertContinueRef = 0;
                queue->frameInsertContinueRef1 = 0;
                queue->frameExceedThreeContinueRef++;
            }
            else {
                queue->frameInsertContinueRef = 0;
                queue->frameInsertContinueRef1 = 0;
                queue->frameExceedThreeContinueRef = 0;
                queue->queMinNums = queue->queMaxNum;
            }
        }
    }
}

static int QueManage_queNormalProc(QueueObj *queue) {
    if (queue->startCacheing) {
        QueManage_queCheckFixedDelay(queue);
    }

    queue->timeLast             = queue->timeCurrent;

    return 0;
}


static int Queue_speciProc(QueueObj *queue) {
    if (queue->isPassiveDrop) {
        QueManage_quePassiveDropPackage(queue);
    }

    if (queue->isActiveDrop) {
        QueManage_queActiveDropPackage(queue);
    }

    if (queue->isInsert) {
    }

    return 0;
}

static int Queue_intervalProcess(QueueObj *queue) {
    switch (queue->runStu) {
        case SwitchCmd_startCaching:
            {
                /*开始缓存处理，如果统计出来的最大值大于预设值，继续缓存*/
                break;
            }
        case SwitchCmd_bitRateProbeSok:     /*探测成功，开始缓存，缓存基于预设值*/
            {
                /*统计过程中，缓存达到预设最大值，则开始丢部分包*/
                if (Queue_SwitchCmdProc(queue, NULL, SwitchCmd_getQueCount) > queue->queMaxNum) {
                    if (!Queue_SwitchCmdProc(queue, (void **)&queue->input, SwitchCmd_getInFull)) {
                        Queue_SwitchCmdProc(queue, (void **)&queue->input, SwitchCmd_putInEmpty);
                    }
                    queue->input = NULL;
                }
                break;
            }
        case SwitchCmd_bitRateNormal:
        case SwitchCmd_timeStampAnomaly:
        case SwitchCmd_startCached:
            {
                if (Queue_SwitchCmdProc(queue, (void **)&queue->input, SwitchCmd_getInFull)) {
                    /* queue->input->numChannel  = (unsigned)queue->numChannel; */
                    queue->input = NULL;
                }
                break;
            }
        case SwitchCmd_bitRateError:        /*码率异常，数据包有效性不考虑*/
        case SwitchCmd_bitRateProbeing:     /*探测中，数据包有效性不考虑*/
        case SwitchCmd_bitRateProbeEfail:   /*探测失败，数据包有效性不考虑*/
        default:
            {
                /*因为统计异常，所以不再减收输入，跟随会议管理来进行处理*/
                if (!Queue_SwitchCmdProc(queue, (void **)&queue->input, SwitchCmd_getInFull)) {
                    Queue_SwitchCmdProc(queue, (void **)&queue->input, SwitchCmd_putInEmpty);
                }
                queue->input = NULL;
                break;
            }
    }

    return 0;
}

int QueProcess(void *oObj) {
    int ret = -1;
    long *pTimeCurrent = NULL;
    QueueObj *queue = (QueueObj *)oObj;

    pTimeCurrent = &queue->timeCurrent;
    Queue_SwitchCmdProc(queue, (void **)&pTimeCurrent, SwitchCmd_getCurrentTimes);

    if (!queue->key) {
        if (queue->initStu) {
            ret = QueManage_queAnalyProc(queue);
            if (ret != 0) {
                Queue_intervalProcess(queue);
                return -1;
            }
        }
        else {
            ret = QueManage_queProcInitStu(queue);
            if (ret != 0) {
                Queue_intervalProcess(queue);
                return -1;
            }
        }
        QueManage_queNormalProc(queue);
    }

    Queue_intervalProcess(queue);

    Queue_speciProc(queue);

    return 0;
}

void *QueGetFrame(void *oObj) {
    void *pBuf      = NULL;
    QueueObj *queue = (QueueObj *)oObj;

    if (queue->input) {
        pBuf = queue->input;
        queue->input = NULL;
    }           

    return pBuf;
}
