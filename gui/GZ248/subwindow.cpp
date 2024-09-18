#include "subwindow.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <stdio.h>
#include "externqslider.h"
#include "common.h"
#include "transmit.h"
#include <QRegExp>
#include <QRegExpValidator>
#include <QCheckBox>
#include <QComboBox>
#include <QPainter>
#include <QScreen>
#include <QPainterPath>
#include <mqtt.h>
#include <QMap>


const char *PowerMode2Name[] = {
    "DC",
    "BAT"
};

const char *SubWindow2Name[] = {
    "Login",
    "ServerList",
    "MainContext",
    "Context",
};

void SubWindow::resizeEvent(QResizeEvent * resizeEvent) {
    QSize newSize = resizeEvent->size();
    printf ("newSize.width:%d height:%d\n", newSize.width(), newSize.height());

    printf (">>>>>>>>>>>>>>> windowType:%s\n", SubWindow2Name[windowType]);
    // 窗口已最大化，执行相应操作
    switch (windowType) {
    case SubWindowType_Login:
    {
        JumpWindowToLogin();
        break;
    }
    case SubWindowType_ServerList:
    {
        ClearQnavigationWidget();
        JumpWindowToServerList();
        break;
    }
    case SubWindowType_MainContext:
    {
        JumpWindowToMainContext();
        break;
    }
    case SubWindowType_Context:
    {
        JumpWindowToContext(currentIndex);
        break;
    }
    default:break;
    }
}

// 重写showEvent事件处理函数
void SubWindow::changeEvent(QEvent *event) {
    printf ("event->type():%d\n", event->type());
    QRect screenGeometry = this->screen()->geometry();
    printf ("x:%d y:%d w:%d h:%d\n", screenGeometry.x(), screenGeometry.y(), screenGeometry.width(), screenGeometry.height());
    QRect screenGeometry1 = this->geometry();
    printf ("x:%d y:%d w:%d h:%d\n", screenGeometry1.x(), screenGeometry1.y(), screenGeometry1.width(), screenGeometry1.height());
    printf ("w:%d y:%d\n", width(), height());
    if (event->type() == QEvent::WindowStateChange) {
        // 当窗口状态改变时（如最大化），此分支会被触发
        if (isMaximized()) {
            printf (">>>>>>>>>>>>>>> windowType:%s\n", SubWindow2Name[windowType]);
            // 窗口已最大化，执行相应操作
            switch (windowType) {
            case SubWindowType_Login:
            {
                JumpWindowToLogin();
                break;
            }
            case SubWindowType_ServerList:
            {
                break;
            }
            case SubWindowType_MainContext:
            {
                break;
            }
            case SubWindowType_Context:
            {
                break;
            }
            default:break;
            }
        }
    }
    QWidget::changeEvent(event);
}

void SubWindow::paintEvent(QPaintEvent *event) {
    #if 0
    static int32_t ref = 0;
    printf ("sdfasdfasdfasdfasd:%d\n", ref++);
    int32_t qnavigationX = navigationWidget->x();
    int32_t qnavigationY = navigationWidget->y();
    int32_t qnavigationW = navigationWidget->width();
    int32_t qnavigationH = navigationWidget->height();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Draw background color.
    painter.setPen(Qt::NoPen);
    painter.setBrush(backgroundColor);
    painter.drawRect(rect());

    QPainterPath itemPath;
    //QPainterPath *itemPath = QPainterPath();
    itemPath.addRect(QRect(qnavigationX, qnavigationY, qnavigationW, qnavigationH));

    painter.setPen("#202020");
    painter.fillPath(itemPath, backgroundColor);
#endif
}

int32_t SubWindow::ClearQnavigationWidget(void) {
    //repaint();
    //mainLayout->removeWidget(navigationWidget);
    //mainLayout->removeWidget(rightWidget);
    contextTable->disconnect();
    return 0;
}

void SubWindow::ClearWidget(void) {

    //navigationWidget->hide();
    enter->hide();
    cancel->hide();
    search->hide();
    lineEdit0->hide();
    lineEdit0->setEchoMode(QLineEdit::Normal);
    lineEdit1->hide();
    lineEdit1->setEchoMode(QLineEdit::Normal);
    lineEdit2->hide();
    lineEdit2->setEchoMode(QLineEdit::Normal);
    lineEdit3->hide();
    lineEdit3->setEchoMode(QLineEdit::Normal);
    lineEdit4->hide();
    lineEdit4->setEchoMode(QLineEdit::Normal);
    lineEdit5->hide();
    lineEdit5->setEchoMode(QLineEdit::Normal);
    label->hide();
    label1->hide();
    label2->hide();
    label3->hide();
    label4->hide();
    label5->hide();
    allCheck->hide();
    serverTable->hide();
    contextTable->hide();
    enter->disconnect();
    cancel->disconnect();
    search->disconnect();
    allCheck->disconnect();
    //serverTable->disconnect();
    //contextTable->disconnect();
}

int32_t SubWindow::SetCurrentIndex(int32_t number) {
    currentIndex = number;
    return 0;
}

