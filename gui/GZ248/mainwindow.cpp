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
    case ModuleDataAttr_SetModuleInfo:
    {
        QString text = lineEdit0->text();
        label->setText(text);
        label->show();
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
        QString strings("温度:");
        QString temperature(QString::number(MQTTGetTemperature()));
        label->setText(strings + temperature);
        label->show();
        break;
    }
    case ModuleDataAttr_GetModuleVersion:
    {
        QString strings("模块版本:");
        QString version((MQTTGetModuleVersion()));
        label->setText(strings + version);
        label->show();
        break;
    }
    case ModuleDataAttr_SetModuleInfo:
    {
        //QString strings("模块版本:");
        //QString version((MQTTGetModuleVersion()));
        lineEdit0->setPlaceholderText("请输入模块版本");
        lineEdit0->show();
        //label->setText(strings + version);
        //label->show();
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

MainWindow::MainWindow(QMainWindow *parent) : QMainWindow(parent)
{
    resize(600, 400);

    QWidget *mainWidget = new QWidget;
    QWidget *rightWidget = new QWidget;
     //垂直布局器 QVBoxLayout：将内部的控件按照垂直方向排布，一行一个。
    rightLayout = new QVBoxLayout(rightWidget);
    // 水平布局器 QHBoxLayout：将内部的控件按照水平方向排布，一列一个。
    mainLayout = new QHBoxLayout(mainWidget);
    QNavigationWidget *navigationWidget = new QNavigationWidget;
    enter = new QPushButton("enter");
    cancel = new QPushButton("cancel");
    lineEdit0 = new QLineEdit("line0");
    lineEdit1 = new QLineEdit("line1");
    lineEdit2 = new QLineEdit("line2");
    lineEdit3 = new QLineEdit("line3");
    lineEdit4 = new QLineEdit("line4");
    lineEdit5 = new QLineEdit("line5");
    label = new QLabel("label");

    rightLayout->addWidget(lineEdit0, 0, Qt::AlignLeft | Qt::AlignTop);
    rightLayout->addWidget(lineEdit1, 0, Qt::AlignLeft | Qt::AlignTop);
    rightLayout->addWidget(lineEdit2, 0, Qt::AlignLeft | Qt::AlignTop);
    rightLayout->addWidget(lineEdit3, 0, Qt::AlignLeft | Qt::AlignTop);
    rightLayout->addWidget(lineEdit4, 0, Qt::AlignLeft | Qt::AlignTop);
    rightLayout->addWidget(lineEdit5, 0, Qt::AlignLeft | Qt::AlignTop);
    rightLayout->addWidget(enter, 0, Qt::AlignLeft | Qt::AlignTop);
    rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
    rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
    lineEdit0->hide();
    lineEdit1->hide();
    lineEdit2->hide();
    lineEdit3->hide();
    lineEdit4->hide();
    lineEdit5->hide();
    enter->hide();
    cancel->hide();
    label->hide();
    //QSlider *slider = new QSlider();
    //slider->setValue(0);
    //slider->setRange(0, (sizeof(res)/sizeof(res[0])) / 2 - 1);
    //slider->setOrientation(Qt::Vertical);
    //slider->setInvertedAppearance(true);
    // 设置边框样式
    //QString styleSheet = QString("QSlider {"
    //                             "border: 2px solid #000000;" // 边框宽度2px，颜色为黑色
    //                             "border-radius: 5px;"      // 边框圆角5px
    //                             "background: #DDDDDD;"     // 背景颜色
    //                             "}"
    //                             "QProgressBar::chunk {"
    //                             "background: #FFFFFFFF;"      // 进度条颜色
    //                             "border-radius: 5px;"       // 圆角
    //                             "}");

    //slider->setStyleSheet(styleSheet);

    navigationWidget->setRowHeight(30);
    for (int32_t index = 0; index < ModuleDataAttr_Cnt; index++) {
        if (!strcmp(toEnumChineseString((ModuleDataAttr)index), "Ack")) continue;
        navigationWidget->addItem("GZ248", toEnumChineseString((ModuleDataAttr)index));
    }

    //widget 就是要添加的控件指针，stretch 是伸展因子（到 6.5 节再讲这个，本节先不管），伸展因子越大，窗口变大时拉伸越 多，alignment 一般不需要指定，用默认的即可
    //rightLayout->addWidget(tipsLabel, 0, Qt::AlignCenter);


    //设置内部控件或子布局器距离四个边的边距
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(navigationWidget);
    //mainLayout->addWidget(slider);
    mainLayout->addWidget(rightWidget);

    setCentralWidget(mainWidget);

    connect(navigationWidget, &QNavigationWidget::currentItemChanged, this, [=](const int &current){
        JumpSubWindow(current);
    });
    connect(lineEdit0, , this, [=](const int &current){
        JumpTransmit(current);
    });
}

MainWindow::~MainWindow()
{
}

