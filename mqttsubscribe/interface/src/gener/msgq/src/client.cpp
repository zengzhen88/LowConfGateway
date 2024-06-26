#include <stdio.h>
#include <uv.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <uvstream.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <assert.h>
// #include <memwatch.h>

#define CONNECT_IS_DEFAULT (0)
#define CONNECT_IS_ON (1)
#define CONNECT_IS_OFF (2)
/*************************************************/
/**************LogPrintf**************************/
/*************************************************/
static void *gPriv = NULL;
static UVStreamPrintCb gPrint;
static LogUVStream gLevel = LogUVStream_Info;

/*************************************************/
/**************LogPrintf**************************/
/*************************************************/

static int UVClientLogPrintf(LogUVStream level,
        const char *file, const char *func,
        int line, const char *format, ...) {
    char logBuf[1024];
    va_list args;
    int funcLine        = 0;

    if (level > gLevel) return -1;

    snprintf (logBuf, sizeof(logBuf), "[%s][%s][%d]", file, func, line);
    funcLine = strlen(logBuf);

    /*va_list*/
    va_start(args, format);
    vsnprintf(&logBuf[funcLine], sizeof(logBuf) - funcLine, format, args);
    va_end(args);
    /*va_list*/

    if (gPrint) {
        return gPrint(gPriv, logBuf);
    }

    return -1;
}

#define LogPrintf(level, ...)\
    UVClientLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogUVStream_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogUVStream_Debug, __VA_ARGS__);
#endif

#ifndef ERRP
#define ERRP(con, ret, flag, ...)               \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif

typedef struct UVClientObj {
    UVCOMOBJ

    /*对内*/
    void *client;

    /*public*/
    int8_t ipServ[128];     /*服务器IP地址字符串*/
    int8_t ipCli[128];      /*客户端IP地址字符串*/
    int32_t portServ;       /*服务器端口*/
    int32_t portCli;        /*客户端端口*/

    /*timer*/
    void *uvTimer;
} UVClientObj;

/*
 * @param:  server  libuv的tcp server对象
 * @param:  status  状态，小于0表示新连接有误
 * @author: sherlock
 */
static void on_connected(uv_connect_t *req, int status) {
    uint32_t peer_port      = 0;
    uint32_t peer_ipaddr    = 0;
    uv_handle_t *handle     = NULL;
    UVClientObj *pObj       = NULL;

    if (req && req->handle) {
        handle = (uv_handle_t *)req->handle;

        if(status) {
            free(req);
            LogPrintf(LogUVStream_Info, "connect error\n");
            return;
        }

        struct sockaddr addr;
        struct sockaddr_in addrin;
        int addrlen = sizeof(addr);
        char sockname[17] = {0};

        struct sockaddr addrpeer;
        struct sockaddr_in addrinpeer;
        int addrlenpeer = sizeof(addrpeer);
        char socknamepeer[17] = {0};

        if(0 == uv_tcp_getsockname((uv_tcp_t*)req->handle,&addr,&addrlen) 
                && 0 == uv_tcp_getpeername((uv_tcp_t*)req->handle,&addrpeer,&addrlenpeer))
        {
            addrin     = *((struct sockaddr_in*)&addr);
            addrinpeer = *((struct sockaddr_in*)&addrpeer);
            uv_ip4_name(&addrin,sockname,addrlen);
            uv_ip4_name(&addrinpeer,socknamepeer,addrlenpeer);

            LogPrintf(LogUVStream_Info, 
                    "%s:%d sendto %s:%d\r\n",
                    sockname, ntohs(addrin.sin_port),socknamepeer,
                    ntohs(addrinpeer.sin_port));  
            LogPrintf(LogUVStream_Info, "stream:%p pObj:%p\n", 
                    handle, handle->data);
            pObj = (UVClientObj *)handle->data;
            if (pObj) {
                if (pObj->newConnect) {
                    peer_port     = ntohs(addrinpeer.sin_port);
                    peer_ipaddr   = inet_addr(socknamepeer);
                    pObj->newConnect(pObj->priv, (uv_tcp_t *)handle, peer_ipaddr, peer_port, 1);
                }
                pObj->isConnect = CONNECT_IS_ON;
            }
        }
        else {
            LogPrintf(LogUVStream_Error, "get socket fail!\n");
            pObj = (UVClientObj *)handle->data;
            if (pObj) {
                pObj->isConnect = CONNECT_IS_DEFAULT;//CONNECT_IS_OFF;
            }
        }

        if ((long)pObj->inPriv != (long)pObj->backPriv) {
            assert(0);
        }

        LogPrintf(LogUVStream_Info, " connect succeed!\r\n");  
    }

    if (req) {
        free(req);
    }
}