int32_t SubWindow::JumpWindowToContextTransmit(int32_t number) {
    switch (number) {
    case ModuleDataAttr_GetTemperature:
    {
        break;
    }
    case ModuleDataAttr_GetModuleVersion:
    {
        break;
    }
    case ModuleDataAttr_GetModuleInfo:
    {
        break;
    }
    case ModuleDataAttr_SetModuleInfo:
    {
        printf ("mqtt set module info\n");
        break;
    }
    case ModuleDataAttr_GetPower:
    {
        break;
    }
    case ModuleDataAttr_Reboot:
    {
        break;
    }
    case ModuleDataAttr_NetState:
    {
        break;
    }
    case ModuleDataAttr_GetEthCfg:
    {
        break;
    }
    case ModuleDataAttr_SetEthCfg:
    {
        break;
    }
    case ModuleDataAttr_GetWifiCfg:
    {
        break;
    }
    case ModuleDataAttr_SetWifiCfg:
    {
        break;
    }
    case ModuleDataAttr_GetMqttCfg:
    {
        break;
    }
    case ModuleDataAttr_SetMqttCfg:
    {
        break;
    }
    case ModuleDataAttr_Update:
    {
        break;
    }
    case ModuleDataAttr_ReportData:
    {
        break;
    }
    default:break;
    }
    return 0;
}

#define COMMON_WIDTH (200)
#define COMMON_HEIGHT (40)

