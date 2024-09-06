#include <stdio.h>
#include <uv.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <uvstream.h>
#include <unistd.h>
#include <sys/syscall.h>
// #include <memwatch.h>

/*************************************************/
/**************LogPrintf**************************/
/*************************************************/

static void *gPriv = NULL;
static UVStreamPrintCb gPrint;
static LogUVStream gLevel = LogUVStream_Info;

/*************************************************/
/**************LogPrintf**************************/
/*************************************************/

static int UVStreamLogPrintf(LogUVStream level,
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
    UVStreamLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

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

typedef struct UVServerObj {
    UVCOMOBJ

    //全局管理
    void *serverObj;
    void *uvTimer;
} UVServerObj;

typedef struct {
    int32_t len;
    int32_t count;
    int32_t rd;
    int32_t wd;
    long *data;
} QueObj;

void *QueInit(int32_t len) {
    QueObj *pObj = (QueObj *) malloc (sizeof(QueObj));
    ERRP(pObj == NULL, goto ERR0, 1, "malloc QueObj failure\n");

    memset(pObj, 0x0, sizeof(*pObj));

    pObj->len   = len;
    pObj->data  = (long *) malloc (pObj->len * sizeof(long));
    ERRP(pObj->data == NULL, goto ERR1, 1, "malloc pObj->data failure\n");

    return pObj;
ERR1:
    free(pObj);
ERR0:
    return NULL;
}

void QueDestory(void *oObj) {
    QueObj *pObj = (QueObj *)oObj;

    if (pObj) {
        if (pObj->data) {
            free(pObj->data);
        }

        free(pObj);
    }
}

int32_t QueDQ(void *oObj, long *data) {
    QueObj *pObj = (QueObj *)oObj;

    if (pObj) {
        if (pObj->rd == pObj->wd) return -1;

        *data = pObj->data[pObj->rd];
        pObj->rd = (pObj->rd + 1) % pObj->len;
        pObj->count--;

        return 0;
    }

    return -1;
}

int32_t QueEQ(void *oObj, long data) {
    QueObj *pObj = (QueObj *)oObj;

    if (pObj) {
        if ((pObj->wd + 1) % pObj->len == pObj->rd) return -1;

        pObj->data[pObj->wd] = data;
        pObj->wd = (pObj->wd + 1) % pObj->len;
        pObj->count++;

        return 0;
    }

    return -1;
}

int32_t QueCount(void *oObj) {
    QueObj *pObj = (QueObj *)oObj;

    if (pObj) {
        return pObj->count;
    }

    return 0;
}

//负责为新来的消息申请空间
static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->len    = suggested_size;
    buf->base   = (char *)(malloc(suggested_size));
}

static void on_client_close(uv_handle_t *handle) {
    UVServerObj *clientObj  = NULL;
    UVServerObj *serverObj  = NULL;
    map<void *, void *>::iterator iti;

    clientObj = (UVServerObj *)((uv_stream_t *)handle)->data;
    if (clientObj) {
        if (clientObj->newConnect) {
            clientObj->newConnect(clientObj->priv, handle, 0, 0, 0);
        }

        serverObj = (UVServerObj *)clientObj->serverObj;
        if (serverObj) {
            iti = ((map<void *, void *> *)(serverObj->handMap))->find(clientObj);
            if (iti != ((map<void *, void *> *)(serverObj->handMap))->end()) {
                if (iti->first) {
                    if (iti->first == clientObj) {
                        LogPrintf(LogUVStream_Info, "unmap client:%p clientObj:%p\n", 
                                clientObj, handle);
                        ((map<void *, void *> *)(serverObj->handMap))->erase(iti);
                    }
                }
            }
        }

        free(clientObj);
    }

    if (handle != NULL)
        free(handle);

    LogPrintf(LogUVStream_Info, "handle:%p pObj:%p on_client_close... \n",
            handle, clientObj);
}