//负责为新来的消息申请空间
static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->len  = suggested_size;
    buf->base = (char *)(malloc(suggested_size));
}

static void on_close(uv_handle_t *handle) {
    UVClientObj *pObj = NULL;
    if (handle) {
        pObj = (UVClientObj *)(((uv_stream_t *)handle)->data);
        if (pObj) {
            if (pObj->newConnect) {
                pObj->newConnect(pObj->priv, handle, 0, 0, 0);
            }
            pObj->inPriv = NULL;     /*清内部私有指针*/
            pObj->backPriv = NULL;
        }
        LogPrintf(LogUVStream_Info, "handle:%p pObj:%p on_close\n", handle, pObj);
        if (handle != NULL) {
            free(handle);
        }
    }
}

/**
 * @brief: 负责处理新来的消息
 * @param: client
 * @param: nread>0表示有数据就绪，nread<0表示异常，nread是有可能为0的，但是这并不是异常或者结束
 * @author: sherlock
 */
static void read_cb(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    int32_t fillLength      = 0;
    UVClientObj *pObj       = (UVClientObj *)client->data;

    if (pObj) {
        if (nread > 0) {
            if (pObj->dealCmd) {
                fillLength = nread;
                pObj->dealCmd(pObj->priv, client,
                        buf->base, (int32_t *)&fillLength);
            }
            // pObj->msgLen    = nread;
            // memcpy(pObj->ackMsg, buf->base, nread);
            // assert(pObj->inPriv == pObj->backPriv);

            //释放之前申请的资源
            if (buf->base != NULL) {
                free(buf->base);
            }

            /* uv_print_all_handles((uv_loop_t *)pObj->loop, stdout); */

            return;
        } 
        else if (nread < 0) {
            // pObj->msgLen = -1;
            if (nread != UV_EOF) {
                LogPrintf(LogUVStream_Error, "handle:%p pObj:%p Read error? (%s)\n", 
                        client, pObj, uv_err_name(nread));
            } else {
                LogPrintf(LogUVStream_Error, "handle:%p pObj:%p disconnect\n");
            }
            // uv_close((uv_handle_t *) client, on_close);

            //释放之前申请的资源
            if (buf->base != NULL) {
                free(buf->base);
            }

            ((UVClientObj *)(client->data))->isConnect = CONNECT_IS_OFF;
        }
        else {
            if (buf->base != NULL) {
                free(buf->base);
            }
        }
    }
}

static int32_t tcpclient_reconnect(void *oObj) {
    int32_t status          = -1;
    UVClientObj *pObj       = (UVClientObj *)oObj;
    uv_tcp_t *client        = NULL;
    uv_connect_t *connect   = NULL;
    struct sockaddr_in addr;
    struct sockaddr_in dest;

    client = (uv_tcp_t *) malloc (sizeof(uv_tcp_t));
    ERRP(client == NULL, goto ERR1, 1, "malloc instance uv_tcp_t failure\n");

    connect = (uv_connect_t *) malloc (sizeof(uv_connect_t));
    ERRP(connect == NULL, goto ERR2, 1, "malloc instance uv_connect_t failure\n");

    status = uv_tcp_init((uv_loop_t *)pObj->loop, client);//初始化tcp server对象
    ERRP(status, goto ERR3, 1, "uv_tcp_init failure? (%s)\n", uv_strerror(status));

    //将ip和port数据填充到sockaddr_in结构体中
    status = uv_ip4_addr((const char *)pObj->ipCli, pObj->portCli, &addr);
    ERRP(status, goto ERR3, 1, "uv_ip4_addr client failure? (%s)\n", uv_strerror(status));

    status = uv_ip4_addr((const char *)pObj->ipServ, pObj->portServ, &dest);
    ERRP(status, goto ERR3, 1, "uv_ip4_addr server failure? (%s)\n", uv_strerror(status));

    status = uv_tcp_bind(client, (const struct sockaddr *)&addr, 0);//bind
    ERRP(status, goto ERR3, 1, "uv_tcp_bind server failure? (%s)\n", uv_strerror(status));

    client->data = (void *)pObj;

    status = uv_tcp_connect(connect, client, (const struct sockaddr*)&dest, on_connected);
    ERRP(status, goto ERR3, 1, "uv_tcp_connect failure? (%s)\n", uv_strerror(status));

    status = uv_read_start((uv_stream_t *) client, alloc_buffer, read_cb);
    ERRP(status, goto ERR3, 1, "uv_read_start failure\n", uv_strerror(status));

    pObj->inPriv    = client;
    pObj->backPriv  = client;

    LogPrintf(LogUVStream_Info, "reconnect ing handle:%p pObj:%p\n", client, pObj);

    return 0;
ERR3:
    free(connect);
ERR2:
    free(client);
ERR1:
    return -1;
}