int32_t SubWindow::JumpWindowToContext(int32_t number) {
    windowType = SubWindowType_Context;
    switch (number) {
    case ModuleDataAttr_GetTemperature:
    {
        //printf ("SSSSSSSSSSSSSSSSSSSSSSSs\n");
        ClearWidget();
        //navigationWidget->show();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        QString strings("温度:");
        QString temperature(QString::number(MQTTGetTemperature()));
        label->setText(strings + temperature);
        label->adjustSize();
        label->show();
        contextTable->show();
        cancel->setText("返回");
        cancel->show();

        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t labelX = 2, labelY = 2, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        label->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);


        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });
        break;
    }
    case ModuleDataAttr_GetModuleVersion:
    {
        ClearWidget();
        //navigationWidget->show();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        QString strings("模块版本:");
        char sVersion[128];
        MQTTGetModuleVersion(sVersion, sizeof(sVersion));
        QString version(sVersion);
        label->setText(strings + version);
        label->adjustSize();
        label->show();
        cancel->setText("返回");
        contextTable->show();
        cancel->show();

        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t labelX = 2, labelY = 2, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        label->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);


        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_GetModuleInfo:
    {
        ClearWidget();
        //navigationWidget->show();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        QString strings("模块信息:");
        char sInfo[128];
        MQTTGetModuleInfo(sInfo, sizeof(sInfo));
        QString info(sInfo);
        //printf ("info:%s\n", strings + info);
        label->setText(strings + info);
        label->adjustSize();
        label->show();
        contextTable->show();
        cancel->setText("返回");
        cancel->show();

        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t labelX = 2, labelY = 2, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        label->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);


        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_SetModuleInfo:
    {
        ClearWidget();
        //navigationWidget->show();
        //subHLayout->addWidget(label, 0, Qt::AlignTop);
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(lineEdit0, 0, Qt::AlignTop);
        rightLayout->addWidget(enter, 0, Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("确定");
        //label->setText("请输入模块版本:");
        lineEdit0->setPlaceholderText("请输入模块版本:");
        //label->show();
        lineEdit0->clear();
        lineEdit0->show();
        enter->show();
        contextTable->show();
        cancel->setText("返回");
        cancel->show();

        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(lineEdit0);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t labelX = 2, labelY = 2, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        lineEdit0->setGeometry(labelX, labelY, labelW, labelH);
        labelX = 2, labelY += labelH, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        enter->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);


        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex);
        });

        break;
    }
    case ModuleDataAttr_GetPower:
    {
        PowerSupplyMode mode;
        int level;
        ClearWidget();
        //navigationWidget->show();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        QString strings("模块信息:");
        MQTTGetPower(&mode, &level);
        QString info((QString)"PowerMode<" + (QString)PowerMode2Name[mode] + (QString)">  PowerLevel<" + QString::number(level) + (QString)">");
        //printf ("info:%s\n", strings + info);
        label->setText(strings + info);
        label->adjustSize();
        label->show();
        contextTable->show();
        cancel->setText("返回");
        cancel->show();

        mainLayout->removeWidget(contextTable);
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);
        mainLayout->removeWidget(rightWidget);

        int32_t labelX = 2, labelY = 2, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        enter->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_Reboot:
    {
        ClearWidget();
        //navigationWidget->show();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(enter, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("重启");
        enter->show();
        cancel->setText("返回");
        contextTable->show();
        cancel->show();

        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t labelX = 2, labelY = 2, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        enter->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);


        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex);
        });

        break;
    }
    case ModuleDataAttr_NetState:
    {
        ClearWidget();
        //navigationWidget->show();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        QString strings("网络状态:");
        QString netState((MQTTGetNetState()));
        //printf ("netState:%s\n", strings + netState);
        label->setText(strings + netState);
        label->adjustSize();
        label->show();
        cancel->setText("返回");
        contextTable->show();
        cancel->show();

        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);
        mainLayout->removeWidget(rightWidget);
        mainLayout->removeWidget(contextTable);

        int32_t labelX = 2, labelY = 2, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        enter->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_GetEthCfg:
    {
        ClearWidget();
        //navigationWidget->show();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(label, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(label1, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(label2, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(label3, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(cancel, 0, Qt::AlignTop | Qt::AlignLeft);
        mainLayout->addWidget(rightWidget);
        QString strings("<有线配置>");
        char sAddress[128];
        char sNetmask[128];
        char sGateway[128];
        MQTTGetEthCfg(sAddress, sNetmask, sGateway, sizeof(sAddress));
        //QString ethCfg("<IP:" + sAddress + "><Netmask:" + sNetmask + "><Gateway:" + sGateway + ">");
        //printf ("netState:%s\n", strings + netState);
        label->setText(strings);// + "<IP:" + sAddress + "><Netmask:" + sNetmask + "><Gateway:" + sGateway + ">");
        label1->setText((QString)"网络地址:" + (QString)sAddress);
        label2->setText((QString)"子网掩码:" + (QString)sNetmask);
        label3->setText((QString)"网关地址:" + (QString)sGateway);
        label->show();
        label1->show();
        label2->show();
        label3->show();
        cancel->setText("返回");
        cancel->show();
        contextTable->show();

        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);
        rightLayout->removeWidget(label1);
        rightLayout->removeWidget(label2);
        rightLayout->removeWidget(label3);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t left, right, top, bottom;
        rightLayout->setContentsMargins(2, 2, 2, 2);
        rightLayout->getContentsMargins(&left, &top, &right, &bottom);
        //printf ("left:%d top:%d right:%d bottom:%d\n",
        //        left, top, right, bottom);
        int32_t labelX = left + 2, labelY = top + 2, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        label->setGeometry(labelX, labelY, labelW, labelH);
        //printf ("label->x():%d label->y():%d width():%d height():%d\n", label->x(), label->y(), label->width(), label->height());
        int32_t label1X = labelX, label1Y = labelY + labelH, label1W = COMMON_WIDTH, label1H = COMMON_HEIGHT;
        label1->setGeometry(label1X, label1Y, label1W, label1H);
        int32_t label2X = label1X, label2Y = label1Y + label1H, label2W = COMMON_WIDTH, label2H = COMMON_HEIGHT;
        label2->setGeometry(label2X, label2Y, label2W, label2H);
        int32_t label3X = label2X, label3Y = label2Y + label2H, label3W = COMMON_WIDTH, label3H = COMMON_HEIGHT;
        label3->setGeometry(label3X, label3Y, label3W, label3H);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_SetEthCfg:
    {
        ClearWidget();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(label, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit0, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit1, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit2, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(enter, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(cancel, 0, Qt::AlignTop | Qt::AlignLeft);
        mainLayout->addWidget(rightWidget);
        enter->setText("确定");
        label->setText("请输入有线配置:");
        lineEdit0->setPlaceholderText("网络地址");
        lineEdit1->setPlaceholderText("子网掩码");
        lineEdit2->setPlaceholderText("网关地址");
        label->show();
        lineEdit0->clear();
        lineEdit1->clear();
        lineEdit2->clear();
        lineEdit0->show();
        lineEdit1->show();
        lineEdit2->show();
        contextTable->show();
        enter->show();

        rightLayout->removeWidget(label);
        rightLayout->removeWidget(lineEdit0);
        rightLayout->removeWidget(lineEdit1);
        rightLayout->removeWidget(lineEdit2);
        rightLayout->removeWidget(enter);
        rightLayout->removeWidget(cancel);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        printf ("contextTable->x:%d y:%d w:%d h:%d\n", contextTable->x(), contextTable->y(), contextTable->width(), contextTable->height());
        printf ("rightWidget x:%d y:%d w:%d h:%d\n", rightWidget->x(), rightWidget->y(), rightWidget->width(), rightWidget->height());
        int32_t left, right, top, bottom;
        rightLayout->setContentsMargins(2, 2, 2, 2);
        rightLayout->getContentsMargins(&left, &top, &right, &bottom);
        printf ("left:%d top:%d right:%d bottom:%d\n",
                left, top, right, bottom);
        int32_t labelX = left + 2, labelY = top + 2, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        label->setGeometry(labelX, labelY, labelW, labelH);
        printf ("label->x():%d label->y():%d width():%d height():%d\n", label->x(), label->y(), label->width(), label->height());
        int32_t lineEdit0X = labelX, lineEdit0Y = labelY + labelH, lineEdit0W = COMMON_WIDTH, lineEdit0H = COMMON_HEIGHT;
        lineEdit0->setGeometry(lineEdit0X, lineEdit0Y, lineEdit0W, lineEdit0H);
        printf ("label->x():%d label->y():%d width():%d height():%d\n", lineEdit0->x(), lineEdit0->y(), lineEdit0->width(), lineEdit0->height());
        int32_t lineEdit1X = labelX, lineEdit1Y = lineEdit0Y + lineEdit0H, lineEdit1W = COMMON_WIDTH, lineEdit1H = COMMON_HEIGHT;
        lineEdit1->setGeometry(lineEdit1X, lineEdit1Y, lineEdit1W, lineEdit1H);
        printf ("label->x():%d label->y():%d width():%d height():%d\n", lineEdit1->x(), lineEdit1->y(), lineEdit1->width(), lineEdit1->height());
        int32_t lineEdit2X = labelX, lineEdit2Y = lineEdit1Y + lineEdit1H, lineEdit2W = COMMON_WIDTH, lineEdit2H = COMMON_HEIGHT;
        lineEdit2->setGeometry(lineEdit2X, lineEdit2Y, lineEdit2W, lineEdit2H);
        printf ("label->x():%d label->y():%d width():%d height():%d\n", lineEdit2->x(), lineEdit2->y(), lineEdit2->width(), lineEdit2->height());
        int32_t enterX = labelX, enterY = lineEdit2Y + lineEdit1H, enterW = 40, enterH = COMMON_HEIGHT;
        enter->setGeometry(enterX, enterY, enterW, enterH);
        printf ("label->x():%d label->y():%d width():%d height():%d\n", enter->x(), enter->y(), enter->width(), enter->height());

        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);
update();


        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex);
        });

        break;
    }
    case ModuleDataAttr_GetWifiCfg:
    {
        ClearWidget();
        //navigationWidget->show();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(label, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(label1, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(label2, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(label3, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(label4, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(label5, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        QString strings("<无线配置>");
        char sSsid[128];
        char sPassword[128];
        char sAddress[128];
        char sNetmask[128];
        char sGateway[128];
        MQTTGetWifiCfg(sSsid, sPassword, sAddress, sNetmask, sGateway, sizeof(sAddress));
        //QString ethCfg("<IP:" + sAddress + "><Netmask:" + sNetmask + "><Gateway:" + sGateway + ">");
        //printf ("netState:%s\n", strings + netState);
        label->setText(strings);// + "<IP:" + sAddress + "><Netmask:" + sNetmask + "><Gateway:" + sGateway + ">");
        label1->setText((QString)"无线账号:" + (QString)sSsid);
        label2->setText((QString)"无线密码:" + (QString)sPassword);
        label3->setText((QString)"网络地址:" + (QString)sAddress);
        label4->setText((QString)"子网掩码:" + (QString)sNetmask);
        label5->setText((QString)"网关地址:" + (QString)sGateway);
        label->show();
        label1->show();
        label2->show();
        label3->show();
        label4->show();
        label5->show();
        contextTable->show();

        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);
        rightLayout->removeWidget(label1);
        rightLayout->removeWidget(label2);
        rightLayout->removeWidget(label3);
        mainLayout->removeWidget(contextTable);
        rightLayout->removeWidget(label4);
        rightLayout->removeWidget(label5);
        mainLayout->removeWidget(rightWidget);

        int32_t left, right, top, bottom;
        rightLayout->setContentsMargins(2, 2, 2, 2);
        rightLayout->getContentsMargins(&left, &top, &right, &bottom);
        //printf ("left:%d top:%d right:%d bottom:%d\n",
        //        left, top, right, bottom);
        int32_t labelX = left + 2, labelY = top + 2, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        label->setGeometry(labelX, labelY, labelW, labelH);
        //printf ("label->x():%d label->y():%d width():%d height():%d\n", label->x(), label->y(), label->width(), label->height());
        int32_t label1X = labelX, label1Y = labelY + labelH + 2, label1W = COMMON_WIDTH, label1H = COMMON_HEIGHT;
        label1->setGeometry(label1X, label1Y, label1W, label1H);
        int32_t label2X = label1X, label2Y = label1Y + label1H, label2W = COMMON_WIDTH, label2H = COMMON_HEIGHT;
        label2->setGeometry(label2X, label2Y, label2W, label2H);
        int32_t label3X = label2X, label3Y = label2Y + label2H, label3W = COMMON_WIDTH, label3H = COMMON_HEIGHT;
        label3->setGeometry(label3X, label3Y, label3W, label3H);
        int32_t label4X = label3X, label4Y = label3Y + label3H, label4W = COMMON_WIDTH, label4H = COMMON_HEIGHT;
        label4->setGeometry(label4X, label4Y, label4W, label4H);
        int32_t label5X = label4X, label5Y = label4Y + label4H, label5W = COMMON_WIDTH, label5H = COMMON_HEIGHT;
        label5->setGeometry(label5X, label5Y, label5W, label5H);
        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_SetWifiCfg:
    {
        ClearWidget();
        //navigationWidget->show();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(label, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit0, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit1, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit2, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit3, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit4, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(enter, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("确定");
        label->setText("请输入无线配置:");
        lineEdit0->setPlaceholderText("无线账号");
        lineEdit1->setPlaceholderText("无线密码");
        lineEdit2->setPlaceholderText("网络地址");
        lineEdit3->setPlaceholderText("子网掩码");
        lineEdit4->setPlaceholderText("网关地址");
        label->show();
        lineEdit0->clear();
        lineEdit1->clear();
        lineEdit2->clear();
        lineEdit3->clear();
        lineEdit4->clear();
        lineEdit0->show();
        lineEdit1->show();
        lineEdit2->show();
        lineEdit3->show();
        lineEdit4->show();
        contextTable->show();
        enter->show();

        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);
        rightLayout->removeWidget(lineEdit0);
        rightLayout->removeWidget(lineEdit1);
        rightLayout->removeWidget(lineEdit2);
        rightLayout->removeWidget(lineEdit3);
        mainLayout->removeWidget(contextTable);
        rightLayout->removeWidget(lineEdit4);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(rightWidget);

        int32_t left, right, top, bottom;
        rightLayout->setContentsMargins(2, 2, 2, 2);
        rightLayout->getContentsMargins(&left, &top, &right, &bottom);
        //printf ("left:%d top:%d right:%d bottom:%d\n",
        //        left, top, right, bottom);
        int32_t labelX = left + 2, labelY = top + 2, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        label->setGeometry(labelX, labelY, labelW, labelH);
        //printf ("label->x():%d label->y():%d width():%d height():%d\n", label->x(), label->y(), label->width(), label->height());
        int32_t lineEdit0X = labelX, lineEdit0Y = labelY + labelH + 2, lineEdit0W = COMMON_WIDTH, lineEdit0H = COMMON_HEIGHT;
        lineEdit0->setGeometry(lineEdit0X, lineEdit0Y, lineEdit0W, lineEdit0H);
        int32_t lineEdit1X = labelX, lineEdit1Y = lineEdit0Y + lineEdit0H, lineEdit1W = COMMON_WIDTH, lineEdit1H = COMMON_HEIGHT;
        lineEdit1->setGeometry(lineEdit1X, lineEdit1Y, lineEdit1W, lineEdit1H);
        int32_t lineEdit2X = labelX, lineEdit2Y = lineEdit1Y + lineEdit1H, lineEdit2W = COMMON_WIDTH, lineEdit2H = COMMON_HEIGHT;
        lineEdit2->setGeometry(lineEdit2X, lineEdit2Y, lineEdit2W, lineEdit2H);
        int32_t lineEdit3X = lineEdit2X, lineEdit3Y = lineEdit2Y + lineEdit2H, lineEdit3W = COMMON_WIDTH, lineEdit3H = COMMON_HEIGHT;
        lineEdit3->setGeometry(lineEdit3X, lineEdit3Y, lineEdit3W, lineEdit3H);
        int32_t lineEdit4X = lineEdit3X, lineEdit4Y = lineEdit3Y + lineEdit3H, lineEdit4W = COMMON_WIDTH, lineEdit4H = COMMON_HEIGHT;
        lineEdit4->setGeometry(lineEdit4X, lineEdit4Y, lineEdit4W, lineEdit4H);
        int32_t enterX = lineEdit4X, enterY = lineEdit4Y + lineEdit4H, enterW = 40, enterH = COMMON_HEIGHT;
        enter->setGeometry(enterX, enterY, enterW, enterH);

        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex);
        });

        break;
    }
    case ModuleDataAttr_GetMqttCfg:
    {
        ClearWidget();
        //navigationWidget->show();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(label, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(label1, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(label2, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(label3, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        QString strings("<MQTT配置>");
        char sUser[128];
        char sPassword[128];
        char sUrl[128];
        MQTTGetMQTTCfg(sUser, sPassword, sUrl, sizeof(sUser));
        //QString ethCfg("<IP:" + sAddress + "><Netmask:" + sNetmask + "><Gateway:" + sGateway + ">");
        //printf ("netState:%s\n", strings + netState);
        label->setText(strings);// + "<IP:" + sAddress + "><Netmask:" + sNetmask + "><Gateway:" + sGateway + ">");
        label1->setText((QString)"用户账号:" + (QString)sUser);
        label2->setText((QString)"用户密码:" + (QString)sPassword);
        label3->setText((QString)"服务地址:" + (QString)sUrl);
        label->show();
        label1->show();
        label2->show();
        label3->show();
        contextTable->show();

        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);
        rightLayout->removeWidget(label1);
        rightLayout->removeWidget(label2);
        rightLayout->removeWidget(label3);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t left, right, top, bottom;
        rightLayout->setContentsMargins(2, 2, 2, 2);
        rightLayout->getContentsMargins(&left, &top, &right, &bottom);
        //printf ("left:%d top:%d right:%d bottom:%d\n",
        //        left, top, right, bottom);
        int32_t labelX = left + 2, labelY = top + 2, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        label->setGeometry(labelX, labelY, labelW, labelH);
        //printf ("label->x():%d label->y():%d width():%d height():%d\n", label->x(), label->y(), label->width(), label->height());
        int32_t label1X = labelX, label1Y = labelY + labelH + 2, label1W = COMMON_WIDTH, label1H = COMMON_HEIGHT;
        label1->setGeometry(label1X, label1Y, label1W, label1H);
        int32_t label2X = label1X, label2Y = label1Y + label1H, label2W = COMMON_WIDTH, label2H = COMMON_HEIGHT;
        label2->setGeometry(label2X, label2Y, label2W, label2H);
        int32_t label3X = label2X, label3Y = label2Y + label2H, label3W = COMMON_WIDTH, label3H = COMMON_HEIGHT;
        label3->setGeometry(label3X, label3Y, label3W, label3H);
        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_SetMqttCfg:
    {
        ClearWidget();
        //navigationWidget->show();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(label, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit0, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit1, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit2, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(enter, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("确定");
        label->setText("请输入MQTT配置:");
        lineEdit0->setPlaceholderText("用户账号");
        lineEdit1->setPlaceholderText("用户密码");
        lineEdit2->setPlaceholderText("服务地址");
        lineEdit0->clear();
        lineEdit1->clear();
        lineEdit2->clear();
        label->show();
        lineEdit0->show();
        lineEdit1->show();
        lineEdit2->show();
        contextTable->show();
        enter->show();

        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);
        rightLayout->removeWidget(lineEdit0);
        rightLayout->removeWidget(lineEdit1);
        rightLayout->removeWidget(lineEdit2);
        mainLayout->removeWidget(contextTable);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(rightWidget);


        int32_t left, right, top, bottom;
        rightLayout->setContentsMargins(2, 2, 2, 2);
        rightLayout->getContentsMargins(&left, &top, &right, &bottom);
        int32_t labelX = left + 2, labelY = top + 2, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        label->setGeometry(labelX, labelY, labelW, labelH);
        int32_t lineEdit0X = labelX, lineEdit0Y = labelY + labelH + 2, lineEdit0W = COMMON_WIDTH, lineEdit0H = COMMON_HEIGHT;
        lineEdit0->setGeometry(lineEdit0X, lineEdit0Y, lineEdit0W, lineEdit0H);
        int32_t lineEdit1X = labelX, lineEdit1Y = lineEdit0Y + lineEdit0H, lineEdit1W = COMMON_WIDTH, lineEdit1H = COMMON_HEIGHT;
        lineEdit1->setGeometry(lineEdit1X, lineEdit1Y, lineEdit1W, lineEdit1H);
        int32_t lineEdit2X = labelX, lineEdit2Y = lineEdit1Y + lineEdit1H, lineEdit2W = COMMON_WIDTH, lineEdit2H = COMMON_HEIGHT;
        lineEdit2->setGeometry(lineEdit2X, lineEdit2Y, lineEdit2W, lineEdit2H);
        int32_t enterX = lineEdit2X, enterY = lineEdit2Y + lineEdit2H, enterW = 40, enterH = COMMON_HEIGHT;
        enter->setGeometry(enterX, enterY, enterW, enterH);

        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex);
        });

        break;
    }
    case ModuleDataAttr_Update:
    {
        break;
    }
    case ModuleDataAttr_ReportData:
    {
        break;
    }
    default:break;
    }

    return 0;
}