static void on_close(uv_handle_t *handle) {
    UVServerObj *pObj = NULL;

    if (handle != NULL) {
        pObj = (UVServerObj *)((uv_stream_t *)handle)->data;
        if (pObj) {
            LogPrintf(LogUVStream_Info, "handle:%p pObj:%p on_close.... \n", handle, pObj);
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
    int32_t fillLength  = 0;
    UVServerObj *pObj   = NULL;

    pObj = (UVServerObj *)client->data;
    if (pObj) {
        if (nread > 0) {
            if (pObj->dealCmd) {
                fillLength = nread;
                pObj->dealCmd(pObj->priv, client,
                        buf->base, (int32_t *)&fillLength);
            }

            //释放之前申请的资源
            if (buf->base != NULL) {
                free(buf->base);
            }
            return;
        } 
        else if (nread < 0) {
            //释放之前申请的资源
            if (nread != UV_EOF) {
                LogPrintf(LogUVStream_Error, 
                        "handle:%p pObj:%p Read error? (%s)\n", 
                        client, pObj, uv_err_name(nread));
            }
            else {
                LogPrintf(LogUVStream_Error, "handle:%p pObj:%p client disconnect\n", 
                        client, pObj);
            }

            if (!pObj->markClose) {
                uv_close((uv_handle_t *) client, on_client_close);
                pObj->markClose = 1;
            }

            if (buf->base != NULL) {
                free(buf->base);
            }
        }
        else {
            if (buf->base != NULL) {
                free(buf->base);
            }
        }
    }
}

/**
 *
 * @param:  stream  libuv的tcp stream对象
 * @param:  status  状态，小于0表示新连接有误
 * @author: sherlock
 */
static void on_new_connection(uv_stream_t *stream, int status) {
    uint32_t peer_port      = 0;
    uint32_t peer_ipaddr    = 0;
    uv_tcp_t *client        = NULL;
    UVServerObj *clientObj  = NULL;
    UVServerObj *serverObj  = NULL;
    int32_t remote_len      = sizeof(struct sockaddr);
    char remote_name[24]    = {0};
    struct sockaddr_in remote_in;
    struct sockaddr remote;

    ERRP(status < 0, return, 1, 
            "New connection failure? error(%s)\n", 
            uv_strerror(status));

    //为tcp client申请资源
    client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
    ERRP(client == NULL, return, 1, "malloc uv_tc_t instance failure\n");

    serverObj = (UVServerObj *)stream->data;
    if (serverObj) {
        clientObj = (UVServerObj *) malloc (sizeof(UVServerObj));
        ERRP(clientObj == NULL, goto ERR00, 1, "malloc UVServerObj instance failure\n");

        memset(clientObj, 0x0, sizeof(*clientObj));

        clientObj->dealCmd      = serverObj->dealCmd;
        clientObj->dealRun      = serverObj->dealRun;
        clientObj->print        = serverObj->print;
        clientObj->priv         = serverObj->priv;
        clientObj->loop         = serverObj->loop;
        clientObj->serverObj    = serverObj;
        clientObj->inPriv       = client;
        clientObj->newConnect   = serverObj->newConnect;

        LogPrintf(LogUVStream_Info, "client:%p\n", clientObj->inPriv);
        ((map<void *, void *> *)(serverObj->handMap))->emplace(
                (void *)clientObj, (void *)clientObj->inPriv);
    }

    status = uv_tcp_init((uv_loop_t *)serverObj->loop, client);//初始化tcp client句柄
    ERRP(status, goto ERR0, 1, 
            "uv_tcp_init failure? error(%s)\n", 
            uv_strerror(status));

    client->data = clientObj;

    //判断accept是否成功
    status = uv_accept(stream, (uv_stream_t *) client);
    ERRP(status, goto ERR1, 1, 
            "uv_accept failure? error(%s)\n", 
            uv_strerror(status));

    if (0 == uv_tcp_getpeername((uv_tcp_t*)client, &remote, &remote_len)) {
        remote_in = *((struct sockaddr_in*)&remote);
        uv_ip4_name(&remote_in,remote_name, remote_len);

        peer_port     = ntohs(remote_in.sin_port);
        peer_ipaddr   = inet_addr(remote_name);
        LogPrintf(LogUVStream_Info, "%s:%d connected\n", remote_name, peer_port);

        //从传入的stream中读取数据，read_cb会被多次调用，
        //直到数据读完，或者主动调用uv_read_stop方法停止
        status = uv_read_start((uv_stream_t *)client, alloc_buffer, read_cb);
        ERRP(status, goto ERR1, 1, 
                "uv_read_start failure? error(%s)\n", 
                uv_strerror(status));

        if (clientObj->newConnect) {
            clientObj->newConnect(clientObj->priv, client, peer_ipaddr, peer_port, 1);
        }

        LogPrintf(LogUVStream_Error, "handle:%p pObj:%p on_new_connection...\n", client, clientObj);
        return;
    }

ERR1:
    uv_close((uv_handle_t *)client, NULL);
ERR0:
    free(clientObj);
ERR00:
    free(client);

    return;
}

/*
 * static void timer_cb(uv_timer_t* handle) {
 *     UVServerObj *pObj = NULL;
 * 
 *     if (handle) {
 *         pObj = (UVServerObj *)handle->data;
 *         if (pObj) {
 *             if (pObj->dealRun) {
 *                 pObj->dealRun(pObj->priv);
 *             }
 *         }
 *     }
 * }
 */

void *UVServerInit(void *_loop, UVServerConfig *config) {
    int32_t status                  = -1;
    uv_tcp_t *server                = NULL;
    UVServerObj *oObj               = NULL;
    // uv_timer_t *uvTimer             = NULL;
    uv_loop_t *loop                 = NULL;
    map<void *, void *> *handMap    = NULL;
    struct sockaddr_in addr;

    //初始化日志输出接口
    gPriv   = config->priv;
    gPrint  = config->print;

    UVStreamSetLogLevel(gPriv, (UVStreamPrintCb)gPrint);

    loop = (uv_loop_t *)_loop;
    ERRP(loop == NULL, goto ERR0, 1, "uv_loop_new failure\n");

    server  = (uv_tcp_t *) malloc (sizeof(uv_tcp_t));
    ERRP(server == NULL, goto ERR0, 1, "malloc instace uv_tcp_t failure\n");
    
    memset(server, 0x0, sizeof(*server));

    oObj    = (UVServerObj *) malloc (sizeof(UVServerObj));
    ERRP(oObj == NULL, goto ERR1, 1, "malloc instance UVServerObj failure\n");

    memset(oObj, 0x0, sizeof(*oObj));

/*
 *     uvTimer = (uv_timer_t *) malloc (sizeof(uv_timer_t));
 *     ERRP(uvTimer == NULL, goto ERR2, 1, "malloc instance uv_timer_t failure\n");
 * 
 *     memset(uvTimer, 0x0, sizeof(*uvTimer));
 */

    oObj->handMap = new map<void *, void *>;
    ERRP(oObj->handMap == NULL, goto ERR3, 1, "oObj->handMap failure\n");

    LogPrintf (LogUVStream_Info, "ip:%s port:%d\n", config->ipServ, config->port);

    oObj->priv      = config->priv;
    oObj->dealCmd   = config->dealCmd;
    oObj->dealRun   = config->dealRun;
    oObj->newConnect= config->newConnect;
    oObj->print     = config->print;
    oObj->loop      = loop;
    oObj->tid       = uv_thread_self();//syscall(224);
    oObj->inPriv    = server;
    /*
     * oObj->uvTimer   = uvTimer;
     */

    /* loop->data      = oObj; */
    LogPrintf(LogUVStream_Info, "server %p inPriv:%p\n", server, oObj->inPriv);

    //将ip和port数据填充到sockaddr_in结构体中
    status = uv_ip4_addr((const char *)config->ipServ, config->port, &addr);
    ERRP(status, goto ERR4, 1, "uv_ip4_addr failure? (%s)\n", uv_strerror(status));

    //初始化tcp server对象
    status = uv_tcp_init(loop, server);
    ERRP(status, goto ERR4, 1, "uv_tcp_init failure? (%s)\n", uv_strerror(status));

    status = uv_tcp_bind(server, (const struct sockaddr *)&addr, 0);//bind
    ERRP(status, goto ERR5, 1, "uv_tcp_bind failure? (%s)\n", uv_strerror(status));

    server->data = (void *)oObj;

    status = uv_listen((uv_stream_t *)server, config->listMaxNum, on_new_connection);//listen
    ERRP(status, goto ERR5, 1, "uv_listen %s port:%d listMaxNum:%d failure, (%s)\n", 
            config->ipServ, config->port, config->listMaxNum, uv_strerror(status));

/*
 *     status = uv_timer_init(loop, uvTimer);
 *     ERRP(status, goto ERR5, 1,
 *             "uv_timer_init failure? (%s)\n", uv_strerror(status));
 * 
 *     uvTimer->data = oObj;
 *     status = uv_timer_start(uvTimer, timer_cb, 50, 50);
 *     ERRP(status, goto ERR6, 1,
 *             "uv_timer_start failure? (%s)\n", uv_strerror(status));
 * 
 *     LogPrintf(LogUVStream_Info, "Add Server Timer handle:%p timeObj:%p\n", uvTimer, oObj);
 */
    LogPrintf(LogUVStream_Info, "UVServInit Suc\n");

    return oObj;
/*
 * ERR6:
 *     uv_timer_stop(uvTimer);
 */
ERR5:
    uv_close((uv_handle_t *)server, on_close);
ERR4:
    handMap = (map<void *, void *> *)oObj->handMap;
    delete handMap;
ERR3:
/*
 *     free(uvTimer);
 * ERR2:
 */
    free(oObj);
ERR1:
    free(server);
ERR0:
    return NULL;
}

int32_t UVServerDestory(void *oObj) {
    UVServerObj *pObj           = NULL;
    UVServerObj *clientObj      = NULL;
    map<void *, void *> *handMap= NULL;

    if (oObj) {
        pObj = (UVServerObj *)oObj;
        if (pObj) {
            uv_stop((uv_loop_t *)pObj->loop);

            /*
             * if (pObj->uvTimer) {
             *     uv_close((uv_handle_t *)pObj->uvTimer, on_close);
             * }
             */

            handMap = (map<void *, void *> *)pObj->handMap;
            for(auto iti = handMap->begin(); iti != handMap->end(); iti++){
                if (iti->first) {
                    clientObj = (UVServerObj *)iti->first;
                    if (!clientObj->markClose) {
                        uv_close((uv_handle_t *)iti->second, on_client_close);
                        clientObj->markClose = 1;
                    }
                }
            }

            uv_run((uv_loop_t *)(pObj->loop), UV_RUN_ONCE);

            if (pObj->inPriv) {
                uv_close((uv_handle_t *)pObj->inPriv, on_close);
            }

            uv_run((uv_loop_t *)(pObj->loop), UV_RUN_ONCE);

            delete handMap;

            free(pObj);
        }
    }

    return 0;
}
