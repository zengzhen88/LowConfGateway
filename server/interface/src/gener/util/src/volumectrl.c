#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "volumectrl.h"

#define sat16(a)        (((a) > 32767) ? 32767 : (((a) < -32768) ? -32768 : (short)(a)))
#define MIN_DB_SOFT     -96 //20*log10(1/65536)

typedef struct VolSec {
    int   minVolume;
    int   maxVolume;
    float minDB;
    float maxDB;
    float dt;
}VolSec;

typedef struct VolManage {
    int inited;
    
    int muteShield;         /*是否静音或者闭音*/
    int volume;             /*实时音量值*/
    float dbValueRatio;     /*实时音量换算db值*/

    int    secnum;
    VolSec sec[DBSECNUMMAX];

    VolCtrlVolType voltype;
	
    void *priv;
	volPrint print;
	LogVol level;
	
    char title[128];
} VolManage;


/*************************************************/ 
/**************LogPrintf**************************/
/*************************************************/ 

static int VolCtrlLogPrintf(void *pObj, LogVol level,
        const char *func, int line, const char *format, ...) {
    char logBuf[1024];
    va_list args;
    int funcLine        = 0;
    VolManage *stu     = (VolManage *)pObj;

    if (NULL == stu) return 0;

    if (level > stu->level) return -1;

    snprintf (logBuf, sizeof(logBuf), "[%s][%d]", func, line);
    funcLine = strlen(logBuf);

    /*va_list*/
    va_start(args, format);
    vsnprintf(&logBuf[funcLine], sizeof(logBuf) - funcLine, format, args);
    va_end(args);
    /*va_list*/

    if (stu->print) {
        return stu->print(stu->priv, logBuf);
    }

    return -1;
}

#define LogPrintf(level, ...)\
        VolCtrlLogPrintf(stu, level, __FUNCTION__, __LINE__, __VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)     
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogVol_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogVol_Debug, __VA_ARGS__);
#endif

#ifndef ERRP
#define ERRP(con, ret, flag, ...)               \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif


void* VolCtrlInit(const char *title, 
        int secnum, 
        VolCtrlVolType voltype,
        volPrint print, void *priv) {
    VolManage* stu = (VolManage*)malloc(sizeof(VolManage));
    if (NULL == stu) {
        LogPrintf(LogVol_Warning, "%s:malloc volctrl handle failure.\n",
                NULL == title ? "" : title);
        return NULL;
    }

    memset(stu, 0x0, sizeof(*stu));    

    stu->muteShield     = 0;
    stu->dbValueRatio   = 1.0;

    stu->secnum = secnum;
    if(secnum < 0)           stu->secnum = 0;
    if(secnum > DBSECNUMMAX) stu->secnum = DBSECNUMMAX;

    stu->voltype = voltype;
	stu->print   = print;
    stu->priv    = priv;
	stu->level   = LogVol_Info;

    strncpy(stu->title, title, sizeof(stu->title));
    stu->inited = 1;

    LogPrintf(LogVol_Info, "VolCtrlInit %s suc\n", title);

    return stu;
}

void VolCtrlConfig(void *pObj, int secid, int minvol, int maxvol, float minctlval, float maxctlval) {
    VolManage* stu = (VolManage*)pObj;
    if (NULL == stu) {
        LogPrintf(LogVol_Warning, "%s:VolManage instance not exist.\n",
                NULL == stu ? "" : stu->title);
        return;
    }

    if (!stu->inited) {
        LogPrintf(LogVol_Warning, "Please call VolCtrlInit first.\n");
        return;
    }

    if (secid < 0 || secid >= stu->secnum) {
        LogPrintf(LogVol_Warning, "%s:input secid :%d invalid! <Should be set in inited range[0~%d)>.\n",
                stu->title, secid, stu->secnum);
        return;
    }
	
	if (maxvol < minvol || maxctlval < minctlval) {
        LogPrintf(LogVol_Warning, "%s:input minvol:%d shold be less than maxvol:%d and"
			"minctlval:%d shoule be less than maxctlval.\n",
                stu->title, secid, stu->secnum, minvol, maxvol, minctlval, maxctlval);
        return;
    }

    stu->sec[secid].minVolume = minvol;
    stu->sec[secid].maxVolume = maxvol;

    switch (stu->voltype) {
        case VolCtrlVolType_Ratio: {
            //转换为db，实际还是按照db的斜率控制，调节过渡效果最好
            if (minctlval < 0 || maxctlval < 0) {
                LogPrintf(LogVol_Warning, "%s:not support a nagtive ctrlvol.\n", stu->title);
                return;
            }
			
			stu->sec[secid].minDB     = (minctlval == 0) ? MIN_DB_SOFT : 20 * log10(minctlval);
			stu->sec[secid].maxDB     = (maxctlval == 0) ? MIN_DB_SOFT : 20 * log10(maxctlval);

            LogPrintf(LogVol_Info, "secid:%d minVol:%d maxVol:%d minCtrlVal:%f maxCtrlVal:%f minDB:%f maxDB:%f\n", 
                    secid, minvol, maxvol, minctlval, maxctlval, 
                    stu->sec[secid].minDB, stu->sec[secid].maxDB);
			
            break;
        }
        case VolCtrlVolType_DB: 
        default: {
            stu->sec[secid].minDB     = minctlval;
            stu->sec[secid].maxDB     = maxctlval;
            break;
        }
    }

    //计算dB的调节斜率
    stu->sec[secid].dt = (stu->sec[secid].maxVolume == stu->sec[secid].minVolume) ? 1 : 
		(float)(stu->sec[secid].maxDB - stu->sec[secid].minDB) / 
        (stu->sec[secid].maxVolume - stu->sec[secid].minVolume);

    LogPrintf(LogVol_Info, "secid:%d dt:%f\n", secid, stu->sec[secid].dt);
    
    return;
}

