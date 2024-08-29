#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <mysystem.h>

static void *gPriv = NULL;
static MySystemPlatformPrint gPrint;
static LogMySystemPlatform gLevel = LogMySystemPlatform_Info;

/*************************************************/ 
/**************LogPrintf**************************/
/*************************************************/ 

static int MySystemPlatformLogPrintf(LogMySystemPlatform level,
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
        MySystemPlatformLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)     
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogMySystemPlatform_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogMySystemPlatform_Debug, __VA_ARGS__);
#endif

#ifndef ERRP
#define ERRP(con, ret, flag, ...)               \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif

int32_t systemArgs(void *oObj, const char *cmd, 
        const char *args, processs myprocess) {
    FILE *fp        = NULL;
    char buf[2048]  = {0};

    fp = popen(cmd, "r");
    ERRP(NULL == fp, return -1, 1, "popen error!\n");

    while (fgets(buf, sizeof(buf), fp)) {
        buf[strcspn(buf, "\n")] = '\0';
        if (NULL != strstr(buf, args)) {
            LogPrintf(LogMySystemPlatform_Info, "buf:%s args:%s\n", buf, args);
            myprocess(oObj, args, buf);
        }
    }

    pclose(fp);

    return 0;
}
