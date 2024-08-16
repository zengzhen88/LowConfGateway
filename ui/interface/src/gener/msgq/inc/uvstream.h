#ifndef __UV_STREAM_H__
#define __UV_STREAM_H__ 

#include <stdint.h>
#include <map>

using namespace std;
/*
 * 注：libuv是单线程操作，所有操作只能在单线程中运转
 */

typedef enum {
    LogUVStream_None = 0,  //关闭日志输出
    LogUVStream_Urgent,    //必须打的
    LogUVStream_Fatal,     //致使级
    LogUVStream_Error,     //错误级
    LogUVStream_Warning,   //告警级
    LogUVStream_Info,      //业务级
    LogUVStream_Debug,     //调试级
    LogUVStream_Trace,     //跟踪级
    LogUVStream_Detail,    //详细级
    LogUVStream_Cnt
} LogUVStream;

typedef enum {
    UVStreamType_TCP,
    UVStreamType_PIPE,
} UVStreamType;


//////////////////////////////////////////////////
//接口使用详见示例 
//////////////////////////////////////////////////
//处理日志输出
//参数1:外部句柄
//参数2:输出字符串
typedef int32_t (*UVStreamPrintCb)(void *extHandle, const char *string);

//处理消息及ACK接口，消息和ACK消息使用同一片内存
//参数1:外部句柄 
//参数2:客户端uv_handle_t句柄
//参数3:消息首地址
//参数4:消息长度
typedef int32_t (*UVStreamDealCmd)(void *extHandle, void *cli, void *, int32_t *);

//处理需要定期在主调度中运行的函数接口回调
//参数1:外部句柄
typedef int32_t (*UVStreamDealRun)(void *extHandle);

//处理相关的客户端连接服务器初始化回调
//参数1:内部句柄
//参数2:外部句柄
typedef int32_t (*UVStreamConnectInit)(void *, void *, uint32_t, uint32_t, int32_t);

#define MAX_MSGQ_LENGTH     (64*1024)    /*64K*/

#define UVCOMOBJ                            \
    /*线程ID,用于关键校验*/                 \
    long tid;                               \
    /*连接断开与否*/                        \
    int32_t isConnect;                      \
    /*标记将要关闭*/                        \
    int32_t markClose;                      \
    /*中转消息长度*/                        \
    int32_t msgLen;                         \
    /*消息中转内存*/                        \
    int8_t ackMsg[MAX_MSGQ_LENGTH];         \
    void *priv;                             \
    /*内部句柄,用于对外接口索引关键结构*/   \
    void *inPriv;                           \
    void *loop;                             \
    void *handMap;                          \
    UVStreamPrintCb print;                  \
    UVStreamDealCmd dealCmd;                \
    UVStreamDealRun dealRun;                \
    UVStreamConnectInit newConnect;         \
    void *backPriv;                         \


typedef struct UVServerConfig {
    /*srver*/
    int8_t ipServ[128];                 /*服务器IP地址字符串*/
    int32_t port;                       /*服务器本地端口*/
    int32_t listMaxNum;                 /*服务器监听端口最大值*/

    /*public*/
    void *priv;                         /*对应模块的实例句柄*/
    UVStreamPrintCb print;              /*日志输出回调*/
    UVStreamDealCmd dealCmd;            /*消息处理回调*/
    UVStreamDealRun dealRun;            /*周期性功能回调*/
    UVStreamConnectInit newConnect;     /*新连接初始*/
} UVServerConfig;

typedef struct UVClientConfig {
    int8_t ipServ[128];     /*服务器IP地址字符串*/
    int8_t ipCli[128];      /*客户端IP地址字符串*/
    int32_t portServ;       /*服务器端口*/
    int32_t portCli;        /*客户端端口*/

    void *priv;             /*对应模块的实例句柄*/
    UVStreamPrintCb print;  /*日志输出回调*/
    UVStreamDealCmd dealCmd;/*消息处理回调*/
    UVStreamDealRun dealRun;/*周期性功能回调*/
    UVStreamConnectInit newConnect;     /*新连接初始*/
} UVClientConfig;


#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////
//模块对应线程调用

/*客户端初始化接口*/
void *UVClientInit(void *_loop, UVClientConfig *config);
int32_t UVClientDestory(void *oObj);

/*客户端是否在线*/
int32_t UVClientAlive(void *oObj);


/*服务端初始化接口*/
void *UVServerInit(void *_loop, UVServerConfig *config);
int32_t UVServerDestory(void *oObj);

/* 客户端发送数据接口
 * 参数1:客户端句柄
 * 参数2:消息首地址
 * 参数3:消息实际长度
 * */
int32_t UVClientSend(void *loop, 
        void *data, int32_t len);
/* 客户端接收数据接口
 * 参数1:客户端句柄
 * 参数2:ACK消息首地址
 * 参数3:Ack消息实际长度
 * */
int32_t UVClientRecv(void *client, void *ack, int32_t *ackLen);

/*服务端发送ACK数据
 * 参数1:客户端句柄 
 * 参数2:消息首地址 
 * 参数3:消息实际长度
 */
int32_t UVServerSend(void *client, void *data, int32_t len);

/////////////////////////////////////////
//模块内部回调调用

//定时器初始化
void *UVStreamTimerInit(void *_loop, void *priv, 
        int32_t timeMs, UVStreamDealRun dealRun);
//定时器销毁
int32_t UVStreamTimerDestory(void *_loop, void *_uvTimer);

/////////////////////////////////////////
//调度接口 

typedef struct UVStreamSchduleConfig {
    char name[128];
    void *priv;
    void *loop;
    UVStreamPrintCb print;
} UVStreamSchduleConfig;

int32_t UVStreamSetLogLevel(void *priv, UVStreamPrintCb print);
void *UVStreamNewLoop();

//主调度接口
int32_t UVStreamSchdule(void *loop/* UVStreamSchduleConfig *config */);
int32_t UVStreamUnSchdule(void *_loop);

/*删除Stream实例*/
int32_t UVStreamDelete(void *loop);

#ifdef __cplusplus
};
#endif

#endif /*__UV_STREAM_H__*/
