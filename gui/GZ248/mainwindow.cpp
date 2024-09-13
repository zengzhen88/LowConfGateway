#include "mainwindow.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include "qnavigationwidget.h"
#include <stdio.h>
#include "externqslider.h"
#include "common.h"
#include "transmit.h"

const char *PowerMode2Name[] = {
    "DC",
    "BAT"
};

void MainWindow::ClearWidget(void) {
    enter->hide();
    cancel->hide();
    lineEdit0->hide();
    lineEdit1->hide();
    lineEdit2->hide();
    lineEdit3->hide();
    lineEdit4->hide();
    lineEdit5->hide();
    label->hide();
}

int32_t MainWindow::SetCurrentIndex(int32_t number) {
    currentIndex = number;
    return 0;
}

int32_t MainWindow::JumpTransmit(int32_t number) {
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

int32_t MainWindow::JumpSubWindow(int32_t number) {
    switch (number) {
    case ModuleDataAttr_GetTemperature:
    {
        ClearWidget();
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        QString strings("温度:");
        QString temperature(QString::number(MQTTGetTemperature()));
        label->setText(strings + temperature);
        label->adjustSize();
        label->show();
        rightLayout->removeWidget(label);
        break;
    }
    case ModuleDataAttr_GetModuleVersion:
    {
        ClearWidget();
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        QString strings("模块版本:");
        char sVersion[128];
        MQTTGetModuleVersion(sVersion, sizeof(sVersion));
        QString version(sVersion);
        label->setText(strings + version);
        label->adjustSize();
        label->show();
        rightLayout->removeWidget(label);
        break;
    }
    case ModuleDataAttr_GetModuleInfo:
    {
        ClearWidget();
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        QString strings("模块信息:");
        char sInfo[128];
        MQTTGetModuleInfo(sInfo, sizeof(sInfo));
        QString info(sInfo);
        //printf ("info:%s\n", strings + info);
        label->setText(strings + info);
        label->adjustSize();
        label->show();
        rightLayout->removeWidget(label);
        break;
    }
    case ModuleDataAttr_SetModuleInfo:
    {
        ClearWidget();
        subHLayout->addWidget(label, 0, Qt::AlignTop);
        subHLayout->addWidget(lineEdit0, 0, Qt::AlignTop);
        subHLayout->addWidget(enter, 0, Qt::AlignTop);
        rightLayout->addLayout(subHLayout);
        enter->setText("确定");
        label->setText("请输入模块版本:");
        label->show();
        lineEdit0->show();
        enter->show();
        rightLayout->removeItem(subHLayout);
        subHLayout->removeWidget(lineEdit0);
        subHLayout->removeWidget(label);
        subHLayout->removeWidget(enter);
        break;
    }
    case ModuleDataAttr_GetPower:
    {
        PowerSupplyMode mode;
        int level;
        ClearWidget();
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        QString strings("模块信息:");
        MQTTGetPower(&mode, &level);
        QString info((QString)"PowerMode<" + (QString)PowerMode2Name[mode] + (QString)">  PowerLevel<" + QString::number(level) + (QString)">");
        //printf ("info:%s\n", strings + info);
        label->setText(strings + info);
        label->adjustSize();
        label->show();
        rightLayout->removeWidget(label);
        break;
    }
    case ModuleDataAttr_Reboot:
    {
        ClearWidget();
        rightLayout->addWidget(enter, 0, Qt::AlignLeft | Qt::AlignTop);
        enter->setText("重启");
        enter->show();
        rightLayout->removeWidget(enter);
        break;
    }
    case ModuleDataAttr_NetState:
    {
        ClearWidget();
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        QString strings("网络状态:");
        QString netState((MQTTGetNetState()));
        //printf ("netState:%s\n", strings + netState);
        label->setText(strings + netState);
        label->adjustSize();
        label->show();
        rightLayout->removeWidget(label);
        break;
    }
    case ModuleDataAttr_GetEthCfg:
    {
        ClearWidget();
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        QString strings("有线配置:");
        char sAddress[128];
        char sNetmask[128];
        char sGateway[128];
        MQTTGetEthCfg(sAddress, sNetmask, sGateway, sizeof(sAddress));
        //QString ethCfg("<IP:" + sAddress + "><Netmask:" + sNetmask + "><Gateway:" + sGateway + ">");
        //printf ("netState:%s\n", strings + netState);
        label->setText(strings + "<IP:" + sAddress + "><Netmask:" + sNetmask + "><Gateway:" + sGateway + ">");
        label->adjustSize();
        label->show();
        rightLayout->removeWidget(label);
        break;
    }
    case ModuleDataAttr_SetEthCfg:
    {
        ClearWidget();
        subVLayout->addWidget(label, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(lineEdit0, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(lineEdit1, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(lineEdit2, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(enter, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addLayout(subVLayout);
        enter->setText("确定");
        label->setText("请输入有线配置:");
        lineEdit0->setPlaceholderText("网络地址");
        lineEdit1->setPlaceholderText("子网掩码");
        lineEdit2->setPlaceholderText("网关地址");
        label->show();
        lineEdit0->show();
        lineEdit1->show();
        lineEdit2->show();
        enter->show();
        int32_t left, right, top, bottom;
        subVLayout->setContentsMargins(2, 2, 2, 2);
        subVLayout->getContentsMargins(&left, &top, &right, &bottom);
        //printf ("left:%d top:%d right:%d bottom:%d\n",
        //        left, top, right, bottom);
        int32_t labelX = left + 2, labelY = top + 2, labelW = 200, labelH = 20;
        label->setGeometry(labelX, labelY, labelW, labelH);
        //printf ("label->x():%d label->y():%d width():%d height():%d\n", label->x(), label->y(), label->width(), label->height());
        int32_t lineEdit0X = labelX, lineEdit0Y = labelY + labelH + 2, lineEdit0W = 200, lineEdit0H = 20;
        lineEdit0->setGeometry(lineEdit0X, lineEdit0Y, lineEdit0W, lineEdit0H);
        int32_t lineEdit1X = labelX, lineEdit1Y = lineEdit0Y + lineEdit0H, lineEdit1W = 200, lineEdit1H = 20;
        lineEdit1->setGeometry(lineEdit1X, lineEdit1Y, lineEdit1W, lineEdit1H);
        int32_t lineEdit2X = labelX, lineEdit2Y = lineEdit1Y + lineEdit1H, lineEdit2W = 200, lineEdit2H = 20;
        lineEdit2->setGeometry(lineEdit2X, lineEdit2Y, lineEdit2W, lineEdit2H);
        int32_t enterX = labelX, enterY = lineEdit2Y + lineEdit1H, enterW = 40, enterH = 20;
        enter->setGeometry(enterX, enterY, enterW, enterH);

        rightLayout->removeItem(subVLayout);
        subVLayout->removeWidget(lineEdit0);
        subVLayout->removeWidget(label);
        subVLayout->removeWidget(enter);
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

MainWindow::MainWindow(QMainWindow *parent) : QMainWindow(parent)
{
    resize(600, 400);

    QWidget *mainWidget = new QWidget;
    QWidget *rightWidget = new QWidget;
     //垂直布局器 QVBoxLayout：将内部的控件按照垂直方向排布，一行一个。
    rightLayout = new QVBoxLayout(rightWidget);
    // 水平布局器 QHBoxLayout：将内部的控件按照水平方向排布，一列一个。
    mainLayout = new QHBoxLayout(mainWidget);

    subVLayout = new QVBoxLayout(NULL);
    // 水平布局器 QHBoxLayout：将内部的控件按照水平方向排布，一列一个。
    subHLayout = new QHBoxLayout(NULL);
    QNavigationWidget *navigationWidget = new QNavigationWidget;
    enter = new QPushButton();
    cancel = new QPushButton();
    lineEdit0 = new QLineEdit();
    lineEdit1 = new QLineEdit();
    lineEdit2 = new QLineEdit();
    lineEdit3 = new QLineEdit();
    lineEdit4 = new QLineEdit();
    lineEdit5 = new QLineEdit();
    label = new QLabel();

    navigationWidget->setRowHeight(30);
    for (int32_t index = 0; index < ModuleDataAttr_Cnt; index++) {
        if (!strcmp(toEnumChineseString((ModuleDataAttr)index), "Ack")) continue;
        navigationWidget->addItem("GZ248", toEnumChineseString((ModuleDataAttr)index));
    }

    //设置内部控件或子布局器距离四个边的边距
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(navigationWidget);
    //mainLayout->addWidget(slider);
    mainLayout->addWidget(rightWidget);

    setCentralWidget(mainWidget);

    connect(navigationWidget, &QNavigationWidget::currentItemChanged, this, [=](const int &current){
        SetCurrentIndex(current);
        JumpSubWindow(current);
    });
    connect(enter, &QPushButton::clicked, this, [=](){
        JumpTransmit(currentIndex);
    });
}

MainWindow::~MainWindow()
{
}

