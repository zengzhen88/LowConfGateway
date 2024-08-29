#if 0
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
#include <signal.h>
#include <sys/time.h>
#include <cgic.h>
#include <jgdmsgtrans.h>
#include <iostream>
#include <time.h>

using namespace std;

int32_t sPrintf(void *priv, const char *strings) {
    fprintf (cgiOut, "%s\n", strings);
    return 0;
}

int32_t cgiBTNLoginForm(const char *strings) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    fprintf (cgiOut, "<form>");
    fprintf (cgiOut, "用户名: <input type=\"text\" name=\"username\"/>");
    fprintf (cgiOut, "密码  : <input type=\"password\" name=\"password\"/>");
    fprintf (cgiOut, "<input type=\"submit\" name=\"btnLogin\" value=\"登录\"/>");
    fprintf (cgiOut, "</form>");
    fprintf (cgiOut, "<br>");
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s </p>", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiUpgradOutForm(const char *strings, int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (!isSok) {
        fprintf (cgiOut, "<form>");
        fprintf (cgiOut, "url  :<input type=\"text\" name=\"url\"/>");
        fprintf (cgiOut, "md5 :<input type=\"text\" name=\"md5\"/>");
        fprintf (cgiOut, "flag:<input type=\"text\" name=\"flag\"/>");
        fprintf (cgiOut, "<input type=\"submit\" name=\"upgradation\" value=\"升级程序\"/>");
        fprintf (cgiOut, "</form>");
        fprintf (cgiOut, "<br>");
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiHttpurlOutForm(const char *strings, int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (!isSok) {
        fprintf (cgiOut, "<form>");
        fprintf (cgiOut, "ip   :<input type=\"text\" name=\"ip\"/>");
        fprintf (cgiOut, "port :<input type=\"text\" name=\"port\"/>");
        fprintf (cgiOut, "<input type=\"submit\" name=\"httpurl\" value=\"设置Web服务器\"/>");
        fprintf (cgiOut, "</form>");
        fprintf (cgiOut, "<br>");
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiSyncTimeOutForm(const char *strings, int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (!isSok) {
        fprintf (cgiOut, "<form>");
        fprintf (cgiOut, "Year:<input type=\"text\" name=\"Year\"/>");
        fprintf (cgiOut, "Month:<input type=\"text\" name=\"Month\"/>");
        fprintf (cgiOut, "Day:<input type=\"text\" name=\"Day\"/>");
        fprintf (cgiOut, "Hour:<input type=\"text\" name=\"Hour\"/>");
        fprintf (cgiOut, "Minute:<input type=\"text\" name=\"Minute\"/>");
        fprintf (cgiOut, "Second:<input type=\"text\" name=\"Second\"/>");
        fprintf (cgiOut, "<input type=\"submit\" name=\"synctime\" value=\"同步系统时间\"/>");
        fprintf (cgiOut, "</form>");
        fprintf (cgiOut, "<br>");
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiGetBlemInfoOutForm(const char *strings, const char *stringss, int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (NULL != stringss) {
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s{%s}</p>", isSok ? "成功" : "失败", strings, stringss);
    }
    else {
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    }
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiSetBlemInfoOutForm(const char *strings, int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");

    if (!isSok) {
        fprintf (cgiOut, "<form>");
        fprintf (cgiOut, "value:<input type=\"text\" name=\"value\"/>");
        fprintf (cgiOut, "<input type=\"submit\" name=\"setbleminfo\" value=\"  修改蓝牙模块信息\"/>");
        fprintf (cgiOut, "</form>");
        fprintf (cgiOut, "<br>");
    }

    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiGetScaninrOutForm(const char *strings, const int32_t scaninr, int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (-1 != scaninr) {
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s{%d}</p>", isSok ? "成功" : "失败", strings, scaninr);
    }
    else {
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    }
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiSetScaninrOutForm(const char *strings, int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (!isSok) {
        fprintf (cgiOut, "<form>");
        fprintf (cgiOut, "扫描时间:<input type=\"text\" name=\"scaninrtime\"/>");
        fprintf (cgiOut, "<input type=\"submit\" name=\"setscaninr\" value=\"修改蓝牙扫描窗口时间\"/>");
        fprintf (cgiOut, "</form>");
        fprintf (cgiOut, "<br>");
    }

    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiGetWifiInfoOutForm(const char *strings, 
        const char *ssid,
        const char *psk,
        const char *ip, 
        const char *netmask,
        const char *gateway,
        int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (isSok) {
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> ssid    :%s</p>", ssid);
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> psk     :%s</p>", psk);
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> ip      :%s</p>", ip);
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> netmask :%s</p>", netmask);
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> gateway :%s</p>", gateway);
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiSetWifiInfoOutForm(const char *strings, 
        int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (!isSok) {
        fprintf (cgiOut, "<form>");
        fprintf (cgiOut, "ssid:<input type=\"text\" name=\"ssid\"/>");
        fprintf (cgiOut, "psk:<input type=\"text\" name=\"psk\"/>");
        fprintf (cgiOut, "ip:<input type=\"text\" name=\"ip\"/>");
        fprintf (cgiOut, "netmask:<input type=\"text\" name=\"netmask\"/>");
        fprintf (cgiOut, "gateway:<input type=\"text\" name=\"gateway\"/>");
        fprintf (cgiOut, "<input type=\"submit\" name=\"setwifi\" value=\" 修改无线网络信息 \"/>");
        fprintf (cgiOut, "</form>");
        fprintf (cgiOut, "<br>");
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiGetEthInfoOutForm(const char *strings, 
        const char *ip, 
        const char *netmask,
        const char *gateway,
        int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (isSok) {
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> ip      :%s</p>", ip);
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> netmask :%s</p>", netmask);
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> gateway :%s</p>", gateway);
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiSetEthInfoOutForm(const char *strings, 
        int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (!isSok) {
        fprintf (cgiOut, "<form>");
        fprintf (cgiOut, "ip:<input type=\"text\" name=\"ip\"/>");
        fprintf (cgiOut, "netmask:<input type=\"text\" name=\"netmask\"/>");
        fprintf (cgiOut, "gateway:<input type=\"text\" name=\"gateway\"/>");
        fprintf (cgiOut, "<input type=\"submit\" name=\"seteth\" value=\" 修改以太网络信息 \"/>");
        fprintf (cgiOut, "</form>");
        fprintf (cgiOut, "<br>");
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiGetAuthInfoOutForm(const char *strings, 
        const char *username,
        const char *password,
        int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (isSok) {
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> username :%s</p>", username);
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> password :%s</p>", password);
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiSetAuthOutForm(const char *strings, 
        int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (!isSok) {
        fprintf (cgiOut, "<form>");
        fprintf (cgiOut, "username:<input type=\"text\" name=\"username\"/>");
        fprintf (cgiOut, "password:<input type=\"text\" name=\"password\"/>");
        fprintf (cgiOut, "<input type=\"submit\" name=\"setauth\" value=\"  修改用户名信息\"/>");
        fprintf (cgiOut, "</form>");
        fprintf (cgiOut, "<br>");
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiGetHttpServerInfoOutForm(const char *strings, 
        const char *ip,
        const int32_t port,
        int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (isSok) {
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> username :%s</p>", ip);
        fprintf (cgiOut, "<p style=\"color:#FF0000;\"> password :%d</p>", port);
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiConfigBlemReportInfoOutForm(const char *strings, int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (!isSok) {
        fprintf (cgiOut, "<form>");
        fprintf (cgiOut, "deviceId:<input type=\"text\" name=\"deviceId\"/>");
        fprintf (cgiOut, "deviceMac:<input type=\"text\" name=\"deviceMac\"/>");
        fprintf (cgiOut, "timeout:<input type=\"text\" name=\"timeout\"/>");
        fprintf (cgiOut, "priority:<input type=\"text\" name=\"priority\"/>");
        fprintf (cgiOut, "message:<input type=\"text\" name=\"message\"/>");
        fprintf (cgiOut, "<input type=\"submit\" name=\"toterminal\" value=\"配置蓝牙数据上报信息\"/>");
        fprintf (cgiOut, "</form>");
        fprintf (cgiOut, "<br>");
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiCleanBlemReportInfoOutForm(const char *strings, int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (!isSok) {
        fprintf (cgiOut, "<form>");
        fprintf (cgiOut, "priority:<input type=\"text\" name=\"priority\"/>");
        fprintf (cgiOut, "<input type=\"submit\" name=\"cleartoterminal\" value=\"清除蓝牙数据上报信息\"/>");
        fprintf (cgiOut, "</form>");
        fprintf (cgiOut, "<br>");
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiSetUploadOutForm(const char *strings, int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (!isSok) {
        fprintf (cgiOut, "<form>");
        fprintf (cgiOut, "deviceID:<input type=\"text\" name=\"deviceID\"/>");
        fprintf (cgiOut, "deviceMac:<input type=\"text\" name=\"deviceMac\"/>");
        fprintf (cgiOut, "time:<input type=\"text\" name=\"time\"/>");
        fprintf (cgiOut, "type:<input type=\"text\" name=\"type\"/>");
        fprintf (cgiOut, "valueRange:<input type=\"text\" name=\"valueRange\"/>");
        fprintf (cgiOut, "<input type=\"submit\" name=\"setupload\" value=\"设置终端指定类型上报\"/>");
        fprintf (cgiOut, "</form>");
        fprintf (cgiOut, "<br>");
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiSetUploadDefaultOutForm(const char *strings, int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (!isSok) {
        fprintf (cgiOut, "<form>");
        fprintf (cgiOut, "deviceID:<input type=\"text\" name=\"deviceID\"/>");
        fprintf (cgiOut, "<input type=\"submit\" name=\"setuploaddefault\" value=\"设置终端默认上报\"/>");
        fprintf (cgiOut, "</form>");
        fprintf (cgiOut, "<br>");
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiReportedDataOutForm(const char *strings, int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (!isSok) {
        fprintf (cgiOut, "<form>");
        fprintf (cgiOut, "value:<input type=\"text\" name=\"value\"/>");
        fprintf (cgiOut, "<input type=\"submit\" name=\"reporteddata\" value=\"定时上报终端数据使能\"/>");
        fprintf (cgiOut, "</form>");
        fprintf (cgiOut, "<br>");
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t cgiotmdataIntervalOutForm(const char *strings, int32_t isSok) {
    cgiHeaderContentType((char *)"text/html; charset=utf-8");
    fprintf (cgiOut, "<html><head></head><body>");
    if (!isSok) {
        fprintf (cgiOut, "<form>");
        fprintf (cgiOut, "interval:<input type=\"text\" name=\"interval\"/>");
        fprintf (cgiOut, "<input type=\"submit\" name=\"otmdataintval\" value=\"设置第三方终端上报间隔\"/>");
        fprintf (cgiOut, "</form>");
        fprintf (cgiOut, "<br>");
    }
    fprintf (cgiOut, "<p style=\"color:#FF0000;\"> %s:%s</p>", isSok ? "成功" : "失败", strings);
    fprintf (cgiOut, "</body></html>");

    return 0;
}

int32_t upgrad(const char *url, 
        const char *md5, 
        const char *flag) {
    int32_t status   = 0;
    JGD215_UpgradCmd cmd;
    JGD215_UpgradAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 123;
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "applicationUpdate");
    snprintf ((char *)cmd.url, sizeof(cmd.url) - 1, "%s", url);
    snprintf ((char *)cmd.md5, sizeof(cmd.md5) - 1, "%s", md5);
    snprintf ((char *)cmd.flag, sizeof(cmd.flag) - 1, "%s", flag);
    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (!strcmp((const char *)ack.message, "OK")
                && ack.aseq == 123) {
            cgiUpgradOutForm("升级程序", 1);
        } 
        else {
            cgiUpgradOutForm("升级程序", 0);
        }
    }
    else {
        cgiUpgradOutForm("升级程序", 0);
    }

    return 0;
}

int32_t httpurl(const char *ip, 
        const char *port) {
    int32_t status   = 0;
    JGD215_HttpUrlCmd cmd;
    JGD215_HttpUrlAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 124;
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "setHttpServer");
    snprintf ((char *)cmd.ip, sizeof(cmd.ip) - 1, "%s", ip);
    snprintf ((char *)cmd.port, sizeof(cmd.port) - 1, "%s", port);
    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 124) {
            cgiHttpurlOutForm("设置WEB服务器", 1);
        }
        else {
            cgiHttpurlOutForm("设置WEB服务器", 0);
        }
    }
    else {
        cgiHttpurlOutForm("设置WEB服务器", 0);
    }

    return 0;
}

int32_t synctime(const char *Year, 
        const char *Month, 
        const char *Day, 
        const char *Hour, 
        const char *Minute, 
        const char *Second) {
    int32_t status   = 0;
    JGD215_SyncTimeCmd cmd;
    JGD215_SyncTimeAck ack(0);
    JGD215MsgClient client;
    struct tm times;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 125;
    times.tm_year   = atoi(Year) - 1900;
    times.tm_mon    = atoi(Month) - 1;
    times.tm_mday   = atoi(Day);
    times.tm_hour   = atoi(Hour);
    times.tm_min    = atoi(Minute);
    times.tm_sec    = atoi(Second);
    cmd.times = (uint64_t)mktime(&times);
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "synTime");
    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 125) {
            cgiSyncTimeOutForm("同步系统时间", 1);
        }
        else {
            cgiSyncTimeOutForm("同步系统时间", 0);
        }
    }
    else {
        cgiSyncTimeOutForm("同步系统时间", 0);
    }

    return 0;
}

int32_t getblueinfo(void) {
    int32_t status   = 0;
    JGD215_GetBlemInfoCmd cmd;
    JGD215_GetBlemInfoAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 126;
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "getBlemInfo");
    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 126) {
            cgiGetBlemInfoOutForm("获取蓝牙信息", (const char *)ack.value, 1);
        }
        else {
            cgiGetBlemInfoOutForm("获取蓝牙信息", NULL, 0);
        }
    }
    else {
        cgiGetBlemInfoOutForm("获取蓝牙信息", NULL, 0);
    }

    return 0;
}

int32_t setblueinfo(const char *info) {
    int32_t status   = 0;
    JGD215_SetBlemInfoCmd cmd;
    JGD215_SetBlemInfoAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 126;
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "getBlemInfo");
    snprintf ((char *)cmd.value, sizeof(cmd.value) - 1, "%s", "setBlemInfo");
    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 126) {
            cgiSetBlemInfoOutForm("设置蓝牙信息", 1);
        }
        else {
            cgiSetBlemInfoOutForm("设置蓝牙信息", 0);
        }
    }
    else {
        cgiSetBlemInfoOutForm("设置蓝牙信息", 0);
    }

    return 0;
}

int32_t getscaninr(void) {
    int32_t status   = 0;
    JGD215_GetScaninrCmd cmd;
    JGD215_GetScaninrAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 127;
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "getScaninr");
    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 127) {
            cgiGetScaninrOutForm("获取蓝牙扫描窗口时间", ack.value, 1);
        }
        else {
            cgiGetScaninrOutForm("获取蓝牙扫描窗口时间", -1, 0);
        }
    }
    else {
        cgiGetScaninrOutForm("获取蓝牙扫描窗口时间", -1, 0);
    }

    return 0;
}

int32_t setscaninr(const char *scaninr) {
    int32_t status   = 0;
    JGD215_SetScaninrCmd cmd;
    JGD215_SetScaninrAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 128;
    cmd.value = atoi(scaninr);
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "setScaninr");
    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 128) {
            cgiSetScaninrOutForm("修改蓝牙扫描窗口时间", 1);
        }
        else {
            cgiSetScaninrOutForm("修改蓝牙扫描窗口时间", 0);
        }
    }
    else {
        cgiSetScaninrOutForm("修改蓝牙扫描窗口时间", 0);
    }

    return 0;
}

int32_t getwifiinfo(void) {
    int32_t status   = 0;
    JGD215_GetWifiInfoCmd cmd;
    JGD215_GetWifiInfoAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 129;
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "getWiFi");
    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 129) {
            cgiGetWifiInfoOutForm(
                    "获取无线网络信息", 
                    (char *)ack.ssid,
                    (char *)ack.psk,
                    (char *)ack.ip,
                    (char *)ack.netmask,
                    (char *)ack.gateway,
                    1);
        }
        else {
            cgiGetWifiInfoOutForm(
                    "获取无线网络信息", 
                    (char *)ack.ssid,
                    (char *)ack.psk,
                    (char *)ack.ip,
                    (char *)ack.netmask,
                    (char *)ack.gateway,
                    0);
        }
    }
    else {
            cgiGetWifiInfoOutForm(
                    "获取无线网络信息", 
                    (char *)ack.ssid,
                    (char *)ack.psk,
                    (char *)ack.ip,
                    (char *)ack.netmask,
                    (char *)ack.gateway,
                    0);
    }

    return 0;
}

int32_t setwifiinfo(
        const char *ssid, 
        const char *psk, 
        const char *ip, 
        const char *netmask, 
        const char *gateway) {
    int32_t status   = 0;
    JGD215_SetWifiInfoCmd cmd;
    JGD215_SetWifiInfoAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 129;
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "getWiFi");
    snprintf ((char *)cmd.ssid, sizeof(cmd.ssid) - 1, "%s", ssid);
    snprintf ((char *)cmd.psk, sizeof(cmd.psk) - 1, "%s", psk);
    snprintf ((char *)cmd.ip, sizeof(cmd.ip) - 1, "%s", ip);
    snprintf ((char *)cmd.netmask, sizeof(cmd.netmask) - 1, "%s", netmask);
    snprintf ((char *)cmd.gateway, sizeof(cmd.gateway) - 1, "%s", gateway);
    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 129) {
            cgiSetWifiInfoOutForm(
                    "修改无线网络信息", 
                    1);
        }
        else {
            cgiSetWifiInfoOutForm(
                    "修改无线网络信息", 
                    0);
        }
    }
    else {
            cgiSetWifiInfoOutForm(
                    "修改无线网络信息", 
                    0);
    }

    return 0;
}

int32_t getethinfo(void) {
    int32_t status   = 0;
    JGD215_GetEthInfoCmd cmd;
    JGD215_GetEthInfoAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 129;
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "getEth");
    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 129) {
            cgiGetEthInfoOutForm(
                    "获取以太网网络信息", 
                    (char *)ack.ip,
                    (char *)ack.netmask,
                    (char *)ack.gateway,
                    1);
        }
        else {
            cgiGetEthInfoOutForm(
                    "获取以太网网络信息", 
                    (char *)ack.ip,
                    (char *)ack.netmask,
                    (char *)ack.gateway,
                    0);
        }
    }
    else {
            cgiGetEthInfoOutForm(
                    "获取以太网网络信息", 
                    (char *)ack.ip,
                    (char *)ack.netmask,
                    (char *)ack.gateway,
                    0);
    }

    return 0;
}

int32_t setethinfo(
        const char *ip, 
        const char *netmask, 
        const char *gateway) {
    int32_t status   = 0;
    JGD215_SetEthInfoCmd cmd;
    JGD215_SetEthInfoAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 129;
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "getWiFi");
    snprintf ((char *)cmd.ip, sizeof(cmd.ip) - 1, "%s", ip);
    snprintf ((char *)cmd.netmask, sizeof(cmd.netmask) - 1, "%s", netmask);
    snprintf ((char *)cmd.gateway, sizeof(cmd.gateway) - 1, "%s", gateway);
    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 129) {
            cgiSetEthInfoOutForm(
                    "修改以太网络信息", 
                    1);
        }
        else {
            cgiSetEthInfoOutForm(
                    "修改以太网络信息", 
                    0);
        }
    }
    else {
            cgiSetEthInfoOutForm(
                    "修改以太网络信息", 
                    0);
    }

    return 0;
}