int32_t SubWindow::JumpWindowToMainContext(void) {
    ClearWidget();

    mainLayout->addWidget(contextTable);
    mainLayout->addWidget(rightWidget);
    windowType = SubWindowType_MainContext;

    setCentralWidget(mainWidget);

    for (int32_t index = 0; index < ModuleDataAttr_Cnt; index++) {
        if (!strcmp(toEnumChineseString((ModuleDataAttr)index), "Ack")) continue;
        int32_t rowCount = contextTable->rowCount();
        contextTable->insertRow(rowCount);
        printf ("rowCount:%d\n", rowCount);
        printf ("label:%s\n", toEnumChineseString((ModuleDataAttr)index));
        QTableWidgetItem *item = new QTableWidgetItem(QString::fromUtf8(toEnumChineseString((ModuleDataAttr)index)));
        contextTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        contextTable->setItem(rowCount, 0, item);

    }

    contextTable->show();

    mainLayout->removeWidget(rightWidget);
    mainLayout->removeWidget(contextTable);

    //单元格被点击处理
    connect(contextTable, &QTableWidget::cellClicked, this, [=](int32_t row, int32_t col){
        //rightWidget->resize(this->width() - contextTable->width(), this->height());
        JumpWindowToContext(row);
        contextTable->selectRow(row);
    });

    return 0;
}

