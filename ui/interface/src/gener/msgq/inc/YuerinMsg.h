#ifndef __YUERIN_MESS_H__
#define __YUERIN_MESS_H__ 



#define YuerinMsgCmd(prefix)           \
    MsgId_##prefix##Cmd,               \
    MsgId_##prefix##Ack,               \

#define CmdToMsgId(suffix)             \
    MsgId_##suffix 
     

typedef struct YuerinMsg {
    int32_t messId;
    int32_t messLen;
    int32_t isAck;  //是否是ackMsg

    YuerinMsg(int32_t messId) {
        this->messId     = messId;
        this->messLen    = sizeof(YuerinMsg);
        this->isAck      = 0;
    }

    int32_t GetSize() const { return messLen; }
    /* virtual  */int32_t GetAckId() { return messId + 1; }
    int32_t GetIsAck() { return isAck; }
    protected:
    void SetLen(int32_t len) {
        this->messLen = len;
    }
} YuerinMsg;

struct MsgAck : public YuerinMsg {
    int32_t ackId;

    MsgAck(int32_t messId, int32_t ackId)
        : YuerinMsg(messId) {
            SetLen(sizeof(MsgAck));
            this->ackId = ackId;
            this->isAck = 1;
        }
    int32_t GetAckId() { return messId; }
};

#endif /*__YUERIN_MESS_H__*/