int32_t getauthinfo(void) {
    int32_t status   = 0;
    JGD215_GetAuthInfoCmd cmd;
    JGD215_GetAuthInfoAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 129;
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "getAuth");
    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 129) {
            cgiGetAuthInfoOutForm(
                    "获取用户名信息", 
                    (char *)ack.username,
                    (char *)ack.passwd,
                    1);
        }
        else {
            cgiGetAuthInfoOutForm(
                    "获取用户名信息", 
                    (char *)ack.username,
                    (char *)ack.passwd,
                    0);
        }
    }
    else {
            cgiGetAuthInfoOutForm(
                    "获取用户名信息", 
                    (char *)ack.username,
                    (char *)ack.passwd,
                    0);
    }

    return 0;
}

int32_t setauthinfo(const char *username, 
        const char *password) {
    int32_t status   = 0;
    JGD215_SetAuthInfoCmd cmd;
    JGD215_SetAuthInfoAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 129;
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "getAuth");
    snprintf ((char *)cmd.username, sizeof(cmd.username) - 1, "%s", username);
    snprintf ((char *)cmd.passwd, sizeof(cmd.passwd) - 1, "%s", password);
    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 129) {
            cgiSetAuthOutForm(
                    "修改用户名信息", 
                    1);
        }
        else {
            cgiSetAuthOutForm(
                    "修改用户名信息", 
                    0);
        }
    }
    else {
            cgiSetAuthOutForm(
                    "修改用户名信息", 
                    0);
    }

    return 0;
}