/*
 * static int32_t tcpclient_disconnect(void *oObj) {
 *     UVClientObj *pObj = (UVClientObj *)oObj;
 * 
 *     uv_close((uv_handle_t *)pObj->inPriv, on_close);
 * 
 *     return 0;
 * }
 */


/*
 * static void timer_reconnect_cb(uv_timer_t* handle) {
 *     UVClientObj *pObj = NULL;
 * 
 *     if (handle) {
 *         pObj = (UVClientObj *)handle->data;
 *         if (pObj) {
 *             if (CONNECT_IS_OFF == pObj->isConnect) {
 *                 uv_timer_stop(handle);
 *                 tcpclient_reconnect(pObj);
 *                 uv_timer_start(handle, timer_reconnect_cb, 1000, 1000);
 *             }
 *         }
 *     }
 * }
 */

void *UVClientInit(void *_loop, UVClientConfig *config) {
    // int32_t status          = -1;
    UVClientObj *oObj       = NULL;
    uv_timer_t *uvTimer     = NULL;
    uv_loop_t *loop         = NULL;

    gPriv   = config->priv;
    gPrint  = config->print;
    UVStreamSetLogLevel(gPriv, (UVStreamPrintCb)gPrint);

    loop = (uv_loop_t *)_loop;//uv_loop_new();
    ERRP(loop == NULL, goto ERR0, 1, "uv_loop_new failure\n");

    oObj = (UVClientObj *) malloc (sizeof(UVClientObj));
    ERRP(oObj == NULL, goto ERR0, 1, "malloc instance UVClientObj failure\n");

    memset(oObj, 0x0, sizeof(*oObj));

    uvTimer = (uv_timer_t *) malloc (sizeof(uv_timer_t));
    ERRP(uvTimer == NULL, goto ERR1, 1, "malloc instance uv_timer_t failure\n");

    oObj->dealRun   = config->dealRun;
    oObj->priv      = config->priv;
    oObj->dealCmd   = config->dealCmd;
    oObj->newConnect= config->newConnect;

    strcpy((char *)oObj->ipServ, (const char *)config->ipServ);
    strcpy((char *)oObj->ipCli, (const char *)config->ipCli);
    oObj->portServ  = config->portServ;
    oObj->portCli   = config->portCli;
    oObj->loop      = loop;
    oObj->tid       = uv_thread_self();//syscall(224);

    LogPrintf(LogUVStream_Info, "tid:%ld portServ:%d ipServ:%s portCli:%d ipCli:%s\n", 
            oObj->tid, oObj->portServ, oObj->ipServ, oObj->portCli, oObj->ipCli);

    //失败与否不关心，我还可以重连
    tcpclient_reconnect(oObj);

/*
 *     status = uv_timer_init(loop, uvTimer);
 *     ERRP(status && !oObj->isConnect, goto ERR3, 1,
 *             "uv_timer_init failure? (%s)\n", uv_strerror(status));
 * 
 *     //定时器周期后续调整
 *     uvTimer->data = oObj;
 *     status = uv_timer_start(uvTimer, timer_reconnect_cb, 200, 200);
 *     ERRP(status && !oObj->isConnect, goto ERR4, 1,
 *             "uv_timer_start failure? (%s)\n", uv_strerror(status));
 *     oObj->uvTimer = uvTimer;
 *     LogPrintf(LogUVStream_Info, "Add client Timer handle:%p timeObj:%p\n", uvTimer, oObj);
 */

    return oObj;
/*
 * ERR4:
 *     uv_timer_stop(uvTimer);
 * ERR3:
 *     tcpclient_disconnect(oObj);
 *     free(uvTimer);
 */
ERR1:
    free(oObj);
ERR0:
    return NULL;
}

int32_t UVClientDestory(void *oObj) {
    UVClientObj *pObj   = NULL;

    if (oObj) {
        pObj = (UVClientObj *)oObj;

        uv_stop((uv_loop_t *)pObj->loop);

        /*
         * if (pObj->uvTimer) {
         *     uv_close((uv_handle_t *)pObj->uvTimer, on_close);
         * }
         */

        if (pObj->inPriv) {
            uv_close((uv_handle_t *)pObj->inPriv, on_close);
        }

        uv_run((uv_loop_t *)(pObj->loop), UV_RUN_ONCE);

        free(pObj);
    }

    return 0;
}

int32_t UVClientAlive(void *oObj) {
    UVClientObj *pObj   = NULL;

    if (oObj) {
        pObj = (UVClientObj *)oObj;

        return pObj->isConnect == CONNECT_IS_ON ? 1 : 0;
    }

    return 0;
}
