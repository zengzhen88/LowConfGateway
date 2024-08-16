#ifndef __TAG_VOLUMECTRL_H__
#define __TAG_VOLUMECTRL_H__

#ifdef __cplusplus
extern "C" {
#endif

#define DBSECNUMMAX    10 //最多10段调节

typedef enum {
    LogVol_None = 0,  //关闭日志输出 
    LogVol_Urgent,    //必须打的
    LogVol_Fatal,     //致使级
    LogVol_Error,     //错误级
    LogVol_Warning,   //告警级
    LogVol_Info,      //业务级
    LogVol_Debug,     //调试级
    LogVol_Trace,     //跟踪级
    LogVol_Detail,    //详细级
    LogVol_Cnt
} LogVol;

typedef int (*volPrint)(void *, const char *);

typedef enum {
    VolCtrlVolType_Ratio,
    VolCtrlVolType_DB,
    VolCtrlVolType_cnt,
} VolCtrlVolType;


/* 音量控制初始化接口
 * 参数1：音量控制命名
 * 参数2：音量控制分段个数
 * 参数3：音量控制的控制类型:按数据放大倍数或者按db调节
 * 参数4：音量控制打印函数回调
 * 参数5：外部调用者句柄
 * 返回值：音量控制句柄
*/
void* VolCtrlInit(const char *title, int secnum, VolCtrlVolType voltype, volPrint print, void *priv);


/* 音量控制分段配置接口
 * 参数1：音量控制句柄
 * 参数2：音量控制第secid段（secid>=0）配置
 * 参数3：音量控制第secid段控制的最小和最大输入音量
 * 参数4：音量控制第secid段控制的最小和最大控制值
 * 返回值：空
*/
void VolCtrlConfig(void *pObj, int secid, int minvol, int maxvol, float minctlval, float maxctlval);


/* 音量控制音量调节接口
 * 参数1：音量控制句柄
 * 参数2：音量控制输入控制的音量大小
 * 返回值：空
*/
void VolCtrlUpdVol(void *pObj, int volume);


/* 音量控制音量状态接口
 * 参数1：音量控制句柄
 * 参数2：音量控制是否静音/闭音
 * 返回值：空
*/
void VolCtrlUpdStu(void *pObj, int muteShield);


/* 音量控制接口
 * 参数1：音量控制句柄
 * 参数2：音量控制要控制的输入数据
 * 参数2：音量控制要控制的输出数据
 * 参数3：音量控制要控制的输入数据长度(字节数)
 * 返回值：空
*/
void VolCtrlControl(void *pObj, short *in, short *out, int dataLen);


/* 音量控制获取放大倍数接口
 * 参数1：音量控制句柄
 * 返回值：音量放大倍数
*/
float VolCtrlGetRatio(void *pObj);


/* 音量控制获取音量状态接口
 * 参数1：音量控制句柄
 * 返回值：是否静音/闭音
*/
int VolCtrlGetVolStu(void *pObj);


/* 音量控制退出接口
 * 参数1：音量控制句柄
 * 返回值：空
*/
void VolCtrlExit(void *pObj);


/* 音量控制打印等级调节接口
 * 参数1：音量控制句柄
 * 参数2：音量控制打印的最低基本
 * 返回值：0
*/
int VolCtrlLogVolevel(void *pObj, LogVol level);

#ifdef __cplusplus
}
#endif

#endif /*__TAG_VOLUME_H__*/