int32_t gethttpserverinfo(void) {
    int32_t status   = 0;
    JGD215_GetHttpServerInfoCmd cmd;
    JGD215_GetHttpServerInfoAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 129;
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "getAuth");
    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 129) {
            cgiGetHttpServerInfoOutForm(
                    "获取服务器信息", 
                    (char *)ack.ip,
                    ack.port,
                    1);
        }
        else {
            cgiGetHttpServerInfoOutForm(
                    "获取服务器信息", 
                    (char *)ack.ip,
                    ack.port,
                    0);
        }
    }
    else {
            cgiGetHttpServerInfoOutForm(
                    "获取服务器信息", 
                    (char *)ack.ip,
                    ack.port,
                    0);
    }

    return 0;
}

int32_t toterminal(
        const char *deviceId, 
        const char *deviceMac, 
        const char *timeout, 
        const char *priority, 
        const char *message) {
    int32_t status   = 0;
    JGD215_ConfigBlemReportInfoCmd cmd;
    JGD215_ConfigBlemReportInfoAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 129;
    cmd.deviceID    = atoi(deviceId);
    cmd.priority    = atoi(priority);
    cmd.timeout     = atoi(timeout);
    snprintf ((char *)cmd.deviceMac, sizeof(cmd.deviceMac) - 1, "%s", deviceMac);
    snprintf ((char *)cmd.message, sizeof(cmd.message) - 1, "%s", message);
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "toTerminal");

    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 129) {
            cgiConfigBlemReportInfoOutForm(
                    "配置蓝牙数据上报信息", 
                    1);
        }
        else {
            cgiConfigBlemReportInfoOutForm(
                    "配置蓝牙数据上报信息", 
                    0);
        }
    }
    else {
            cgiConfigBlemReportInfoOutForm(
                    "配置蓝牙数据上报信息", 
                    0);
    }

    return 0;
}

