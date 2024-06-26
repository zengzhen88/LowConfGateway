#include <MsgTrans.h>
#include <uvstream.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <YuerinMsg.h>
#include <assert.h>

MsgTrans::MsgTrans() {
    /*获取全局句柄*/
    ackValid    = 0;
    loop        = UVStreamNewLoop();
}
MsgTrans::~MsgTrans() { }

// void *MsgTrans::loop = NULL;

/*服务端*/
void *MsgTrans::MsgTransInit(
        const char *serIp, int32_t serPort) {
    UVServerConfig config;

    config.port         = serPort;
    config.listMaxNum   = YUERIN_MSG_LISTEN_MAX_NUM;
    config.print        = MsgTransPrintfStatic;
    config.dealCmd      = MsgTransDealCmdStatic;
    config.dealRun      = MsgTransDealRunStatic;
    config.newConnect   = MsgTransDealNewConnectStatic;
    config.priv         = this;
    strcpy((char *)config.ipServ, serIp);

    return UVServerInit(loop, &config);
}

/*客户端*/
void *MsgTrans::MsgTransInit( 
        const char *serIp, int32_t serPort, 
        const char *cliIp, int32_t cliPort) {
    void *handle    = NULL;
    UVClientConfig config;

    config.print        = MsgTransPrintfStatic;
    config.dealCmd      = MsgTransDealCmdStatic;
    config.dealRun      = MsgTransDealRunStatic;
    config.newConnect   = MsgTransDealNewConnectStatic;
    config.priv         = this;
    config.portServ     = serPort;
    strcpy((char *)config.ipServ, serIp);
    config.portCli      = cliPort;
    strcpy((char *)config.ipCli, cliIp);

    ringBuffer_ = RingBufferInit(RINGMEMSIZE);
    if (ringBuffer_ == NULL) {
        return NULL;
    }

    handle = UVClientInit(loop, &config);
    if (handle == NULL) {
        RingBufferDestory(ringBuffer_);
        return NULL;
    }

    return handle;
}

void *MsgTrans::MsgTransInit(MsgTransType type, 
        const char *serIp, int32_t serPort, 
        const char *cliIp, int32_t cliPort) {

    switch (type) {
        case MsgTransType_server:
            {
                return MsgTransInit(serIp, serPort);
            }
        case MsgTransType_Client:
            {
                return MsgTransInit(serIp, serPort, cliIp, cliPort);
            }
        default:
            {
                return NULL;
            }
    }

    return NULL;
}

void *MsgTrans::MsgTransAddTimer(int32_t timerMs, void *priv, RunTimerCb runTimerCb) {
    return UVStreamTimerInit(loop, priv, timerMs, runTimerCb);
}

int32_t MsgTrans::MsgTransDelTimer(void *uvTimer) {
    return UVStreamTimerDestory(loop, uvTimer);
}

int32_t MsgTrans::MsgTransDestory(MsgTransType type, void *oObj) {
    switch (type) {
        case MsgTransType_server:
            {
                return UVServerDestory(oObj);
            }
        case MsgTransType_Client:
            {
                return UVClientDestory(oObj);
            }
        default:
            {
                return -1;
            }
    }

    return -1;
}

int32_t MsgTrans::MsgTransSchdule(void) {
    // UVStreamSchduleConfig config;

    // config.loop     = loop;
    // config.priv     = this;
    // config.print    = MsgTransPrintfStatic;

    return UVStreamSchdule(loop);
}

int32_t MsgTrans::MsgTransUnSchdule(void) {
    return UVStreamUnSchdule(loop);
}

int32_t MsgTrans::MsgTransDelete(void) {
    return UVStreamDelete(loop);
}

int32_t MsgTrans::MsgTransPrintfStatic(void *oObj, const char *strings) {
    MsgTrans *messTrans = (MsgTrans *)oObj;

    if (messTrans) {
        messTrans->MsgTransPrintfs(strings);
    }

    return 0;
}