int32_t SubWindow::JumpWindowToServerList(void) {
    ClearWidget();
    rightLayout->addWidget(lineEdit0, 0, Qt::AlignTop | Qt::AlignLeft);
    rightLayout->addWidget(lineEdit1, 0, Qt::AlignTop | Qt::AlignLeft);
    rightLayout->addWidget(lineEdit2, 0, Qt::AlignTop | Qt::AlignLeft);
    rightLayout->addWidget(serverTable, 0, Qt::AlignTop | Qt::AlignLeft);
    rightLayout->addWidget(enter, 0, Qt::AlignTop | Qt::AlignLeft);
    rightLayout->addWidget(cancel, 0, Qt::AlignTop | Qt::AlignLeft);
    rightLayout->addWidget(lineEdit3, 0, Qt::AlignTop | Qt::AlignLeft);
    rightLayout->addWidget(allCheck);
    mainLayout->addWidget(rightWidget);
    windowType = SubWindowType_ServerList;

    lineEdit3->setPlaceholderText("请输入要搜索的内容...");
    lineEdit0->setPlaceholderText("用户账号");
    lineEdit1->setPlaceholderText("用户密码");
    lineEdit2->setPlaceholderText("服务地址");
    allCheck->setText("全选");
    allCheck->show();
    lineEdit0->clear();
    lineEdit1->clear();
    lineEdit2->clear();
    lineEdit3->clear();
    lineEdit0->show();
    lineEdit1->show();
    lineEdit2->show();
    lineEdit3->show();
    serverTable->show();
    enter->setText("添加");
    enter->show();
    cancel->setText("删除");
    cancel->show();

    int32_t left, right, top, bottom;
    subVLayout->getContentsMargins(&left, &top, &right, &bottom);
    int32_t labelX = left + 2, labelY = top + 2, labelW = 200, labelH = 20;
    lineEdit0->setGeometry(labelX, labelY, labelW, labelH);
    int32_t label1X = left + 2, label1Y = labelY + labelH, label1W = 200, label1H = 20;
    lineEdit1->setGeometry(label1X, label1Y, label1W, label1H);
    int32_t label2X = left + 2, label2Y = label1Y + label1H, label2W = 200, label2H = 20;
    lineEdit2->setGeometry(label2X, label2Y, label2W, label2H);
    int32_t enterX = label2W + 10, enterY = label2Y, enterW = 100, enterH = 20;
    enter->setGeometry(enterX, enterY, enterW, enterH);
    int32_t cancelX = enterX + enterW + 10, cancelY = label2Y, cancelW = 100, cancelH = 20;
    cancel->setGeometry(cancelX, cancelY, cancelW, cancelH);
    int32_t checkX = cancelX + cancelW + 10, checkY = cancelY, checkW = 50, checkH = 20;
    allCheck->setGeometry(checkX, checkY, checkW, checkH);
    int32_t tableX = left + 2, tableY = cancelY + cancelH, tableW = mainWidget->width() - 10, tableH = mainWidget->height() - labelH - label1H - label2H - enterH;
    serverTable->setGeometry(tableX, tableY, tableW, tableH);
    int32_t searchX = left + 2, searchY = mainWidget->height() - 20, searchW = 200, searchH = 20;
    lineEdit3->setGeometry(searchX, searchY, searchW, searchH);

    rightLayout->removeWidget(lineEdit0);
    rightLayout->removeWidget(lineEdit1);
    rightLayout->removeWidget(lineEdit2);
    rightLayout->removeWidget(lineEdit3);
    rightLayout->removeWidget(enter);
    rightLayout->removeWidget(cancel);
    rightLayout->removeWidget(allCheck);
    rightLayout->removeWidget(serverTable);
    mainLayout->removeWidget(rightWidget);
#if 0
    void *CreateMQTTCollection(const char *username,
                               const char *password,
                               const char *hostname,
                               int32_t port,
                               int32_t qos,
                               MQTTConnect pushConnect,
                               MQTTDisconnect pushDisconnect,
                               MQTTPushlish pushPushlish,
                               MQTTConnect scribeConnect,
                               MQTTDisconnect scribeDisconnect,
                               MQTTSubscribe scribeScribe,
                               MQTTMessage scribeMessage);
#endif
    //添加客户端列表成员
    connect(enter, &QPushButton::clicked, this, [=](){
        QString userString = lineEdit0->text();
        QString passwordString = lineEdit1->text();
        QString netString = lineEdit2->text();
        int rowCount = serverTable->rowCount();
        serverTable->insertRow(rowCount);
        QTableWidgetItem *item = new QTableWidgetItem(userString);
#if 0
        collect = CreateMQTTCollection(userString.toStdString().c_str(),
                                       passwordString.toStdString().c_str(),
                                       netString.toStdString().c_str(),
                                       )
        serverTableMap.insert((long)item, )
     #endif
        //char buffer[128];
        //snprintf (buffer, sizeof(buffer) - 1, "%p", item);
        //serverTable->setSelectionMode(QAbstractItemView::SingleSelection);
        serverTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        serverTable->setItem(rowCount, 0, item);
        item->setCheckState(Qt::Unchecked);
        item = new QTableWidgetItem(passwordString);
        //item->setFlags(Qt::ItemIsEnabled);
        serverTable->setItem(rowCount, 1, item);
        item = new QTableWidgetItem(netString);
        //item->setFlags(Qt::ItemIsEnabled);
        serverTable->setItem(rowCount, 2, item);
        item = new QTableWidgetItem(QString("进入"));
        //item->setFlags(Qt::ItemIsEnabled);
        serverTable->setItem(rowCount, 3, item);
        //serverTable->setProperty("MQTTSubscribe", );
        //serverTable->setProperty(MQTTNewPublish);

    });
    //删除客户端列表成员
    connect(cancel, &QPushButton::clicked, this, [=](){
        int32_t sRow = serverTable->rowCount();
        for (int32_t row = sRow - 1; row >= 0; row--) {
            //获取勾选框
            QTableWidgetItem *item = serverTable->item(row, 0);
            if (item->checkState() == Qt::Checked) {
                serverTable->removeRow(row);
            }
        }
        if (serverTable->rowCount() == 0) {
            //全选后恢复至不全选
            if (allCheck->checkState() == Qt::Checked) {
                allCheck->setCheckState(Qt::Unchecked);
            }
        }
    });

    //复选框状态变化
    connect(allCheck, &QCheckBox::stateChanged, this, [=](int32_t state){
        int32_t sRow = serverTable->rowCount();
        for (int32_t row = sRow - 1; row >= 0; row--) {
            //获取勾选框
            if (state == Qt::Checked) {
                QTableWidgetItem *item = serverTable->item(row, 0);
                item->setCheckState(Qt::Checked);
            }
            else if (state == Qt::Unchecked) {
                QTableWidgetItem *item = serverTable->item(row, 0);
                item->setCheckState(Qt::Unchecked);
            }
        }
    });

    //单元格被点击处理
    connect(serverTable, &QTableWidget::cellClicked, this, [=](int32_t row, int32_t col){
        //printf (">>>>>>>>>>>>>>> row:%d col:%d\n", row, col);
        if (col == 3) {//只有第3列需要有点击响应
            JumpWindowToMainContext();
        }
    });

    return 0;
}


