#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <message.h>


/*
 * static int32_t RequestMqttMessage(Message *mess, int32_t *size, int32_t count) {
 *     if (mess) {
 *         if (count != 2) {
 *             return -1;
 *         }
 * 
 *         mess->mqtt.topic = (char *) malloc (size[0]);
 *         if (mess->mqtt.topic) {
 *             mess->mqtt.payload = (void *) malloc (size[1]);
 *             if (mess->mqtt.payload) {
 *                 return 0;
 *             }
 *         }
 *     }
 * 
 *     return -1;
 * }
 * 
 * int32_t ReleaseMqttMessage(Message *mess) {
 *     if (mess) {
 *         if (mess->mqtt.topic) {
 *             free(mess->mqtt.topic);
 *         }
 *         if (mess->mqtt.payload) {
 *             free(mess->mqtt.payload);
 *         }
 *     }
 * 
 *     return 0;
 * }
 */

Message *RequestMessage(DataAttr attr, int32_t *size, int32_t count) {
    /* int32_t status  = -1; */
    Message *mess   = (Message *) malloc (sizeof(*mess));
    if (mess) {
        switch (attr) {
            case DataAttr_Wifi:
                {
                    break;
                }
            /*
             * case DataAttr_MqqtTest:
             *     {
             *         status = RequestMqttMessage(mess, size, count);
             *         if (!status) {
             *             return mess;
             *         }
             *         break;
             *     }
             */
            default:break;
        }
    }

    return NULL;
}

int32_t ReleaseMessage(Message *mess) {
    if (mess) {
        switch (mess->attr) {
            case DataAttr_Wifi:
                {
                    break;
                }
            /*
             * case DataAttr_MqqtTest:
             *     {
             *         ReleaseMqttMessage(mess);
             *         break;
             *     }
             */
            default:break;
        }

        free(mess);
    }

    return 0;
}
