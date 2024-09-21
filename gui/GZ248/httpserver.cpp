#include <stdint.h>
#include <string.h>
#include <httpserver/HttpExample.h>
#include <pthread.h>
#include <QString>

QString gDownload;
int32_t gPort;
static int32_t gJustrunonetimes = 0;

void *func(void *arg) {
    //QString download = *((QString *)arg);

    printf ("gDownload:%s\n", gDownload.toStdString().c_str());


    HttpExample *httpTest = new HttpExample();
    httpTest->start(gDownload.toStdString().c_str(), gPort);

    while(true) {
        httpTest->run(2);
    }

    return NULL;
}

void runHttpServer(QString downloadDir, int32_t port) {
    pthread_t pid;

    if (gJustrunonetimes) {
        printf ("httpserver alreay running\n");
        return;
    }

    gJustrunonetimes = 1;
    gDownload = downloadDir;
    gPort = port;
    pthread_create(&pid, NULL, func, NULL);
}


