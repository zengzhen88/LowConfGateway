#ifndef __HTTP_H_H___
#define __HTTP_H_H___ 


typedef enum {
    LogHttp_None = 0,  //关闭日志输出
    LogHttp_Urgent,    //必须打的
    LogHttp_Fatal,     //致使级
    LogHttp_Error,     //错误级
    LogHttp_Warning,   //告警级
    LogHttp_Info,      //业务级
    LogHttp_Debug,     //调试级
    LogHttp_Trace,     //跟踪级
    LogHttp_Detail,    //详细级
    LogHttp_Cnt
} LogHttp;

typedef int32_t (*HttpPrint)(void *, const char *);

int32_t HttpInitLog(void *priv, HttpPrint print);
int32_t HttpSetLogLevel(LogHttp level);

void *HttpInit(void);
void HttpDestory(void *oObj);

int32_t HttpCreate(void *oObj);
char *HttpPost(void *oObj, const char *url, 
        const char **headers, int32_t size, 
        const char *body);

void *HttpParseArray(const char *strings);
void *HttpCreateJson();
void *HttpCreateArray(void);
// void *HttpCreateSubArray(void *oObj);
void *HttpCreateDoubleArray(const double *data, int32_t count);
void HttpDeleteArray(void *oObj);
void *HttpAddStringJson(void *oObj,
        const char *key, const char *value);
void *HttpAddNumberJson(void *oObj, 
        const char *key, double number);
int32_t HttpAddJsonToArray(void *array, void *json);
int32_t HttpAddArrayToStringsJson(void *root, const char *strings, void *array);
char *HttpGetArray(void *oObj);


void *HttpParseJson(const char *value);
int32_t HttpParseGetArraySize(void *oObj);
const char *HttpParseGetArrayItemString(void *oObj, int32_t index, const char *strings);
const double HttpParseGetArrayItemNumber(void *oObj, int32_t index, const char *strings);

#endif /*__HTTP_H_H___*/