int32_t cleantoterminal(const char *priority) {
    int32_t status   = 0;
    JGD215_CleanBlemReportInfoCmd cmd;
    JGD215_CleanBlemReportInfoAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 129;
    cmd.priority    = atoi(priority);
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "cleanToTerminal");

    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 129) {
            cgiCleanBlemReportInfoOutForm(
                    "配置蓝牙数据上报信息", 
                    1);
        }
        else {
            cgiCleanBlemReportInfoOutForm(
                    "配置蓝牙数据上报信息", 
                    0);
        }
    }
    else {
        cgiCleanBlemReportInfoOutForm(
                "配置蓝牙数据上报信息", 
                0);
    }

    return 0;
}

int32_t setupload(
        const char *deviceID, 
        const char *deviceMac, 
        const char *time, 
        const char *type, 
        const char *valueRange) {
    int32_t status   = 0;
    JGD215_SetUploadCmd cmd;
    JGD215_SetUploadAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 129;
    cmd.deviceID    = atoi(deviceID);
    cmd.time        = atoi(time);
    cmd.type        = atoi(type);
    strcpy((char *)cmd.valueRange, valueRange);
    strcpy((char *)cmd.deviceMac, deviceMac);
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "setUpload");

    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 129) {
            cgiSetUploadOutForm(
                    "设置终端指定类型上报", 
                    1);
        }
        else {
            cgiSetUploadOutForm(
                    "设置终端指定类型上报", 
                    0);
        }
    }
    else {
        cgiSetUploadOutForm(
                "设置终端指定类型上报", 
                0);
    }

    return 0;
}

