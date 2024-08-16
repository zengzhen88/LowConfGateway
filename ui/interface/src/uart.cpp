#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <termios.h>
#include <errno.h>
#include <tska_thr.h>
#include <uart.h>

static void *gPriv = NULL;
static UartPrint gPrint;
static LogUart gLevel = LogUart_Info;


/*************************************************/ 
/**************LogPrintf**************************/
/*************************************************/ 

static int UartLogPrintf(LogUart level,
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
        UartLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)     
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogUart_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogUart_Debug, __VA_ARGS__);
#endif

#ifndef ERRP
#define ERRP(con, ret, flag, ...)               \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif

typedef struct {
    int32_t fd;
    char tty[128];
    int32_t baudrate;
    int32_t c_flow;
    int32_t bits;
    int32_t stop;
    char parity;

    int32_t step;
    int32_t start;
    int32_t end;
    int32_t recv_length;
    char recv_data[512];
    /* malloc realloc */

    void *priv;
    UartSend send;
    UartRecv recv;

    TSKA_ThrHndl uartHndl;
} UartObj;

//设置日志输出对象，主要看你想输出到哪里，就对应填充回调
int32_t UartInitLog(void *priv, UartPrint print) {
    gPriv   = priv;
    gPrint  = print;

    return 0;
}

//设置日志输出等级，看需要，调试、错误、宕机等
int32_t UartSetLogLevel(LogUart level) {
    gLevel  = level;

    return 0;
}

static int32_t UartOpen(UartObj *pObj, char *tty) {
    /* int32_t status  = -1; */

    pObj->fd = open(tty, O_RDWR | O_NOCTTY | O_NDELAY);
    ERRP(pObj->fd < 0, goto ERR0, 1, "open %s failure\n", tty);

    /*清除非阻塞标志*/
    /* status = fcntl(pObj->fd, F_SETFL, 0); */
    /* ERRP(status < 0, goto ERR1, 1, "fcntl %s failure\n", tty); */

    return 0;
/* ERR1: */
    /* if (pObj->fd >= 0) close(pObj->fd); */
ERR0:
    return -1;
}

static int32_t UartClose(UartObj *pObj) {
    if (pObj->fd >= 0) {
        close(pObj->fd);
    }

    return 0;
}

static ssize_t UartSafeWrite(UartObj *pObj, 
        const void *vptr, size_t n) {
    size_t  nleft;
    ssize_t nwritten;
    const unsigned char *ptr;

    ptr = (const unsigned char *)vptr;
    nleft = n;

    while (nleft > 0) {
        if ((nwritten = write(pObj->fd, ptr, nleft)) <= 0) {
            if (nwritten < 0/*  && errno == EINTR */)
                nwritten = 0;
            else
                return -1;
        }
        nleft -= nwritten;
        ptr   += nwritten;
    }

    return(n);
}

static ssize_t UartSafeRead(UartObj *pObj,
        unsigned char *vptr, size_t n) {
    size_t nleft;
    ssize_t nread;
    unsigned char *ptr;

    ptr = (unsigned char *)vptr;
    nleft = n;

    int total = 0;
    while (nleft > 0) {
        nread = read(pObj->fd, ptr + total, nleft);
        if (nread <= 0) {
            break;
        }

        total += nread;
        nleft -= nread;
    }

    return (n - nleft);
}

