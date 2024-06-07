#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <message.h>


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
        if (mess->size > 0) free(mess->data);
        free(mess);
    }

    return 0;
}