int32_t setuploaddefault(const char *deviceID) {
    int32_t status   = 0;
    JGD215_SetUploadDefaultCmd cmd;
    JGD215_SetUploadDefaultAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 129;
    cmd.deviceID    = atoi(deviceID);
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "setUploadDefault");

    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 129) {
            cgiSetUploadDefaultOutForm(
                    "设置终端默认上报", 
                    1);
        }
        else {
            cgiSetUploadDefaultOutForm(
                    "设置终端默认上报", 
                    0);
        }
    }
    else {
        cgiSetUploadDefaultOutForm(
                "设置终端默认上报", 
                0);
    }

    return 0;
}

int32_t reporteddata(const char *value) {
    int32_t status   = 0;
    JGD215_ReportedDataCmd cmd;
    JGD215_ReportedDataAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 129;
    cmd.value    = atoi(value);
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "reportedData");

    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 129) {
            cgiReportedDataOutForm(
                    "定时上报终端数据使能", 
                    1);
        }
        else {
            cgiReportedDataOutForm(
                    "定时上报终端数据使能", 
                    0);
        }
    }
    else {
        cgiReportedDataOutForm(
                "定时上报终端数据使能", 
                0);
    }

    return 0;
}

int32_t otmdatainterval(const char *interval) {
    int32_t status   = 0;
    JGD215_otMdataIntervalCmd cmd;
    JGD215_otMdataIntervalAck ack(0);
    JGD215MsgClient client;

    // client.JGD215MsgClientSetLevel(NULL, (JGD215MsgTransPrint)sPrintf);
    client.JGD215MsgClientCreate();
    client.MsgTransDealRun();

    cmd.seq = 129;
    cmd.interval    = atoi(interval);
    snprintf ((char *)cmd.htype, sizeof(cmd.htype) - 1, "%s", "reportedData");

    status = client.JGD215MsgSend(&cmd, &ack);
    if (!status) {
        if (ack.aseq == 129) {
            cgiotmdataIntervalOutForm(
                    "设置第三方终端上报间隔", 
                    1);
        }
        else {
            cgiotmdataIntervalOutForm(
                    "设置第三方终端上报间隔", 
                    0);
        }
    }
    else {
        cgiotmdataIntervalOutForm(
                "设置第三方终端上报间隔", 
                0);
    }

    return 0;
}

