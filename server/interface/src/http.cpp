#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <curl/curl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<cJSON.h>
#include <http.h>

static void *gPriv = NULL;
static HttpPrint gPrint;
static LogHttp gLevel = LogHttp_Info;

/*************************************************/ 
/**************LogPrintf**************************/
/*************************************************/ 

static int HttpLogPrintf(LogHttp level,
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
        HttpLogPrintf(level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

#ifndef DEBUG0
#define DEBUG0(...)     
#endif
#ifndef DEBUG1
#define DEBUG1(...)     LogPrintf(LogHttp_Error, __VA_ARGS__);
#endif
#ifndef DEBUG2
#define DEBUG2(...)     LogPrintf(LogHttp_Debug, __VA_ARGS__);
#endif

#ifndef ERRP
#define ERRP(con, ret, flag, ...)               \
    if (con) {                              \
        DEBUG##flag(__VA_ARGS__)            \
        ret;                                \
    }
#endif

typedef struct {
    CURL* curl;
    int32_t size;
    char *response;
} HttpObj;

static int32_t writer(char* data, size_t size, size_t nmemb, char* writer_data) {
	unsigned long sizes = size * nmemb;
    void *real          = NULL;
    HttpObj *pObj       = (HttpObj *)writer_data;

    if (pObj) {
        if (NULL == pObj->response) {
            pObj->response  = (char *) malloc (sizes + 1);
            ERRP(NULL == pObj->response, return 0, 1, 
                    "malloc response sizes:%d failure\n", sizes + 1);
            pObj->size      = sizes + 1;
        }
        if (pObj->size < (int32_t)sizes + 1) {
            real = (char *) realloc (pObj->response, sizes + 1);
            ERRP(NULL == real, return 0, 1, 
                    "realloc response sizes:%d failure\n", sizes + 1);
            pObj->response  = (char *)real;
            pObj->size      = sizes + 1;
            memcpy(pObj->response, data, sizes);
            pObj->response[sizes] = '\0';
        }
        else {
            memcpy(pObj->response, data, sizes);
            pObj->response[sizes] = '\0';
        }
        LogPrintf (LogHttp_Trace, "<< %s >>\n", pObj->response);
        return sizes;
    }

    return 0;
}

//设置日志输出对象，主要看你想输出到哪里，就对应填充回调
int32_t HttpInitLog(void *priv, HttpPrint print) {
    gPriv   = priv;
    gPrint  = print;

    return 0;
}

//设置日志输出等级，看需要，调试、错误、宕机等
int32_t HttpSetLogLevel(LogHttp level) {
    gLevel  = level;
    return 0;
}

void *HttpInit(void) {
    HttpObj *pObj   = (HttpObj *) malloc (sizeof(*pObj));
    ERRP(NULL == pObj, return NULL, 1, "malloc HttpObj failure\n");

    memset(pObj, 0x0, sizeof(*pObj));

    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    return pObj;
}

//现在不考虑多个结点
int32_t HttpCreate(void *oObj) {
    HttpObj *pObj = (HttpObj *)oObj;

    pObj->curl = curl_easy_init();
    ERRP(NULL == pObj->curl, return -1, 1, "curl_easy_init failure\n");

    /* curl_easy_setopt(pObj->curl, CURLOPT_VERBOSE, 1L); */

    curl_easy_setopt(pObj->curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(pObj->curl, CURLOPT_WRITEFUNCTION, writer);
    curl_easy_setopt(pObj->curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(pObj->curl, CURLOPT_TCP_KEEPIDLE, 300L);
    curl_easy_setopt(pObj->curl, CURLOPT_TCP_KEEPINTVL, 20L);
    curl_easy_setopt(pObj->curl, CURLOPT_TIMEOUT, 30);
    /**
     * 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
     * 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
     */
    curl_easy_setopt(pObj->curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(pObj->curl, CURLOPT_CONNECTTIMEOUT, 20);

    return 0;
}

char *HttpPost(void *oObj, const char *url, 
        const char **heads, int32_t size, 
        const char *body) {
    int32_t index               = 0;
    struct curl_slist *headers  = NULL;
    HttpObj *pObj               = (HttpObj *)oObj;

    headers = curl_slist_append(headers, "Content-type:application/json");
    for(index = 0; index < size; index++) {
        headers = curl_slist_append(headers, heads[index]);
    }

    curl_easy_setopt(pObj->curl, CURLOPT_URL, url);
    curl_easy_setopt(pObj->curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(pObj->curl, CURLOPT_POST, 1);
    curl_easy_setopt(pObj->curl, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(pObj->curl, CURLOPT_WRITEDATA, (void *)pObj);
    curl_easy_perform(pObj->curl);

    return pObj->response;
}

void HttpDestory(void *oObj) {
    HttpObj *pObj = (HttpObj *)oObj;

    if (pObj) {
        curl_easy_cleanup(pObj->curl);
        free(pObj);
    }
}

/* typedef struct { */
    /* cJSON *root; */
    /* cJSON *array; */
/* } JSONObj; */

void *HttpCreateArray(void) {
    /* JSONObj *pObj = NULL; */

    /* pObj = (JSONObj *) malloc (sizeof(*pObj)); */
    /* ERRP(NULL == pObj, return NULL, 1, "malloc sizeof(JSONObj) failure\n"); */

    void *array     = cJSON_CreateArray();
    ERRP(NULL == array, goto ERR0, 1, "create array failure\n");

    /* pObj->root      = cJSON_CreateObject(); */
    /* ERRP(NULL == pObj->root, goto ERR1, 1, "create oObj failure\n"); */

    /* cJSON_AddItemToArray(pObj->array, pObj->root); */

    return array;
/* ERR1: */
    /* cJSON_Delete(pObj->array); */
ERR0:
    /* free(pObj); */
    return NULL;
}

void *HttpCreateJson(void) {
    /* JSONObj *pObj   = (JSONObj *)oObj; */
    cJSON *root     = NULL;

    root      = cJSON_CreateObject();
    ERRP(NULL == root, goto ERR1, 1, "create oObj failure\n");

    /* cJSON_AddItemToArray(pObj->array, root); */

    return root;
ERR1:
    /* free(pObj); */
    return NULL;
}

int32_t HttpAddJsonToArray(void *array, void *json) {
    /* JSONObj *pObj   = (JSONObj *)oObj; */
    /* cJSON *root     = NULL; */

    /* root      = cJSON_CreateObject(); */
    /* ERRP(NULL == root, goto ERR1, 1, "create oObj failure\n"); */

    return cJSON_AddItemToArray((cJSON *)array, (cJSON *)json);

    /* return root; */
/* ERR1: */
    /* free(pObj); */
    /* return NULL; */
}

int32_t HttpAddArrayToStringsJson(void *root, const char *strings, void *array) {
    return cJSON_AddItemToObject((cJSON *)root, strings, (cJSON *)array);
}

void *HttpCreateDoubleArray(const double *data, int32_t count) {
    return cJSON_CreateDoubleArray(data, count);
}

/*
 * void *HttpCreateSubArray(void *oObj) {
 *     cJSON *root     = NULL;
 * 
 *     root      = cJSON_CreateArray();
 *     ERRP(NULL == root, goto ERR1, 1, "create Array failure\n");
 * 
 *     return root;
 * ERR1:
 *     return NULL;
 * }
 */

void HttpDeleteArray(void *oObj) {
    /* JSONObj *pObj = (JSONObj *)oObj; */

    /*
     * if (pObj) {
     *     if (pObj->array) cJSON_Delete(pObj->array);
     *     free(pObj);
     * }
     */
    if (oObj) {
        cJSON_Delete((cJSON *)oObj);
    }
}

void *HttpAddStringJson(void *oObj, const char *key, const char *value) {
    cJSON  *pObj = (cJSON  *)oObj;
    return cJSON_AddStringToObject(pObj, key, value);  // 添加键值对
}

void *HttpAddNumberJson(void *oObj, const char *key, double number) {
    cJSON *pObj = (cJSON *)oObj;
    return cJSON_AddNumberToObject(pObj, key, number);  // 添加键值对
}

char *HttpGetArray(void *oObj) {
    /* JSONObj *pObj = (JSONObj *)oObj; */

    return cJSON_Print((cJSON *)oObj);
}

void *HttpParseArray(const char *value) {
    /* JSONObj *pObj = NULL; */

    /* pObj = (JSONObj *) malloc (sizeof(*pObj)); */
    /* ERRP(NULL == pObj, return NULL, 1, "malloc sizeof(JSONObj) failure\n"); */

    void *array =  cJSON_Parse(value);
    ERRP(NULL == array, return NULL, 1, "cJSON_Parse failure\n");

    return array;
}

int32_t HttpParseGetArraySize(void *oObj) {
    /* JSONObj *pObj = (JSONObj *)oObj; */

    if (oObj) {
        return cJSON_GetArraySize((cJSON *)oObj);
    }

    return 0;
}

const char *HttpParseGetArrayItemString(void *oObj, int32_t index, const char *type) {
    cJSON *root     = NULL;
    /* JSONObj *pObj   = (JSONObj *)oObj; */

    if (oObj) {
        root = cJSON_GetArrayItem((cJSON *)oObj, index);
        if (root) {
            cJSON *elem = cJSON_GetObjectItem(root, type);
            if (elem) {
                if (elem->type == cJSON_String)
                    return elem->valuestring;
                else 
                    return NULL;
            }
        }
    }

    return NULL;
}

const double HttpParseGetArrayItemNumber(void *oObj, int32_t index, const char *type) {
    cJSON *root     = NULL;
    /* JSONObj *pObj   = (JSONObj *)oObj; */

    if (oObj) {
        root = cJSON_GetArrayItem((cJSON *)oObj, index);
        if (root) {
            cJSON *elem = cJSON_GetObjectItem(root, type);
            if (elem) {
                if (elem->type == cJSON_Number)
                    return elem->valuedouble;
                else 
                    return 0.0;
            }
        }
    }

    return 0.0;
}

#ifdef AAA
int32_t http_print(void *oObj, const char *strings) {
    printf ("%s", strings);
    return 0;
}

int32_t main(int32_t args, char *argv[]) {
    void *json  = NULL;
    void *array = NULL;
    char *out   = NULL;
    void *http  = NULL;
    char *response = NULL;

    const char *header[] = {
        "\"Authenticate\":\"\"",
    };


    HttpInitLog(NULL, http_print);
    HttpSetLogLevel(LogHttp_Info);
    http = HttpInit();
    if (http) {
        HttpCreate(http);

/*
 *         {
 *             array = (void *)HttpCreateArray();
 *             if (array) {
 *                 json = (void *)HttpCreateJson();
 *                 if (json) {
 *                     [> char ip[128]; <]
 *                     char *out;
 *                     HttpAddStringJson(json, "htype", "heartbeat");
 *                     HttpAddNumberJson(json, "timestamp", (double)time(NULL));
 *                     HttpAddStringJson(json, "power", "DC");
 *                     HttpAddNumberJson(json, "batteryLevel", 100);
 *                     HttpAddNumberJson(json, "temperature", 50);
 *                     HttpAddStringJson(json, "gatewayVer", "1.0.0");//这个版本我建议是git版本,不过目前不太好获取,后续考虑
 *                     ///重重重，需要调整
 *                     HttpAddStringJson(json, "blemVer", "5.4.0");
 *                     HttpAddJsonToArray(array, json);
 *                     out = HttpGetArray(array);
 *                     if (out) {
 *                         printf ("out:%s\n", out);
 *                     }
 *                     HttpDeleteArray(array);
 *                 }
 *             }
 * 
 *             response = HttpPost(http, "43.142.162.137:8080", header, 1, out);
 *             if (response) {
 *                 printf ("response:%s\n", response);
 *             }
 *         }
 * 
 *         free(out);
 */


        array = (void *)HttpCreateArray();
        if (array) {
            json = HttpCreateJson();
            if (json) {
                HttpAddStringJson(json, "htype", "gatewayConn");
                HttpAddStringJson(json, "ip", "192.168.0.138");
                HttpAddStringJson(json, "bleminfo", "f9ead127c9c0,name,location");
                HttpAddStringJson(json, "username", "admin");
                HttpAddStringJson(json, "password", "11111111");
                HttpAddJsonToArray(array, json);

                out = HttpGetArray(array);
                printf ("out:%s\n", out);
                HttpDeleteArray(array);
            }
        }

        response = HttpPost(http, "43.142.162.137:8080", header, 1, out);
        if (response) {
            printf ("response:%s\n", response);
        }

        free(out);

        HttpDestory(http);
    }

    return 0;
}
#endif