int32_t MsgTrans::MsgTransDealCmdStatic(void *oObj, 
        void *client, void *mess, int32_t *messLen) {
    MsgTrans *messTrans = (MsgTrans *)oObj;

    if (messTrans) {
        return messTrans->MsgTransDealCmds(client, mess, messLen);
    }

    return -1;
}

int32_t MsgTrans::MsgTransDealRunStatic(void *oObj) {
    MsgTrans *messTrans = (MsgTrans *)oObj;

    if (messTrans) {
        return messTrans->MsgTransDealRuns();
    }

    return -1;
}

int32_t MsgTrans::MsgTransDealNewConnectStatic(void *oObj, void *client, 
        uint32_t peerIp, uint32_t peerPort, int32_t isConnect) {
    MsgTrans *messTrans = (MsgTrans *)oObj;

    if (messTrans) {
        return messTrans->MsgTransConnects(client, peerIp, peerPort, isConnect);
    }

    return -1;
}

int32_t MsgTrans::MsgTransConnects(void *client, uint32_t peerIp,
        uint32_t peerPort, int32_t isConnect) {
    RingBuffer *ringBuffer      = NULL;
    MsgTransClientLabel *label  = NULL;

    if (client) {
        if (isConnect) {
            ringBuffer = RingBufferInit(RINGMEMSIZE);
            if (ringBuffer != NULL) {
                ringMap.emplace(client, ringBuffer);
            }

            label = new MsgTransClientLabel;
            if (label) {
                label->peerIp   = peerIp;
                label->peerPort = peerPort;
                label_.emplace(client, label);
            }

            MsgTransDealConnects(client, peerPort);
        }
        else {
            map<void *, RingBuffer *>::iterator iti;
            iti = ringMap.find(client);
            if (iti != ringMap.end()) {
                if (iti->second) {
                    RingBufferDestory((RingBuffer *)iti->second);
                }
                ringMap.erase(iti);
            }
            map<void *, MsgTransClientLabel *>::iterator iti1;
            iti1 = label_.find(client);
            if (iti1 != label_.end()) {
                if (iti1->second) {
                    delete ((MsgTransClientLabel *)iti1->second);
                }
                label_.erase(iti1);
            }
        }
    }

    for (auto iti = ringMap.begin(); iti != ringMap.end(); iti++) {
        // printf ("ringMapSize:%u: loopPrint ringBuffer match(client:%p <<>> ringMem:%p)\n", 
                // ringMap.size(), (void *)iti->first, (void *)iti->second);
    }
    for (auto iti = label_.begin(); iti != label_.end(); iti++) {
        // printf ("ringMapSize:%u: loopPrint ringBuffer match(client:%p <<>> ringMem:%p)\n", 
                // label_.size(), (void *)iti->first, (void *)iti->second);
    }

    return 0;
}