static int32_t JGD215WebServerLogIn(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    //登录
    status = cgiFormString((char *)"btnLogin", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        char username[256]  = {0};
        char password[256]  = {0};
        status = cgiFormString((char *)"username", username, sizeof(username) - 1);
        if (cgiFormSuccess != status) {
            cgiBTNLoginForm("用户名必须填写");
            return -1;
        }

        status = cgiFormString((char *)"password", password, sizeof(password) - 1);
        if (cgiFormSuccess != status) {
            cgiBTNLoginForm("密码必须填写");
            return -1;
        }

        if (strcmp(username, "admin")) {
            cgiBTNLoginForm("用户名/密码填写错误");
            return -1;
        }

        if (strcmp(password, "123456")) {
            cgiBTNLoginForm("用户名/密码填写错误");
            return -1;
        }

        cgiHeaderLocation((char *)"../main.html");

        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerUpgradtion(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    status = cgiFormString((char *)"upgradation", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        char url[127] = {0};
        char md5[127] = {0};
        char flag[127] = {0};

        status = cgiFormString((char *)"url", url, sizeof(url) - 1);
        if (cgiFormSuccess != status) {
            cgiUpgradOutForm("URL必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"md5", md5, sizeof(md5) - 1);
        if (cgiFormSuccess != status) {
            cgiUpgradOutForm("MD5必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"flag", flag, sizeof(flag) - 1);
        if (cgiFormSuccess != status) {
            cgiUpgradOutForm("FLAG必须填写", 0);
            return -1;
        }

        upgrad(url, md5, flag);

        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerConfigHttpUrl(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    status = cgiFormString((char *)"httpurl", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        char ip[127] = {0};
        char port[127] = {0};

        status = cgiFormString((char *)"ip", ip, sizeof(ip) - 1);
        if (cgiFormSuccess != status) {
            cgiHttpurlOutForm("IP必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"port", port, sizeof(port) - 1);
        if (cgiFormSuccess != status) {
            cgiHttpurlOutForm("PORT必须填写", 0);
            return -1;
        }

        httpurl(ip, port);

        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerSyncTime(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    //2023-09-09 11:40:05
    status = cgiFormString((char *)"synctime", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        char Year[127];
        char Month[127];
        char Day[127];
        char Hour[127];
        char Minute[127];
        char Second[127];

        status = cgiFormString((char *)"Year", Year, sizeof(Year) - 1);
        if (cgiFormSuccess != status) {
            cgiSyncTimeOutForm("Year必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"Month", Month, sizeof(Month) - 1);
        if (cgiFormSuccess != status) {
            cgiSyncTimeOutForm("Month必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"Day", Day, sizeof(Day) - 1);
        if (cgiFormSuccess != status) {
            cgiSyncTimeOutForm("Day必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"Hour", Hour, sizeof(Hour) - 1);
        if (cgiFormSuccess != status) {
            cgiSyncTimeOutForm("Hour必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"Minute", Minute, sizeof(Minute) - 1);
        if (cgiFormSuccess != status) {
            cgiSyncTimeOutForm("Minute必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"Second", Second, sizeof(Second) - 1);
        if (cgiFormSuccess != status) {
            cgiSyncTimeOutForm("Second必须填写", 0);
            return -1;
        }

        synctime(Year,
                Month, 
                Day, 
                Hour, 
                Minute, 
                Second);


        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerGetBlueInfo(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    //2023-09-09 11:40:05
    status = cgiFormString((char *)"getbleminfo", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        getblueinfo();
        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerSetBlueInfo(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    //2023-09-09 11:40:05
    status = cgiFormString((char *)"setbleminfo", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        char info[127];

        status = cgiFormString((char *)"info", info, sizeof(info) - 1);
        if (cgiFormSuccess != status) {
            cgiSetBlemInfoOutForm("info必须填写", 0);
            return -1;
        }

        setblueinfo(info);
        return 0;
    }

    return 0;
}


static int32_t JGD215WebServerGetScaninr(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    //2023-09-09 11:40:05
    status = cgiFormString((char *)"getscaninr", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        getscaninr();
        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerSetScaninr(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    //2023-09-09 11:40:05
    status = cgiFormString((char *)"setscaninr", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        char scaninr[127];

        status = cgiFormString((char *)"scaninrtime", scaninr, sizeof(scaninr) - 1);
        if (cgiFormSuccess != status) {
            cgiSetScaninrOutForm("scaninrtime必须填写", 0);
            return -1;
        }

        setscaninr(scaninr);
        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerGetWifiInfo(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    status = cgiFormString((char *)"getwifiinfo", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        getwifiinfo();
        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerSetWifiInfo(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    status = cgiFormString((char *)"setwifiinfo", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        char ssid[127];
        char psk[127];
        char ip[127];
        char netmask[127];
        char gateway[127];

        status = cgiFormString((char *)"ssid", ssid, sizeof(ssid) - 1);
        if (cgiFormSuccess != status) {
            cgiSetWifiInfoOutForm("ssid必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"psk", psk, sizeof(psk) - 1);
        if (cgiFormSuccess != status) {
            cgiSetWifiInfoOutForm("psk必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"ip", ip, sizeof(ip) - 1);
        if (cgiFormSuccess != status) {
            cgiSetWifiInfoOutForm("ip必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"netmask", netmask, sizeof(netmask) - 1);
        if (cgiFormSuccess != status) {
            cgiSetWifiInfoOutForm("netmask必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"gateway", gateway, sizeof(gateway) - 1);
        if (cgiFormSuccess != status) {
            cgiSetWifiInfoOutForm("gateway必须填写", 0);
            return -1;
        }

        setwifiinfo(ssid, psk, ip, netmask, gateway);
        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerGetEthInfo(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    status = cgiFormString((char *)"getethinfo", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        getethinfo();
        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerSetEthInfo(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    status = cgiFormString((char *)"setethinfo", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        char ip[127];
        char netmask[127];
        char gateway[127];

        status = cgiFormString((char *)"ip", ip, sizeof(ip) - 1);
        if (cgiFormSuccess != status) {
            cgiSetEthInfoOutForm("ip必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"netmask", netmask, sizeof(netmask) - 1);
        if (cgiFormSuccess != status) {
            cgiSetEthInfoOutForm("netmask必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"gateway", gateway, sizeof(gateway) - 1);
        if (cgiFormSuccess != status) {
            cgiSetEthInfoOutForm("gateway必须填写", 0);
            return -1;
        }

        setethinfo(ip, netmask, gateway);
        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerGetAuthInfo(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    status = cgiFormString((char *)"getauthinfo", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        getauthinfo();
        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerSetAuthInfo(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    status = cgiFormString((char *)"setauthinfo", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        char username[127];
        char password[127];

        status = cgiFormString((char *)"username", username, sizeof(username) - 1);
        if (cgiFormSuccess != status) {
            cgiSetAuthOutForm("username必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"password", password, sizeof(password) - 1);
        if (cgiFormSuccess != status) {
            cgiSetAuthOutForm("password必须填写", 0);
            return -1;
        }

        setauthinfo(username, password);
        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerConfigBlemReportInfo(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    status = cgiFormString((char *)"toterminal", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        char deviceId[127];
        char deviceMac[127];
        char timeout[127];
        char priority[127];
        char message[127];

        status = cgiFormString((char *)"deviceId", deviceId, sizeof(deviceId) - 1);
        if (cgiFormSuccess != status) {
            cgiConfigBlemReportInfoOutForm("deviceId必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"deviceMac", deviceMac, sizeof(deviceMac) - 1);
        if (cgiFormSuccess != status) {
            cgiConfigBlemReportInfoOutForm("deviceMac必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"timeout", timeout, sizeof(timeout) - 1);
        if (cgiFormSuccess != status) {
            cgiConfigBlemReportInfoOutForm("timeout必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"priority", priority, sizeof(priority) - 1);
        if (cgiFormSuccess != status) {
            cgiConfigBlemReportInfoOutForm("priority必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"message", message, sizeof(message) - 1);
        if (cgiFormSuccess != status) {
            cgiConfigBlemReportInfoOutForm("message必须填写", 0);
            return -1;
        }

        toterminal(deviceId, 
                deviceMac, 
                timeout, 
                priority, 
                message);
        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerCleanBlemReportInfo(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    status = cgiFormString((char *)"cleantoterminal", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        char priority[127];

        status = cgiFormString((char *)"priority", priority, sizeof(priority) - 1);
        if (cgiFormSuccess != status) {
            cgiCleanBlemReportInfoOutForm("priority必须填写", 0);
            return -1;
        }

        cleantoterminal(priority);
        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerSetUpload(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    status = cgiFormString((char *)"setupload", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        char deviceID[127];
        char deviceMac[127];
        char time[127];
        char type[127];
        char valueRange[127];

        status = cgiFormString((char *)"deviceID", deviceID, sizeof(deviceID) - 1);
        if (cgiFormSuccess != status) {
            cgiSetUploadOutForm("deviceID必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"deviceMac", deviceMac, sizeof(deviceMac) - 1);
        if (cgiFormSuccess != status) {
            cgiSetUploadOutForm("deviceMac必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"time", time, sizeof(time) - 1);
        if (cgiFormSuccess != status) {
            cgiSetUploadOutForm("time必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"type", type, sizeof(type) - 1);
        if (cgiFormSuccess != status) {
            cgiSetUploadOutForm("type必须填写", 0);
            return -1;
        }

        status = cgiFormString((char *)"valueRange", valueRange, sizeof(valueRange) - 1);
        if (cgiFormSuccess != status) {
            cgiSetUploadOutForm("valueRange必须填写", 0);
            return -1;
        }

        setupload(deviceID, 
                deviceMac, 
                time, 
                type, 
                valueRange);
        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerSetUploadDefault(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    status = cgiFormString((char *)"setuploaddefault", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        char deviceID[127];

        status = cgiFormString((char *)"deviceID", deviceID, sizeof(deviceID) - 1);
        if (cgiFormSuccess != status) {
            cgiSetUploadDefaultOutForm("deviceID必须填写", 0);
            return -1;
        }

        setuploaddefault(deviceID);
        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerReportedData(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    status = cgiFormString((char *)"reporteddata", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        char value[127];

        status = cgiFormString((char *)"value", value, sizeof(value) - 1);
        if (cgiFormSuccess != status) {
            cgiReportedDataOutForm("value必须填写", 0);
            return -1;
        }

        reporteddata(value);
        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerotMdataInterval(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    status = cgiFormString((char *)"otmdatainterval", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        char interval[127];

        status = cgiFormString((char *)"interval", interval, sizeof(interval) - 1);
        if (cgiFormSuccess != status) {
            cgiotmdataIntervalOutForm("interval必须填写", 0);
            return -1;
        }

        otmdatainterval(interval);
        return 0;
    }

    return 0;
}

static int32_t JGD215WebServerGetHttpServerInfo(void) {
    char formats[256] = {0};
    cgiFormResultType status;

    status = cgiFormString((char *)"gethttpserverinfo", formats, sizeof(formats) - 1);
    if (cgiFormSuccess == status) {
        gethttpserverinfo();
        return 0;
    }

    return 0;
}

int32_t cgiMain() {
    {
        //登录接口
        JGD215WebServerLogIn();
    }

    {
        //升级接口
        JGD215WebServerUpgradtion();
    }

    {
        //配置服务器接口
        JGD215WebServerConfigHttpUrl();

        //获取服务器信息
        JGD215WebServerGetHttpServerInfo();
    }

    {
        //同步系统时间
        JGD215WebServerSyncTime();
    }

    {
        //获取蓝牙模块信息
        JGD215WebServerGetBlueInfo();

        //设置蓝牙模块信息
        JGD215WebServerSetBlueInfo();
    }

    {
        //获取蓝牙扫描窗口时间
        JGD215WebServerGetScaninr();

        //修改蓝牙扫描窗口时间
        JGD215WebServerSetScaninr();
    }

    {
        //获取无线网络信息
        JGD215WebServerGetWifiInfo();

        //修改无线网络信息
        JGD215WebServerSetWifiInfo();
    }

    {
        //获取以太网网络信息
        JGD215WebServerGetEthInfo();

        //修改以太网络信息
        JGD215WebServerSetEthInfo();
    }

    {
        //获取用户名信息
        JGD215WebServerGetAuthInfo();

        //修改用户名信息
        JGD215WebServerSetAuthInfo();
    }

    {
        //配置蓝牙数据上报信息
        JGD215WebServerConfigBlemReportInfo();

        //清除蓝牙数据上报信息
        JGD215WebServerCleanBlemReportInfo();
    }

    {
        //设置终端指定类型上报
        JGD215WebServerSetUpload();

        //设置终端默认上报
        JGD215WebServerSetUploadDefault();
    }

    {
        //定时上报终端数据使能
        JGD215WebServerReportedData();
    }

    {
        //设置第三方终端上报间隔
        JGD215WebServerotMdataInterval();
    }

    return 0;
}
#else 
#include <stdint.h>
#include <stdio.h>

int32_t main(int32_t args, char *argv[]) {
    return 0;
}
#endif
