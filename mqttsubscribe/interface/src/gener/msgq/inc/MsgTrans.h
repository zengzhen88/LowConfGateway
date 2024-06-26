#ifndef __MESS_TRANS_H__
#define __MESS_TRANS_H__ 

#include <uvstream.h>
#include <stdint.h>
#include <stdio.h>
#include <YuerinMsg.h>
#include <vector>
#include <map>
#include <RingBuffer.h>


#define RINGMEMSIZE     (MAX_MSGQ_LENGTH * 2)
using namespace std;

#define YUERIN_MSG_LISTEN_MAX_NUM   (128)

typedef enum {
    MsgTransType_server,
    MsgTransType_Client,
} MsgTransType;

typedef struct {
    uint32_t peerIp;
    uint32_t peerPort;
} MsgTransClientLabel;

typedef int32_t (*RunTimerCb)(void *oObj);

class MsgTrans {
    public:
        MsgTrans(); /*创建第一步*/
        ~MsgTrans(); /*销毁第三步*/

        /*通用接口*/
        virtual void *MsgTransInit(MsgTransType type,  /*创建第二步*/
                const char *serIp, int32_t serPort, 
                const char *cliIp = NULL, int32_t cliPort = 0);
        
        int32_t MsgTransDestory(MsgTransType type, void *oObj); /*销毁第一步*/

        /*全局调度启动*/
        int32_t MsgTransSchdule(void); /*创建第三步*/
        int32_t MsgTransUnSchdule(void); /*销毁第二步*/

        int32_t MsgTransDelete(void);

        /*日志输出接口*/
        virtual int32_t MsgTransPrintf(const char *strings) = 0;
        /*消息处理接口*/
        /* 参数1:客户端句柄 
         * 参数2:消息首地址 
         * 参数3:消息实际长度 
         */
        virtual int32_t MsgTransDealCmd(void *client, void *mess, int32_t *messLen) = 0;
        /*周期性处理接口，调度周期为50ms;注：此处非定时器功能*/
        virtual int32_t MsgTransDealRun(void) = 0;

        virtual int32_t MsgTransDealConnects(void *client, uint32_t peerPort) = 0;

        /*添加定时器*/
        /* 参数1:定时器周期，单位毫秒
         * 参数2:runTimerCb回调函数参数
         * 参数3:定时器周期回调函数接口
         */
        void *MsgTransAddTimer(int32_t timerMs, void *priv, RunTimerCb runTimerCb);
        /*删除定时器*/
        int32_t MsgTransDelTimer(void *uvTimer);

        /*带ACK发送接口*/
        int32_t MsgTransSend(void *client, YuerinMsg *mess, MsgAck *ackMsg);
        /*不带ACK发送接口*/
        int32_t MsgTransSend(void *client, YuerinMsg *mess);


        //Libuv的核心句柄
        void *loop;

    private:
        /*服务端初始化*/
        void *MsgTransInit(const char *serIp, int32_t serPort);
        /*客户端初始化*/
        void *MsgTransInit( 
                const char *serIp, int32_t serPort, 
                const char *cliIp, int32_t cliPort);

        int32_t MsgTransPrintfs(const char *strings) { 
            return MsgTransPrintf(strings);   
        }
        int32_t MsgTransDealCmds(void *client, void *mess, int32_t *messLen);
        int32_t MsgTransDealRuns(void) { 
            return this->MsgTransDealRun();
        }
        int32_t MsgTransConnects(void *client, uint32_t peerIp, 
                uint32_t peerPort, int32_t isConnect);

        static int32_t MsgTransPrintfStatic(void *oObj, const char *strings);
        static int32_t MsgTransDealCmdStatic(void *oObj, 
                void *client, void *mess, int32_t *messLen);
        static int32_t MsgTransDealRunStatic(void *oObj);
        static int32_t MsgTransDealNewConnectStatic(void *oObj, void *client, 
                uint32_t peerIp, uint32_t peerPort, int32_t isConnect);

        map<void *, RingBuffer *> ringMap;  /*服务端使用*/
        map<void *, MsgTransClientLabel *> label_;
        RingBuffer *ringBuffer_;             /*客户端使用*/
        int8_t ackValid;
        uint8_t ackBuffer[RINGMEMSIZE];
};

#endif /*__MESS_TRANS_H__*/
