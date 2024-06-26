
#ifndef _TSKA_DEBUG_H_
#define _TSKA_DEBUG_H_

#include <stdio.h>

/*module name*/
#define TSKA_MODULE_NAME "Tska"

// printf wrapper that can be turned on and off by defining/undefining
#define TSKA_printf(format,...)          printf("FILE: %s, LINE: %d: "format"\n",__FILE__, __LINE__, ##__VA_ARGS__)
#define TSKA_printf_urgent(format,...) printf("FILE: %s, LINE: %d: "format"\n", __FILE__,__LINE__, ##__VA_ARGS__)

#define TSKA_assert(x)  \
    { \
        if( (x) == 0) { \
            printf(" ASSERT (%s|%s|%d)\r\n", __FILE__, __func__, __LINE__); \
        } \
    } 

#define TSKA_DEBUG
#define TSKA_DEBUG_WAIT

#define TSKA_ERROR TSKA_printf

#define TSKA_assertSuccess(ret) TSKA_assert(ret==TSKA_SOK)

#include <sys/prctl.h>
#define TSKTHRDSETPROCNAME(name)           \
    do {                                \
        char sbuf[16] = {0};             \
        snprintf (sbuf, 16, "%s", name);\
        prctl(PR_SET_NAME, sbuf);       \
    } while (0)

#endif  //_TSKA_DEBUG_H_