int32_t MsgTrans::MsgTransDealCmds(void *client, void *mess, int32_t *messLen) {
    int32_t availLen        = 0;        //环形缓冲区有效长度
    uint32_t cpyLen         = 0;        //首先考虑拷贝的长度
    int32_t remainLen       = 0;
    int32_t msgLen          = 0;
    uint32_t retVal         = -1;
    YuerinMsg *msg          = NULL;
    RingBuffer *ringBuffer  = NULL;
    map<void *, RingBuffer *>::iterator iti;
    uint8_t swapBuffer[RINGMEMSIZE];

    iti = ringMap.find(client);
    if (iti != ringMap.end()) {
        ringBuffer = (RingBuffer *)iti->second;
        if (ringBuffer) {
            availLen = RingBufferAvail(ringBuffer);
            if (availLen >= *messLen) {
                //环形缓冲区大小足够，全拷贝
                cpyLen = *messLen;
            }
            else {
                //环形缓冲区大小不够，拷贝缓冲区剩余大小
                cpyLen      = availLen;
                remainLen   = *messLen - cpyLen;
            }

            //将数据写入环形缓冲区
            retVal = RingBufferWrite(ringBuffer, mess, cpyLen);
            if (retVal != cpyLen) {
                //写入失败????
                printf (">?????????????\n");
            }

            while (1) {
                //再次检测环形缓冲区有效长度
                availLen = RingBufferLen(ringBuffer);
                //有效长度大于基类大小
                if ((uint32_t)availLen >= sizeof(*msg)) {
                    //预读基类大小数据从缓冲区
                    retVal = RingBufferPreRead(ringBuffer, (void *)swapBuffer, (uint32_t)sizeof(*msg));
                    if (retVal == sizeof(*msg)) {
                        msg = (YuerinMsg *)swapBuffer;
                        if (msg) {
                            //获取实际的消息总大小
                            msgLen = msg->GetSize();
                            //如果环形缓冲区数据大小大于消息总大小，则可以进行消息处理
                            if (availLen >= msgLen) {
                                //预读数据长度实时生效
                                RingBufferMatchPreRead(ringBuffer, sizeof(*msg));
                                //再次将剩余的数据消息包大小拷贝出来
                                cpyLen = msgLen - sizeof(*msg);
                                retVal = RingBufferRead(ringBuffer, 
                                        swapBuffer + sizeof(*msg), cpyLen);
                                if (retVal == cpyLen) {
                                    if (!msg->GetIsAck()) {
                                        ackValid = 0;
                                        MsgTransDealCmd(client, (void *)swapBuffer, &msgLen);
                                    }
                                    else {
                                        ackValid = 1;
                                        memcpy(ackBuffer, swapBuffer, msgLen);
                                    }

                                    if (!remainLen) {
                                        //数据有效长度不够，结束
                                        break;
                                    }
                                }
                                else {
                                    assert(0);
                                }
                            }
                            else {
                                if (!remainLen) {
                                    //数据有效长度不够，结束
                                    break;
                                }

                                //再次检测环形缓冲区有效长度
                                availLen = RingBufferAvail(ringBuffer);
                                if (remainLen > availLen) {
                                    assert(remainLen > availLen);
                                }
                                else {
                                    //将剩余的数据写入到环形缓冲区
                                    retVal = RingBufferWrite(ringBuffer, 
                                            (uint8_t *)mess + (*messLen - remainLen), remainLen);
                                    if (retVal != (uint32_t)remainLen) {
                                        assert(0);
                                    }
                                }
                            }
                        }
                        else {
                            assert(0);
                        }
                    }
                    else {
                        assert(0);
                    }
                }
                else {
                    if (!remainLen) {
                        //数据有效长度不够，结束
                        break;
                    }

                    //再次检测环形缓冲区有效长度
                    availLen = RingBufferAvail(ringBuffer);
                    if (remainLen > availLen) {
                        assert(remainLen > availLen);
                    }
                    else {
                        //将剩余的数据写入到环形缓冲区
                        retVal = RingBufferWrite(ringBuffer, 
                                (uint8_t *)mess + (*messLen - remainLen), remainLen);
                        if (retVal != (uint32_t)remainLen) {
                            assert(0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}

//客户端使用
int32_t MsgTrans::MsgTransSend(void *client, YuerinMsg *mess, MsgAck *ackMsg) {
    int32_t status          = -1;
    // int32_t availLen        = 0;        //环形缓冲区有效长度
    // int32_t cpyLen          = 0;        //首先考虑拷贝的长度
    // int32_t remainLen       = 0;
    // int32_t msgLen          = 0;
    // int32_t ackLen          = ackMsg != NULL ? ackMsg->GetSize() : 0;
    // YuerinMsg *msg          = NULL;
    MsgAck *sAckMsg         = NULL;
    // uint32_t retVal         = -1;
    // RingBuffer *ringBuffer  = NULL;
    // map<void *, RingBuffer *>::iterator iti;
    // uint8_t swapBuffer[RINGMEMSIZE];

    status = UVClientSend(client, 
            mess, mess->GetSize());
    if (!status) {
        if (ackMsg != NULL) {
            int32_t sleeps = 200;
            while (1) {
                status = -1;
                if (ackValid) {
                    ackValid = 0;
                    sAckMsg = (MsgAck *)ackBuffer;
                    if (ackMsg->GetAckId() == sAckMsg->GetAckId()) {
                        memcpy((void *)ackMsg, (void *)sAckMsg, ackMsg->GetSize());
                        status = 0;
                        break;
                    }
                    else {
                        assert(ackMsg->GetAckId() == sAckMsg->GetAckId());
                    }
                }
                else {
                    UVStreamSchdule(loop);
                    sleeps--;
                    if (sleeps <= 0) break;
                    usleep(5000);
                }
/*
 *                 status = UVClientRecv(client, swapBuffer, &ackLen);
 *                 if (!status) {
 *                     //读到数据才开始处理
 *                     if (ackLen) {
 *                         //检测环形缓冲区大小
 *                         availLen = RingBufferAvail(ringBuffer_);
 *                         if (availLen >= ackLen) {
 *                             //环形缓冲区大小足够，全拷贝
 *                             cpyLen      = ackLen;
 *                         }
 *                         else {
 *                             //通常这个应该是不太可能发生
 *                             //环形缓冲区大小不够，拷贝缓冲区剩余大小
 *                             cpyLen      = availLen;
 *                             remainLen   = ackLen - cpyLen;
 *                         }
 * 
 *                         //将有效数据长度的数据拷贝入环形缓冲区
 *                         status = RingBufferWrite(ringBuffer_, swapBuffer, cpyLen);
 *                         if (status != cpyLen) {
 *                             //写入失败 
 *                             printf (">>>/???????\n");
 *                         }
 * 
 *                         //检测环形缓冲区有效长度
 *                         availLen = RingBufferLen(ringBuffer_);
 *                         //有效长度大于基类大小
 *                         if ((uint32_t)availLen >= sizeof(*msg)) {
 *                             status = RingBufferPreRead(ringBuffer_, (void *)ackMsg, (uint32_t)sizeof(*msg));
 *                             if (status == sizeof(*msg)) {
 *                                 msg = (YuerinMsg *)ackMsg;
 *                                 if (msg) {
 *                                     //获取实际的消息总大小
 *                                     msgLen = msg->GetSize();
 *                                     //如果环形缓冲区数据大小大于消息总大小，则可以进行消息处理
 *                                     if (availLen >= msgLen) {
 *                                         RingBufferMatchPreRead(ringBuffer_, sizeof(*msg));
 *                                         cpyLen = msgLen - sizeof(*msg);
 *                                         status = RingBufferRead(ringBuffer_, 
 *                                                 ((uint8_t *)ackMsg) + sizeof(*msg), cpyLen);
 *                                         if (status == cpyLen) {
 *                                             if (remainLen) {
 *                                                 status = RingBufferWrite(ringBuffer_, 
 *                                                         (uint8_t *)swapBuffer + (ackLen - remainLen), remainLen);
 *                                                 if (status != remainLen) {
 *                                                     assert(0);
 *                                                 }
 *                                             }
 *                                             return 0;
 *                                         }
 *                                         else {
 *                                             assert(0);
 *                                         }
 *                                     }
 *                                     else {
 *                                         if (remainLen) {
 *                                             status = RingBufferWrite(ringBuffer_, 
 *                                                     (uint8_t *)swapBuffer + (ackLen - remainLen), remainLen);
 *                                             if (status != remainLen) {
 *                                                 assert(0);
 *                                             }
 *                                         }
 *                                     }
 *                                 }
 *                             }
 *                         }
 *                     }
 *                 }
 *                 else {
 *                     break;
 *                 }
 */
            }
        }
    }

    return status;
}

//服务端使用
int32_t MsgTrans::MsgTransSend(void *client, YuerinMsg *mess) {
    return UVServerSend(client, mess, mess->GetSize());
}
