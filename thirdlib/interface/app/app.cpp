#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <execinfo.h>

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <gateway.h>
#include <signal.h>
#include <sys/time.h>

#include <sys/syscall.h> 
pid_t gettid(void)
{
    return syscall(SYS_gettid);
}

int32_t runPrint(void *data, const char *strings) {
    fprintf (stderr, "%s", strings);
    return 0;
}

int32_t main(int32_t args, char *argv[]) {
    LogGateway level = LogGateway_Cnt;

    if (args != 2) {
        fprintf (stderr, "input args: ./app_Gateway.out loglevel(Urgent/Fatal/Error/Warning/Info/Debug/Trace/Detail)\n");
        return -1;
    }

    if (!strcmp(argv[1], "Urgent")) {
        level = LogGateway_Urgent;
    }
    else if (!strcmp(argv[1], "Fatal")) {
        level = LogGateway_Fatal;
    }
    else if (!strcmp(argv[1], "Error")) {
        level = LogGateway_Error;
    }
    else if (!strcmp(argv[1], "Warning")) {
        level = LogGateway_Warning;
    }
    else if (!strcmp(argv[1], "Info")) {
        level = LogGateway_Info;
    }
    else if (!strcmp(argv[1], "Debug")) {
        level = LogGateway_Debug;
    }
    else if (!strcmp(argv[1], "Trace")) {
        level = LogGateway_Trace;
    }
    else if (!strcmp(argv[1], "Detail")) {
        level = LogGateway_Detail;
    }
    else {
        level = LogGateway_Info;
    }

    GatewayInitLog(NULL, runPrint);
    GatewaySetLogLevel(level);

    GatewayInit();

    while (1) {
        sleep(1);
    }

    return 0;
}
