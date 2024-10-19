#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QFrame>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QPixmap>
#include <QPalette>
#include <QColor>
#include "qnavigationwidget.h"
#include <QMap>
#define QTMQTT_
#ifdef QTMQTT_
#include <QtMqtt/qmqttclient.h>
#endif
#include <message.h>
#include <QFileSystemModel>
#include <QTreeView>
#include <QHeaderView>
#include <QComboBox>
#include <httpserver.h>
#include <QTimer>
//#include <QHttpServer>

enum SubWindowType {
    SubWindowType_Login,
    SubWindowType_ServerList,
    SubWindowType_MainContext,
    SubWindowType_RealTimeRefreshContext,
    SubWindowType_Context,
    SubWindowType_Cnt
};

enum SignalSync {
    SignalSync_Init,
    SignalSync_FAILURE,
    SignalSync_OK,
    SignalSync_Cnt,
};

enum TableItem {
    TableItem_0,
    TableItem_1,
    TableItem_2,
    TableItem_3,
    TableItem_4,
    TableItem_5,
    TableItem_6,
};

struct sConfigTogether {
    /*system*/
    float sTemperature;
    float sReportTemperture;
    QString sVersion;
    QString sInfo;
    QString sReportInfo;

    /*eth*/
    QString sEthAddress;
    QString sEthNetmask;
    QString sEthGateway;
    QString sReportEthAddress;
    QString sReportEthNetmask;
    QString sReportEthGateway;

    /*wifi*/
    QString sWifiSsid;
    QString sWifiPassword;
    QString sWifiAddress;
    QString sWifiNetmask;
    QString sWifiGateway;
    QString sReportWifiSsid;
    QString sReportWifiPassword;
    QString sReportWifiAddress;
    QString sReportWifiNetmask;
    QString sReportWifiGateway;

    /*mqtt*/
    QString sMqttUser;
    QString sMqttPassword;
    QString sMqttUrl;
    QString sReportMqttUser;
    QString sReportMqttPassword;
    QString sReportMqttUrl;

    int sScanTimeout;
    int sReportScanTimeout;

    QString sReportPtRecvMac;
    int sReportPtRecvSeq;
    QString sReportPtRecvState;

    QString sReportTransmitMac;
    QString sReportTransmitData;

    QString sTransmitData;

    /*power mode*/
    QString sMode;
    int sLevel;
    QString sReportMode;
    int sReportLevel;

    SignalSync signalSync;

    /*update*/
    QFileSystemModel *sFileSystemMode;
    QTreeView *sTreeView;

    QString updateFileString;

    QMqttClient *client;

    QString topic; //发布
    QString topicack;//订阅

    int seq;

    QTimer *timer;

    sConfigTogether() {
        signalSync = SignalSync_Init;
        sReportTemperture = 30.0;
        sReportInfo = "default";
        sReportEthAddress = "xx.xx.xx.xx";
        sReportEthGateway = "xx.xx.xx.xx";
        sReportEthNetmask = "xx.xx.xx.xx";
        sReportWifiSsid = "xxxxx";
        sReportWifiPassword = "xxxxxxxx";
        sReportWifiAddress = "xx.xx.xx.xx";
        sReportWifiGateway = "xx.xx.xx.xx";
        sReportWifiNetmask = "xx.xx.xx.xx";
        sReportMqttUser = "xxxxx";
        sReportMqttPassword = "xxxxxxxx";
        sReportMqttUrl = "mqtt://xx.xx.xx.xx:1883";

        sReportScanTimeout = 30;
        sReportPtRecvMac = "xx.xx.xx.xx.xx.xx";
        sReportPtRecvSeq = 1;
        sReportPtRecvState = "CONNECT_SUCCESS";
        sReportTransmitMac = "xx.xx.xx.xx.xx.xx";
        sReportTransmitData = "xxxxxxxxx";
        sReportMode = "DC";
        sReportLevel = 50;

        sTransmitData = "xxxxxxxxxxxxxxxxxxxxx";

        timer = new QTimer(NULL);
    }
};