int32_t SubWindow::JumpWindowToLogin(void) {
    ClearWidget();
    rightLayout->addWidget(lineEdit0, 0, Qt::AlignTop | Qt::AlignLeft);
    rightLayout->addWidget(lineEdit1, 0, Qt::AlignTop | Qt::AlignLeft);
    rightLayout->addWidget(enter, 0, Qt::AlignTop | Qt::AlignLeft);
    rightLayout->addWidget(cancel, 0, Qt::AlignTop | Qt::AlignLeft);

    //rightLayout->addLayout(subVLayout);
    enter->setText("确定");
    cancel->setText("取消:");
    lineEdit0->setPlaceholderText("用户账号");
    lineEdit0->setClearButtonEnabled(true);
    lineEdit1->setPlaceholderText("用户密码");
    lineEdit1->setClearButtonEnabled(true);
    lineEdit1->setEchoMode(QLineEdit::Password);
    mainLayout->addWidget(rightWidget);
    setCentralWidget(mainWidget);
    windowType = SubWindowType_Login;


    lineEdit0->show();
    lineEdit1->show();
    enter->show();
    cancel->show();

    int32_t x = 0, y = 0, w = this->width(), h = this->height();
    int32_t middleX = w / 2, middleY = h / 2;

    int32_t labelX = middleX - 100, labelY = middleY - 30;
    //int32_t labelX = 220, labelY = 220;
    printf ("w:%d h:%d middlex:%d middleY:%d labelX:%d labelY:%d\n",
            w, h, middleX, middleY, labelX, labelY);
    int32_t hW = subHWidget->width(), hH = subHWidget->height();
    printf ("hw:%d hh:%d\n", hW, hH);

    lineEdit0->setGeometry(labelX, labelY, 200, 20);
    printf ("labelX:%d labelY:%d\n", labelX, labelY);
    lineEdit1->setGeometry(labelX, labelY + 20, 200, 20);
    enter->setGeometry(labelX, labelY + 40, 50, 20);
    cancel->setGeometry(labelX + 200 - 50, labelY + 40, 50, 20);
    printf ("lineedit0 x:%d y:%d w:%d h:%d\n", lineEdit0->x(), lineEdit0->y(), lineEdit0->width(), lineEdit0->height());
    printf ("lineedit0 x:%d y:%d w:%d h:%d\n", lineEdit1->x(), lineEdit1->y(), lineEdit1->width(), lineEdit1->height());
    printf ("lineedit0 x:%d y:%d w:%d h:%d\n", enter->x(), enter->y(), enter->width(), enter->height());
    rightLayout->removeWidget(lineEdit0);
    rightLayout->removeWidget(lineEdit1);
    rightLayout->removeWidget(enter);
    rightLayout->removeWidget(cancel);
    mainLayout->removeWidget(rightWidget);

    connect(enter, &QPushButton::clicked, this, [=](){
        //printf ("admin:%s password:%s\n", lineEdit0->text().toStdString().c_str(), lineEdit1->text().toStdString().c_str());
        //账号密码有效性检测
        if (!strcmp(lineEdit0->text().toStdString().c_str(), "admin")
                && !strcmp(lineEdit1->text().toStdString().c_str(), "123456")) {
            //printf ("enter\n");
            //创建客户端列表
            //serverTable = new QTableWidget(this);
            serverTable->setRowCount(0); // 初始不设置行，动态添加
            serverTable->setColumnCount(4);
            serverTable->setHorizontalHeaderLabels(QStringList() << "用户账号" << "用户密码" << "服务地址" << "操作方式");
            // 启用拖拽
            serverTable->setDragEnabled(true);
            serverTable->setAcceptDrops(true);
            serverTable->setDropIndicatorShown(true);
            //跳转客户端列表添加删除界面
            JumpWindowToServerList();
        }
    });

    return 0;
}

