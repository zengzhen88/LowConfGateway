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
        QString version((MQTTGetModuleVersion()));
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
        QString info((MQTTGetModuleInfo()));
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

