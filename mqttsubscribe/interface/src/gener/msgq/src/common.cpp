#include <uv.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <uvstream.h>
#include <pthread.h>
#include <stdarg.h>
#include <que.h>
// #include <memwatch.h>

#define MAX_STREAM_NUM (256)

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
#define ERRP(con, ret, flag, ...)           \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif

typedef struct UVStreamObj {
    UVCOMOBJ;
} UVStreamObj;

int32_t UVStreamSetLogLevel(void *priv, UVStreamPrintCb print) {
    gPriv   = priv;
    gPrint  = (UVStreamPrintCb)print;

    return 0;
}

void *UVStreamNewLoop(void) {
    uv_loop_t *loop     = NULL;
    UVStreamObj *pObj   = NULL;

    loop = uv_loop_new();
    if (loop == NULL) {
        fprintf (stderr, "uv_loop_new failure\n");
        return NULL;
    }

    pObj = (UVStreamObj *)malloc(sizeof(UVStreamObj));
    if (pObj == NULL) {
        fprintf (stderr, "malloc UVStreamObj failure\n");
        uv_loop_delete(loop);
        return NULL;
    }

    memset(pObj, 0x0, sizeof(*pObj));

    loop->data = pObj;

    return loop;
}

//必需跟随主线程
int32_t UVStreamRun(void *_loop) {
    uv_loop_t *loop     = NULL;

    if (_loop) {
        loop = (uv_loop_t *)_loop;

        int32_t status = uv_run(loop, UV_RUN_ONCE);
        return status;
    }
    else {
        return -1;
    }
}

static void timer_cb(uv_timer_t* handle) {
    UVStreamObj *pObj   = NULL;

    if (handle) {
        pObj = (UVStreamObj *)handle->data;
        if (pObj) {
            if (pObj->dealRun) {
                pObj->dealRun(pObj->priv);
            }
        }
    }
}

typedef struct {
    int32_t running;
    UVStreamSchduleConfig config;
} UVStreamSchduleConfig_;

/*
 * static void entryFunc(void* arg) {
 *     uv_loop_t *loop                 = NULL;
 *     UVStreamObj *pObj               = NULL;
 *     UVStreamSchduleConfig_ *config  = (UVStreamSchduleConfig_ *)arg;
 * 
 *     if (config->config.loop) {
 *         loop            = (uv_loop_t *)config->config.loop;
 *         pObj            = (UVStreamObj *)loop->data;
 *         if (pObj) {
 *             pObj->tid       = uv_thread_self();
 *         }
 *         
 *         config->running = 1;
 *         UVStreamRun(loop);
 *     }
 * 
 *     pthread_exit((void *)"stream common exit");
 * }
 */

static void echo_write(uv_write_t *req, int status) {
    if (status) {
        LogPrintf(LogUVStream_Error, "Write error? (%s)\n", uv_strerror(status));
    }

    if (req)
        free(req);
}

static void on_close(uv_handle_t *handle) {
    LogPrintf(LogUVStream_Info, "handle:%p on_close......\n", handle);

    if (handle) {
        if (handle != NULL)
            free(handle);
    }
}

/************************************************
 ***********************************************
 ***********************************************
 ***对外接口
 ***********************************************
 ***********************************************
 ***********************************************
 */
void *UVStreamTimerInit(void *_loop, void *priv,
        int32_t timeMs, UVStreamDealRun dealRun) {
    int32_t status      = -1;
    uv_loop_t *loop     = NULL;
    uv_timer_t *uvTimer = NULL;
    UVStreamObj *timeObj= NULL;

    if (_loop) {
        loop = (uv_loop_t *)_loop;
        timeObj = (UVStreamObj *) malloc (sizeof(UVStreamObj));
        ERRP(timeObj == NULL, goto ERR0, 1, "malloc UVStreamObj instance failure\n");
        memset(timeObj, 0x0, (size_t)sizeof(timeObj));

        timeObj->priv       = priv;
        timeObj->dealRun    = dealRun;

        uvTimer = (uv_timer_t *) malloc (sizeof(uv_timer_t));
        ERRP(uvTimer == NULL, goto ERR1, 1, "malloc uv_timer_t instance failure\n");

        status = uv_timer_init(loop, uvTimer);
        ERRP(status, goto ERR2, 1, "uv_timer_init failure\n");

        uvTimer->data = timeObj;

        status = uv_timer_start(uvTimer, timer_cb, 
                timeMs, timeMs);
        ERRP(status, goto ERR3, 1, "uv_timer_start failure\n");

        LogPrintf(LogUVStream_Info, "Add Timer handle:%p timeObj:%p\n", uvTimer, timeObj);

        return uvTimer;
    }

    return uvTimer;
ERR3:
    uv_timer_stop(uvTimer);
ERR2:
    free(uvTimer);
ERR1:
    free(timeObj);
ERR0:
    return NULL;
}