static int32_t UartSet(UartObj *pObj, 
        int32_t baude, int32_t c_flow, 
        int32_t bits, char parity, 
        int32_t stop) {
    int32_t status  = -1;
    struct termios options;

    /*获取终端属性*/
    status = tcgetattr(pObj->fd, &options);
    ERRP(status < 0, return -1, 1, "tcgetattr failure(%d)\n", errno);

    /*设置输入输出波特率，两者保持一致*/
    switch (baude) {
        case 4800:
            cfsetispeed(&options, B4800);
            cfsetospeed(&options, B4800);
            break;
        case 9600:
            cfsetispeed(&options, B9600);
            cfsetospeed(&options, B9600);
            break;
        case 19200:
            cfsetispeed(&options, B19200);
            cfsetospeed(&options, B19200);
            break;
        case 38400:
            cfsetispeed(&options, B38400);
            cfsetospeed(&options, B38400);
            break;
        case 115200:
            cfsetispeed(&options, B115200);
            cfsetospeed(&options, B115200);
            break;
        default:
            LogPrintf(LogUart_Error, "Unkown baude!\n");
            return -1;
    }

    /*设置控制模式*/
    options.c_cflag |= CLOCAL;//保证程序不占用串口
    options.c_cflag |= CREAD;//保证程序可以从串口中读取数据

    /*设置数据流控制*/
    switch (c_flow) {
        case 0://不进行流控制
            options.c_cflag &= ~CRTSCTS;
            break;
        case 1://进行硬件流控制
            options.c_cflag |= CRTSCTS;
            break;
        case 2://进行软件流控制
            options.c_cflag |= IXON|IXOFF|IXANY;
            break;
        default:
            LogPrintf(LogUart_Error, "Unkown c_flow!\n");
            return -1;
    }

    /*设置数据位*/
    switch (bits) {
        case 5:
            options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS5;
            break;
        case 6:
            options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS6;
            break;
        case 7:
            options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS8;
            break;
        default:
            LogPrintf(LogUart_Error, "Unkown bits!\n");
            return -1;
    }

    /*设置校验位*/
    switch (parity) {
        /*无奇偶校验位*/
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;//PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~INPCK;//INPCK：使奇偶校验起作用
                                      //options.c_cflag &= (~ISTRIP);////禁止将所有接收的字符裁减为7比特

            break;
            /*设为空格,即停止位为2位*/
        case 's':
        case 'S':
            options.c_cflag &= ~PARENB;//PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~CSTOPB;//CSTOPB：使用两位停止位
            break;
            /*设置奇校验*/
        case 'o':
        case 'O':
            options.c_cflag |= PARENB;//PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag |= PARODD;//PARODD：若设置则为奇校验,否则为偶校验
            options.c_cflag |= INPCK;//INPCK：使奇偶校验起作用
            options.c_cflag |= ISTRIP;//ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
            break;
            /*设置偶校验*/
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;//PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~PARODD;//PARODD：若设置则为奇校验,否则为偶校验
            options.c_cflag |= INPCK;//INPCK：使奇偶校验起作用
            options.c_cflag |= ISTRIP;//ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
            break;
        default:
            LogPrintf(LogUart_Error, "Unkown parity!\n");
            return -1;
    }

    /*设置停止位*/
    switch (stop) {
        case 1:
            options.c_cflag &= ~CSTOPB;//CSTOPB：使用两位停止位
            break;
        case 2:
            options.c_cflag |= CSTOPB;//CSTOPB：使用两位停止位
            break;
        default:
            LogPrintf(LogUart_Error, "Unkown stop!\n");
            return -1;
    }

    options.c_iflag &= ~ISTRIP;

    /*设置输出模式为原始输出*/
    options.c_oflag &= ~OPOST;//OPOST：若设置则按定义的输出处理，否则所有c_oflag失效

    /*设置本地模式为原始模式*/
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    /*
     *ICANON：允许规范模式进行输入处理
     *ECHO：允许输入字符的本地回显
     *ECHOE：在接收EPASE时执行Backspace,Space,Backspace组合
     *ISIG：允许信号
     */

    /*设置等待时间和最小接受字符*/
    options.c_cc[VTIME] = 0;//可以在select中设置
    options.c_cc[VMIN] = 1;//最少读取一个字符

    /*如果发生数据溢出，只接受数据，但是不进行读操作*/
    tcflush(pObj->fd, TCIFLUSH);

    options.c_cflag |= CS8;
    //预防0D强制变0A
    options.c_iflag &= ~ICRNL;

    options.c_iflag &= ~(ICRNL | IXON); 

    /*激活配置*/
    status = tcsetattr(pObj->fd, TCSANOW, &options);
    ERRP(status < 0, return -1, 1, "tcsetattr failure(%d)\n", errno);

    return 0;
}

static int32_t UartWrite(UartObj *pObj, 
        const char *writes, int32_t len) {
    ssize_t cnt = 0;

    cnt = UartSafeWrite(pObj, writes, (size_t)len);
    ERRP(cnt < 0, return -1, 1, "safe_write failure(%d)\n", errno);

    return cnt;
}

static int32_t UartRead(UartObj *pObj, 
        char *readss, int32_t len) {
    ssize_t cnt = 0;

    cnt = UartSafeRead(pObj, (unsigned char *)readss, (size_t)len);
    ERRP(cnt < 0, return -1, 1, "safe_read failure(%d)\n", errno);

    return cnt;
}