int32_t subWindowPrintf(void *oObj, const char *strings) {
    printf ("%s", strings);
    return 0;
}

SubWindow::SubWindow(QWidget *parent) : QMainWindow(parent)
{
    printf ("w:%d h:%d\n", minimumSize(), maximumSize());
    setMinimumSize(0, 0);
    setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
    resize(600, 400);

    mainWidget = new QWidget;
    rightWidget = new QWidget;
    subVWidget = new QWidget;
    subHWidget = new QWidget;
     //垂直布局器 QVBoxLayout：将内部的控件按照垂直方向排布，一行一个。
    rightLayout = new QVBoxLayout(rightWidget);
    // 水平布局器 QHBoxLayout：将内部的控件按照水平方向排布，一列一个。
    mainLayout = new QHBoxLayout(mainWidget);

    subVLayout = new QVBoxLayout(NULL);
    // 水平布局器 QHBoxLayout：将内部的控件按照水平方向排布，一列一个。
    subHLayout = new QHBoxLayout(NULL);
    navigationWidget = new QNavigationWidget;
    enter = new QPushButton();
    cancel = new QPushButton();
    search = new QPushButton();
    lineEdit0 = new QLineEdit();
    lineEdit1 = new QLineEdit();
    lineEdit2 = new QLineEdit();
    lineEdit3 = new QLineEdit();
    lineEdit4 = new QLineEdit();
    lineEdit5 = new QLineEdit();
    label = new QLabel();
    label1 = new QLabel();
    label2 = new QLabel();
    label3 = new QLabel();
    label4 = new QLabel();
    label5 = new QLabel();
    serverTable = new QTableWidget(this);
    contextTable = new QTableWidget(this);
    contextTable->setFixedWidth(150);
    allCheck = new QCheckBox("全选");
    backgroundColor = "#E4E4E4";
    windowType = SubWindowType_Login;
    //palette = new QPalette();
    //QPixmap pixmap(QString(":/background.png"));
    //palette->setBrush(QPalette::Background, QBrush(pixmap));
    //*palettes = this->palette();
    //*color = palettes->color(this->backgroundRole());

    contextTable->setRowCount(0); // 初始不设置行，动态添加
    contextTable->setColumnCount(1);
    contextTable->setHorizontalHeaderLabels(QStringList() << "功能列表");
    // 启用拖拽
    contextTable->setDragEnabled(true);
    contextTable->setAcceptDrops(true);
    contextTable->setDropIndicatorShown(true);

    MQTTInitLog(this, subWindowPrintf);
    MQTTInit();

    //跳转到登陆界面
    JumpWindowToLogin();
}

SubWindow::~SubWindow()
{
}