int32_t UVStreamTimerDestory(void *_loop, void *_uvTimer) {
    uv_timer_t *uvTimer     = NULL;
    uv_loop_t *loop         = NULL;
    UVStreamObj *timeObj    = NULL;

    if (_loop && _uvTimer) {
        uvTimer = (uv_timer_t *)_uvTimer;
        loop    = (uv_loop_t *)_loop;
        timeObj = (UVStreamObj *)uvTimer->data;

        LogPrintf(LogUVStream_Info, "Del Timer handle:%p timeObj:%p\n", uvTimer, timeObj);

        uv_close((uv_handle_t *)uvTimer, on_close);

        uv_run((uv_loop_t *)(loop), UV_RUN_ONCE);

        if (timeObj) {
            free(timeObj);
        }

        return 0;
    }

    return -1;
}

int32_t UVStreamSchdule(void *loop/* UVStreamSchduleConfig *config */) {
    return UVStreamRun((uv_loop_t *)loop);
/*
 *     int32_t status  = -1;
 *     uv_thread_t tid;
 *     UVStreamSchduleConfig_ config_;
 * 
 *     gPriv   = config->priv;
 *     gPrint  = config->print;
 * 
 *     config_.running = 0;
 *     config_.config  = *config;
 * 
 *     status = uv_thread_create(&tid, entryFunc, &config_);
 *     ERRP(status, return -1, 1, "uv_thread_create failure\n");
 * 
 *     while(!config_.running) {
 *         sleep(1);
 *     }
 */

    // return 0;
}

int32_t UVStreamUnSchdule(void *_loop) {
/*
 *     UVStreamObj *pObj   = NULL;
 *     uv_loop_t *loop     = (uv_loop_t *)_loop;
 *     void *exitPtr       = NULL;
 * 
 *     if (loop) {
 *         uv_stop(loop);
 * 
 *         pObj = (UVStreamObj *)loop->data;
 *         if (pObj) {
 *             pthread_join(pObj->tid, &exitPtr);
 * 
 *             if (exitPtr) {
 *                 LogPrintf(LogUVStream_Info, "===> %s <===\n", exitPtr);
 *             }
 *         }
 *     }
 */

    return 0;
}

int32_t UVStreamDelete(void *_loop) {
    UVStreamObj *pObj   = NULL;
    uv_loop_t *loop     = (uv_loop_t *)_loop;

    if (loop) {
        uv_print_all_handles(loop, stdout);
        pObj = (UVStreamObj *)loop->data;
        if (pObj) {

            uv_run((uv_loop_t *)(loop), UV_RUN_ONCE);
            uv_loop_delete(loop);

            free(pObj);
        }
    }

    return 0;
}

/*uv发送数据接口
 *参数1:客户端句柄
 *参数2:消息首地址
 *参数3:消息实际长度
 *参数4:Ack消息首地址
 *参数5:Ack消息长度返回地址
 **/
int32_t UVClientSend(void *client, 
        void *data, int32_t len) {
    int32_t status      = -1;
    uv_write_t* req     = NULL;
    uv_stream_t *stream = NULL;
    UVStreamObj *pObj   = NULL;
    uv_buf_t uvBuf;

    if (client) {
        pObj = (UVStreamObj *)(client);
        if (pObj) {
            stream = (uv_stream_t *)pObj->inPriv;
            if (stream) {
                if (UVClientAlive(pObj)) {
                    req = (uv_write_t*)malloc(sizeof(uv_write_t));
                    if (req) {
                        uvBuf = uv_buf_init((char *)data, len);
                        return uv_write(req, stream, &uvBuf, 1, echo_write);
                    }
                }
            }
        }
    }

    return status;
}

int32_t UVClientRecv(void *client, void *ack, int32_t *ackLen) {
    int32_t status      = -1;
    uv_stream_t *stream = NULL;
    UVStreamObj *pObj   = NULL;

    pObj = (UVStreamObj *)client;
    if (pObj) {
        stream = (uv_stream_t *)pObj->inPriv;
        if (stream) {
            if (UVClientAlive(pObj)) {
                if (ack != NULL && ackLen != NULL) {
                    uv_stream_read((uv_stream_t *)client);
                    //消息ACK有效
                    if (pObj->msgLen > 0) {
                        if (*ackLen >= pObj->msgLen ) {
                            *ackLen = pObj->msgLen;
                            memcpy(ack, pObj->ackMsg, pObj->msgLen);
                            return 0;
                        }
                    }
                }
            }
        }
    }


    return status;
}


/*主要用于服务端发送ACK消息
 * 参数1:对应客户端uv_stream_t句柄
 * 参数2:消息首地址 
 * 参数3:消息长度 
 */
int32_t UVServerSend(void *client, void *data, int32_t len) {
    int32_t status      = -1;
    uv_write_t* req     = NULL;
    uv_stream_t *stream = (uv_stream_t *)client;
    UVStreamObj *pObj   = NULL;
    uv_buf_t uvBuf;

    if (stream) {
        pObj = (UVStreamObj *)(stream->data);
        if (pObj) {
            req = (uv_write_t*)malloc(sizeof(uv_write_t));
            if (req) {
                uvBuf = uv_buf_init((char *)data, len);
                status = uv_write(req, stream, &uvBuf, 1, echo_write);
            }
        }
    }

    return status;
}