void VolCtrlUpdVol(void *pObj, int volume) {
    VolManage* stu = (VolManage*)pObj;
    int   index = 0;
    float dbfit = 0.0;
    
    if (NULL == stu) {
        LogPrintf(LogVol_Warning, "%s:VolManage instance not exist.\n",
                NULL == stu ? "" : stu->title);
        return;
    }

    if (!stu->inited) {
        LogPrintf(LogVol_Warning, "Please call VolCtrlInit first.\n");
        return;
    }

    for (index = 0; index < stu->secnum; index++) {
        if (volume >= stu->sec[index].minVolume && volume <= stu->sec[index].maxVolume) {
            dbfit = stu->sec[index].minDB + stu->sec[index].dt * (volume - stu->sec[index].minVolume);
            stu->dbValueRatio = pow(10.0, dbfit / 20.0);
            break;
        }
    }

    stu->volume = volume;
    LogPrintf(LogVol_Info, "%s:volume:%d fitto secid:%d db:%f ratio:%f.\n",
        stu->title, volume, index, dbfit, stu->dbValueRatio);
    
    return;
}

void VolCtrlUpdStu(void *pObj, int muteShield) {
    VolManage* stu = (VolManage*)pObj;
    if (NULL == stu) {
        LogPrintf(LogVol_Warning, "%s:VolManage instance not exist\n",
                NULL == stu ? "" : stu->title);
        return;
    }

    if (!stu->inited) {
        LogPrintf(LogVol_Warning, "Please call VolCtrlInit first.\n");
        return;
    }

    stu->muteShield     = muteShield;

    return;
}

/* params 0: volume manage instance
 * params 1: input data
 * params 2: input data length byte
 * params 3: mute or shield
 */
void VolCtrlControl(void *pObj, short *in, short *out, int dataLen) {
    VolManage* stu = (VolManage*)pObj;
    int index = 0;

    if (NULL == stu
            || NULL == in
            || NULL == out
            || 0 > dataLen) {
        LogPrintf(LogVol_Warning, "VolManageControl[%s] input params Error[%d:%d:%d:%d]\n",
                stu ? stu->title : "", NULL == stu, NULL == in, NULL == out, 0 > dataLen);
        return;
    }

    if (!stu->inited) {
        LogPrintf(LogVol_Warning, "Please call VolCtrlInit first.\n");
        return;
    }

    LogPrintf(LogVol_Trace, "muteShield:%d dbValueRatio:%f\n", stu->muteShield, stu->dbValueRatio);

    if (stu->muteShield) {
        memset(out, 0x0, dataLen);
        return;
    }

    dataLen >>= 1;/*short per sample*/

    for (index = 0; index < dataLen; index++) {
        out[index] = sat16(in[index] * stu->dbValueRatio);
    }

    return;
}

float VolCtrlGetRatio(void *pObj) {
    VolManage* stu = (VolManage*)pObj;
    if (NULL == stu) {
        LogPrintf(LogVol_Warning, "%s:VolManage instance not exist\n",
                NULL == stu ? "" : stu->title);
        return 1.0;
    }

    if (!stu->inited) {
        LogPrintf(LogVol_Warning, "Please call VolCtrlInit first.\n");
        return 1.0;
    }
    
    return stu->dbValueRatio;
}

int VolCtrlGetVolStu(void *pObj){
    VolManage* stu = (VolManage*)pObj;
    if (NULL == stu) {
        LogPrintf(LogVol_Warning, "%s:VolManage instance not exist\n",
                NULL == stu ? "" : stu->title);
        return 0;
    }

    if (!stu->inited) {
        LogPrintf(LogVol_Warning, "Please call VolCtrlInit first.\n");
        return 0;
    }

    return stu->muteShield;
}

void VolCtrlExit(void *pObj) {
	VolManage* stu = (VolManage*)pObj;
    if (NULL == stu) {
        LogPrintf(LogVol_Warning, "%s:VolManage instance not exist\n",
                NULL == stu ? "" : stu->title);
        return;
    }

    if (!stu->inited) {
        LogPrintf(LogVol_Warning, "Please call VolCtrlInit first.\n");
        return;
    }
	
	free(stu);
	stu = NULL;
	
	return;
}

int VolCtrlLogVolevel(void *pObj, LogVol level) {
    VolManage* stu = (VolManage*)pObj;
	if (NULL == stu) {
        LogPrintf(LogVol_Warning, "%s:VolManage instance not exist\n",
                NULL == stu ? "" : stu->title);
        return -1;
    }
	
    ERRP((int)level < (int)LogVol_None || (int)level > (int)LogVol_Cnt, return -1, 1, 
            "input LogVol Level invalid? valid[%d-%d]\n",
            LogVol_None , LogVol_Cnt);

    stu->level = level;

    return 0;
}
