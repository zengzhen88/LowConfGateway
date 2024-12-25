#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <message.h>

int32_t req = 0;
int32_t rel = 0;

Message *RequestMessage1(void **data, int32_t size) {
    Message *mess = (Message *) malloc (sizeof(*mess));
    if (mess) {
        if (size > 0) {
            mess->data = *data;
            if (mess->data) {
                req++;
                mess->length = 0;
                mess->size   = size;
                return mess;
            }
            else {
                free(mess);
            }
        }
        else {
            /*应该可以允许为0*/
            mess->size   = 0;
            mess->length = 0;
            mess->data   = NULL;
            return mess;
        }
    }

    return NULL;
}

Message *RequestMessage(int32_t size) {
    Message *mess = (Message *) malloc (sizeof(*mess));
    if (mess) {
        if (size > 0) {
            mess->data = (void *) malloc (size);
            if (mess->data) {
                mess->length = 0;
                mess->size   = size;
                return mess;
            }
            else {
                free(mess);
            }
        }
        else {
            /*应该可以允许为0*/
            mess->size   = 0;
            mess->length = 0;
            mess->data   = NULL;
            return mess;
        }
    }

    return NULL;
}

int32_t ReleaseMessage(Message *mess) {
    if (mess) {
        if (mess->size > 0) {
            if (mess->attr == DataAttr_SpiToMqtt) 
                rel++;
            free(mess->data);
        }
        free(mess);
    }

    /* printf ("req:%ld rel:%ld\n", req, rel); */

    return 0;
}