static char *UartRecvParse(UartObj *pObj, 
        char *recv, int32_t length) {
    int32_t msgvalid    = 0;
    char *strings       = recv;
    char *chrr          = NULL;

    if (pObj->recv_length > 0) {
        memcpy(strings - pObj->recv_length, pObj->recv_data, pObj->recv_length);
        length  += pObj->recv_length;
        strings -= pObj->recv_length;
        strings[length] = '\0';
        pObj->recv_length   = 0;
    }

    chrr = strchr(strings, '\r');
    if (chrr) {
        if (chrr[1] == '\n') {
            msgvalid = 1;
            chrr[0] = '\0';
            if (strlen(chrr + 2) > 0) {
                chrr += 2;
                memcpy(pObj->recv_data, chrr, strlen(chrr));//只拷贝有效数据，不包括'\0';
                pObj->recv_length = strlen(chrr);
            }
        }
        else {
            memcpy(pObj->recv_data, strings, length);
            pObj->recv_length += length;
        }
    }
    else {
        memcpy(pObj->recv_data, strings, length);
        pObj->recv_length += length;
    }

    if (msgvalid) {
        pObj->send(pObj->priv, strings, TSKA_TIMEOUT_FOREVER);

        return strings;
    }

    return NULL;
}

static void *UartEntryFunc(void *args) {
    int32_t status          = -1;
    int32_t length          = 0;
    LogPrintf(LogUart_Info, "enter UartEntryFunc\n");
    UartObj *pObj           = (UartObj *)args;
#define RECVMAXLENGTH (1024)
    char recv[RECVMAXLENGTH];
    LogPrintf(LogUart_Info, "enter UartEntryFunc\n");
    char *srecv             = (char *)&recv[RECVMAXLENGTH>>1];
    LogPrintf(LogUart_Info, "enter UartEntryFunc\n");
    char *brecv             = NULL;
    LogPrintf(LogUart_Info, "enter UartEntryFunc\n");

    while (1) {
    LogPrintf(LogUart_Info, "enter UartEntryFunc\n");
        status = UartRead(pObj, srecv, RECVMAXLENGTH >> 1);
        if (status > 0) {
    LogPrintf(LogUart_Info, "enter UartEntryFunc:%c status:%d\n", srecv[0], status);
    int32_t index = 0;
    for (index = 0; index < status; index++) {
        printf("%c", srecv[index]);
    }
    printf ("\n");
    continue;
            srecv[status] = '\0';
            brecv = UartRecvParse(pObj, srecv, status);
            if (NULL != brecv) {
                LogPrintf(LogUart_Debug, "UART AT Recv: %s\n", brecv);
            }
            continue;
        }

        length = sizeof(recv);
        status = pObj->recv(pObj->priv, recv, &length, 1);
        if (!status) {
            /* LogPrintf(LogUart_Debug, "UART AT SEND: %s\n", recv); */
            strcat(recv, "\r\n");
            LogPrintf(LogUart_Debug, "UART AT SEND: %s\n", recv);
            UartWrite(pObj, recv, strlen(recv));
        }
    }

    LogPrintf(LogUart_Debug, "out UartEntryFunc\n");

    return NULL;
}

static int32_t UartStart(UartObj *pObj) {
    int32_t status = -1;

    status = TSKA_thrCreate(&pObj->uartHndl, 
            UartEntryFunc, 30, 0, pObj);
    ERRP(status, goto ERR0, 1, "Create UartEntryFunc thrd failure\n");

    return 0;
ERR0:
    return -1;
}

void *UartInit(UartConfig *config) {
    int32_t status      = -1;
    UartObj *pObj     = NULL;

    pObj = (UartObj *) malloc (sizeof(*pObj));
    ERRP(NULL == pObj, goto ERR0, 1, "malloc UartObj failure\n");
    memset(pObj, 0x0, sizeof(*pObj));

    pObj->priv      = config->priv;
    pObj->send      = config->send;
    pObj->recv      = config->recv;
    pObj->baudrate  = config->baudrate;
    pObj->c_flow    = config->c_flow;
    pObj->bits      = config->bits;
    pObj->parity    = config->parity;
    pObj->stop      = config->stop;
    strcpy(pObj->tty, config->tty);

    status = UartOpen(pObj, pObj->tty);
    ERRP(status < 0, goto ERR1, 1, "UartOpen failure\n");

    status = UartSet(pObj, pObj->baudrate, 
            pObj->c_flow, pObj->bits, 
            pObj->parity, pObj->stop);
    ERRP(status < 0, goto ERR2, 1, "UartSet failure\n");

    status = UartStart(pObj);
    ERRP(status < 0, goto ERR1, 1, "UartStart failure\n");

    return pObj;
ERR2:
    UartClose(pObj);
ERR1:
    if (pObj) free(pObj);
ERR0:
    return NULL;
}