#define SEQMAX (255)

class SubQTableWidget : public QTableWidget {
public:
    SubQTableWidget(QWidget *parent = nullptr) : QTableWidget(parent) {
        this->installEventFilter(this);
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Tab) {
                return true; // 返回true，表示事件被过滤，空格键不会被处理
            }
        }
        return QTableWidget::eventFilter(watched, event); // 对其他按键进行正常处理
    }
};

class SubWindow : public QMainWindow
{
    Q_OBJECT   

protected:
    void paintEvent(QPaintEvent *);
    void changeEvent(QEvent *event);
    void resizeEvent(QResizeEvent * resizeEvent);
    void mousePressEvent(QMouseEvent *event);
public:
    SubWindow(QWidget *parent = nullptr);
    int32_t JumpWindowToMainContext(sConfigTogether *config, const QString &topic);
    int32_t JumpWindowToContext(QString &match, sConfigTogether *config, const QString &topic);
    int32_t JumpWindowToContextTransmit(int32_t number, sConfigTogether *config, const QString &topics);
    int32_t JumpWindowToRealTimeRefreshContext(sConfigTogether *config, const QString &topic);
    int32_t SetCurrentIndex(int32_t number);
    int32_t JumpMainWindow(void);
    int32_t JumpWindowToLogin();
    int32_t JumpWindowToServerList();
    int32_t ClearQnavigationWidget(void);
    void ClearWidget(void);
    int32_t RecvMqttMessage(sConfigTogether *config, const QByteArray message, const QMqttTopicName topic);
    QString getLocalIP(void);
    ~SubWindow();
    QVBoxLayout *rightLayout;
    QHBoxLayout *mainLayout;
    QVBoxLayout *subVLayout;
    QHBoxLayout *subHLayout;
    QPushButton *enter;
    QPushButton *cancel;
    QPushButton *search;
    QLineEdit *lineEdit0;
    QLineEdit *lineEdit1;
    QLineEdit *lineEdit2;
    QLineEdit *lineEdit3;
    QLineEdit *lineEdit4;
    QLineEdit *lineEdit5;
    QLabel *label;
    QLabel *label1;
    QLabel *label2;
    QLabel *label3;
    QLabel *label4;
    QLabel *label5;
    int32_t currentIndex;
    QWidget *mainWidget;
    QWidget *rightWidget;
    QWidget *subVWidget;
    QWidget *subHWidget;
    QNavigationWidget *navigationWidget;
    QTableWidget *serverTable;
    QMap<void *, void *> serverTableMap;
    QMap<void *, QString> topicTableMap;
    QTableWidget *contextTable;
    QCheckBox *allCheck;
    const QPalette *palettes;
    QColor *color;
    QColor backgroundColor;
    /*second window*/
    QMainWindow *secondWindow;

    SubWindowType windowType;

    int sRow;
    int sCol;

    int sContextRow;
    int sContextCol;

    //QMqttClient *client;
#if 0
    /*system*/
    float sTemperature;
    QString sVersion;
    QString sInfo;

    /*net status*/
    NetState sNetState;

    /*eth*/
    QString sEthAddress;
    QString sEthNetmask;
    QString sEthGateway;

    /*wifi*/
    QString sWifiSsid;
    QString sWifiPassword;
    QString sWifiAddress;
    QString sWifiNetmask;
    QString sWifiGateway;

    /*mqtt*/
    QString sMqttUser;
    QString sMqttPassword;
    QString sMqttUrl;

    /*power mode*/
    PowerSupplyMode sMode;
    int sLevel;

    SignalSync signalSync;

    /*update*/
    QFileSystemModel *sFileSystemMode;
    QTreeView *sTreeView;

    QString updateFileString;
#endif
    sConfigTogether *clickConfig;
};
#endif // SUBWINDOW_H
