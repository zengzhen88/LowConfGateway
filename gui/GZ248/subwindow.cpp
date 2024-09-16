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

const char *PowerMode2Name[] = {
    "DC",
    "BAT"
};

void SubWindow::ClearWidget(void) {
    //setPalette(*palette);
    //this->palette().setColor(this->backgroundRole(), this->palette().color(this->backgroundRole()));
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
    enter->disconnect();
    cancel->disconnect();
    search->disconnect();
    allCheck->disconnect();
    //serverTable->disconnect();
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


int32_t SubWindow::JumpWindowToContext(int32_t number) {
    switch (number) {
    case ModuleDataAttr_GetTemperature:
    {
        ClearWidget();
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        QString strings("温度:");
        QString temperature(QString::number(MQTTGetTemperature()));
        label->setText(strings + temperature);
        label->adjustSize();
        label->show();
        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = 20;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_GetModuleVersion:
    {
        ClearWidget();
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        QString strings("模块版本:");
        char sVersion[128];
        MQTTGetModuleVersion(sVersion, sizeof(sVersion));
        QString version(sVersion);
        label->setText(strings + version);
        label->adjustSize();
        label->show();
        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = 20;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_GetModuleInfo:
    {
        ClearWidget();
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        QString strings("模块信息:");
        char sInfo[128];
        MQTTGetModuleInfo(sInfo, sizeof(sInfo));
        QString info(sInfo);
        //printf ("info:%s\n", strings + info);
        label->setText(strings + info);
        label->adjustSize();
        label->show();
        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = 20;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_SetModuleInfo:
    {
        ClearWidget();
        //subHLayout->addWidget(label, 0, Qt::AlignTop);
        subHLayout->addWidget(lineEdit0, 0, Qt::AlignTop);
        subHLayout->addWidget(enter, 0, Qt::AlignTop);
        rightLayout->addLayout(subHLayout);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        enter->setText("确定");
        //label->setText("请输入模块版本:");
        lineEdit0->setPlaceholderText("请输入模块版本:");
        //label->show();
        lineEdit0->clear();
        lineEdit0->show();
        enter->show();
        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = 20;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);
        rightLayout->removeWidget(cancel);
        rightLayout->removeItem(subHLayout);
        subHLayout->removeWidget(lineEdit0);
        //subHLayout->removeWidget(label);
        subHLayout->removeWidget(enter);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_GetPower:
    {
        PowerSupplyMode mode;
        int level;
        ClearWidget();
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        QString strings("模块信息:");
        MQTTGetPower(&mode, &level);
        QString info((QString)"PowerMode<" + (QString)PowerMode2Name[mode] + (QString)">  PowerLevel<" + QString::number(level) + (QString)">");
        //printf ("info:%s\n", strings + info);
        label->setText(strings + info);
        label->adjustSize();
        label->show();
        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = 20;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_Reboot:
    {
        ClearWidget();
        rightLayout->addWidget(enter, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        enter->setText("重启");
        enter->show();
        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = 20;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(enter);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_NetState:
    {
        ClearWidget();
        rightLayout->addWidget(label, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        QString strings("网络状态:");
        QString netState((MQTTGetNetState()));
        //printf ("netState:%s\n", strings + netState);
        label->setText(strings + netState);
        label->adjustSize();
        label->show();
        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = 20;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_GetEthCfg:
    {
        ClearWidget();
        subVLayout->addWidget(label, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(label1, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(label2, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(label3, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addLayout(subVLayout);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
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
        int32_t left, right, top, bottom;
        subVLayout->setContentsMargins(2, 2, 2, 2);
        subVLayout->getContentsMargins(&left, &top, &right, &bottom);
        //printf ("left:%d top:%d right:%d bottom:%d\n",
        //        left, top, right, bottom);
        int32_t labelX = left + 2, labelY = top + 2, labelW = 200, labelH = 20;
        label->setGeometry(labelX, labelY, labelW, labelH);
        //printf ("label->x():%d label->y():%d width():%d height():%d\n", label->x(), label->y(), label->width(), label->height());
        int32_t label1X = labelX, label1Y = labelY + labelH + 2, label1W = 200, label1H = 20;
        label1->setGeometry(label1X, label1Y, label1W, label1H);
        int32_t label2X = label1X, label2Y = label1Y + label1H, label2W = 200, label2H = 20;
        label2->setGeometry(label2X, label2Y, label2W, label2H);
        int32_t label3X = label2X, label3Y = label2Y + label2H, label3W = 200, label3H = 20;
        label3->setGeometry(label3X, label3Y, label3W, label3H);
        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = 20;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);
        rightLayout->removeWidget(cancel);
        subVLayout->removeWidget(label);
        subVLayout->removeWidget(label1);
        subVLayout->removeWidget(label2);
        subVLayout->removeWidget(label3);
        rightLayout->removeItem(subVLayout);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            JumpWindowToServerList();
        });

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
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
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

        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = 20;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);
        rightLayout->removeWidget(cancel);
        subVLayout->removeWidget(label);
        subVLayout->removeWidget(lineEdit0);
        subVLayout->removeWidget(lineEdit1);
        subVLayout->removeWidget(lineEdit2);
        rightLayout->removeItem(subVLayout);
        subVLayout->removeWidget(enter);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_GetWifiCfg:
    {
        ClearWidget();
        subVLayout->addWidget(label, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(label1, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(label2, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(label3, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(label4, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(label5, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addLayout(subVLayout);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
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
        int32_t left, right, top, bottom;
        subVLayout->setContentsMargins(2, 2, 2, 2);
        subVLayout->getContentsMargins(&left, &top, &right, &bottom);
        //printf ("left:%d top:%d right:%d bottom:%d\n",
        //        left, top, right, bottom);
        int32_t labelX = left + 2, labelY = top + 2, labelW = 200, labelH = 20;
        label->setGeometry(labelX, labelY, labelW, labelH);
        //printf ("label->x():%d label->y():%d width():%d height():%d\n", label->x(), label->y(), label->width(), label->height());
        int32_t label1X = labelX, label1Y = labelY + labelH + 2, label1W = 200, label1H = 20;
        label1->setGeometry(label1X, label1Y, label1W, label1H);
        int32_t label2X = label1X, label2Y = label1Y + label1H, label2W = 200, label2H = 20;
        label2->setGeometry(label2X, label2Y, label2W, label2H);
        int32_t label3X = label2X, label3Y = label2Y + label2H, label3W = 200, label3H = 20;
        label3->setGeometry(label3X, label3Y, label3W, label3H);
        int32_t label4X = label3X, label4Y = label3Y + label3H, label4W = 200, label4H = 20;
        label4->setGeometry(label4X, label4Y, label4W, label4H);
        int32_t label5X = label4X, label5Y = label4Y + label4H, label5W = 200, label5H = 20;
        label5->setGeometry(label5X, label5Y, label5W, label5H);
        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = 20;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);
        rightLayout->removeWidget(cancel);
        subVLayout->removeWidget(label);
        subVLayout->removeWidget(label1);
        subVLayout->removeWidget(label2);
        subVLayout->removeWidget(label3);
        subVLayout->removeWidget(label4);
        subVLayout->removeWidget(label5);
        rightLayout->removeItem(subVLayout);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_SetWifiCfg:
    {
        ClearWidget();
        subVLayout->addWidget(label, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(lineEdit0, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(lineEdit1, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(lineEdit2, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(lineEdit3, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(lineEdit4, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(enter, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addLayout(subVLayout);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
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
        int32_t lineEdit3X = lineEdit2X, lineEdit3Y = lineEdit2Y + lineEdit2H, lineEdit3W = 200, lineEdit3H = 20;
        lineEdit3->setGeometry(lineEdit3X, lineEdit3Y, lineEdit3W, lineEdit3H);
        int32_t lineEdit4X = lineEdit3X, lineEdit4Y = lineEdit3Y + lineEdit3H, lineEdit4W = 200, lineEdit4H = 20;
        lineEdit4->setGeometry(lineEdit4X, lineEdit4Y, lineEdit4W, lineEdit4H);
        int32_t enterX = lineEdit4X, enterY = lineEdit4Y + lineEdit4H, enterW = 40, enterH = 20;
        enter->setGeometry(enterX, enterY, enterW, enterH);

        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = 20;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);
        rightLayout->removeWidget(cancel);
        subVLayout->removeWidget(label);
        subVLayout->removeWidget(lineEdit0);
        subVLayout->removeWidget(lineEdit1);
        subVLayout->removeWidget(lineEdit2);
        subVLayout->removeWidget(lineEdit3);
        subVLayout->removeWidget(lineEdit4);
        rightLayout->removeItem(subVLayout);
        subVLayout->removeWidget(enter);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_GetMqttCfg:
    {
        ClearWidget();
        subVLayout->addWidget(label, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(label1, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(label2, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(label3, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addLayout(subVLayout);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
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
        int32_t left, right, top, bottom;
        subVLayout->setContentsMargins(2, 2, 2, 2);
        subVLayout->getContentsMargins(&left, &top, &right, &bottom);
        //printf ("left:%d top:%d right:%d bottom:%d\n",
        //        left, top, right, bottom);
        int32_t labelX = left + 2, labelY = top + 2, labelW = 200, labelH = 20;
        label->setGeometry(labelX, labelY, labelW, labelH);
        //printf ("label->x():%d label->y():%d width():%d height():%d\n", label->x(), label->y(), label->width(), label->height());
        int32_t label1X = labelX, label1Y = labelY + labelH + 2, label1W = 200, label1H = 20;
        label1->setGeometry(label1X, label1Y, label1W, label1H);
        int32_t label2X = label1X, label2Y = label1Y + label1H, label2W = 200, label2H = 20;
        label2->setGeometry(label2X, label2Y, label2W, label2H);
        int32_t label3X = label2X, label3Y = label2Y + label2H, label3W = 200, label3H = 20;
        label3->setGeometry(label3X, label3Y, label3W, label3H);
        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = 20;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);
        rightLayout->removeWidget(cancel);
        subVLayout->removeWidget(label);
        subVLayout->removeWidget(label1);
        subVLayout->removeWidget(label2);
        subVLayout->removeWidget(label3);
        rightLayout->removeItem(subVLayout);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            JumpWindowToServerList();
        });

        break;
    }
    case ModuleDataAttr_SetMqttCfg:
    {
        ClearWidget();
        subVLayout->addWidget(label, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(lineEdit0, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(lineEdit1, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(lineEdit2, 0, Qt::AlignTop | Qt::AlignLeft);
        subVLayout->addWidget(enter, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addLayout(subVLayout);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
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
        enter->show();
        int32_t left, right, top, bottom;
        subVLayout->setContentsMargins(2, 2, 2, 2);
        subVLayout->getContentsMargins(&left, &top, &right, &bottom);
        int32_t labelX = left + 2, labelY = top + 2, labelW = 200, labelH = 20;
        label->setGeometry(labelX, labelY, labelW, labelH);
        int32_t lineEdit0X = labelX, lineEdit0Y = labelY + labelH + 2, lineEdit0W = 200, lineEdit0H = 20;
        lineEdit0->setGeometry(lineEdit0X, lineEdit0Y, lineEdit0W, lineEdit0H);
        int32_t lineEdit1X = labelX, lineEdit1Y = lineEdit0Y + lineEdit0H, lineEdit1W = 200, lineEdit1H = 20;
        lineEdit1->setGeometry(lineEdit1X, lineEdit1Y, lineEdit1W, lineEdit1H);
        int32_t lineEdit2X = labelX, lineEdit2Y = lineEdit1Y + lineEdit1H, lineEdit2W = 200, lineEdit2H = 20;
        lineEdit2->setGeometry(lineEdit2X, lineEdit2Y, lineEdit2W, lineEdit2H);
        int32_t enterX = lineEdit2X, enterY = lineEdit2Y + lineEdit2H, enterW = 40, enterH = 20;
        enter->setGeometry(enterX, enterY, enterW, enterH);

        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = 20;
        //printf ("w:%d h:%d\n", rightWidget->width(), rightWidget->height());
        //printf ("returnx:%d returny:%d returnw:%d returnh:%d\n", returnX, returnY, returnW, returnH);
        cancel->setGeometry(returnX, returnY, returnW, returnH);
        rightLayout->removeWidget(cancel);
        subVLayout->removeWidget(label);
        subVLayout->removeWidget(lineEdit0);
        subVLayout->removeWidget(lineEdit1);
        subVLayout->removeWidget(lineEdit2);
        rightLayout->removeItem(subVLayout);
        subVLayout->removeWidget(enter);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            JumpWindowToServerList();
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
    navigationWidget->setRowHeight(30);
    for (int32_t index = 0; index < ModuleDataAttr_Cnt; index++) {
        if (!strcmp(toEnumChineseString((ModuleDataAttr)index), "Ack")) continue;
        navigationWidget->addItem("GZ248", toEnumChineseString((ModuleDataAttr)index));
    }

    //设置内部控件或子布局器距离四个边的边距
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(navigationWidget);
    mainLayout->addWidget(rightWidget);

    setCentralWidget(mainWidget);

    //JumpWindowToContext(0);
    connect(navigationWidget, &QNavigationWidget::currentItemChanged, this, [=](const int &current){
        SetCurrentIndex(current);
        JumpWindowToContext(current);
    });

    connect(enter, &QPushButton::clicked, this, [=](){
        JumpWindowToContextTransmit(currentIndex);
    });

    return 0;
}

int32_t SubWindow::JumpWindowToServerList(void) {
    ClearWidget();
    subVLayout->addWidget(lineEdit0, 0, Qt::AlignTop | Qt::AlignLeft);
    subVLayout->addWidget(lineEdit1, 0, Qt::AlignTop | Qt::AlignLeft);
    subVLayout->addWidget(lineEdit2, 0, Qt::AlignTop | Qt::AlignLeft);
    subVLayout->addWidget(serverTable, 0, Qt::AlignTop | Qt::AlignLeft);
    subVLayout->addWidget(enter, 0, Qt::AlignTop | Qt::AlignLeft);
    subVLayout->addWidget(cancel, 0, Qt::AlignTop | Qt::AlignLeft);
    subVLayout->addWidget(lineEdit3, 0, Qt::AlignTop | Qt::AlignLeft);
    subVLayout->addWidget(allCheck);
    mainLayout->addLayout(subVLayout);

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

    subVLayout->removeWidget(lineEdit0);
    subVLayout->removeWidget(lineEdit1);
    subVLayout->removeWidget(lineEdit2);
    subVLayout->removeWidget(lineEdit3);
    subVLayout->removeWidget(enter);
    subVLayout->removeWidget(cancel);
    subVLayout->removeWidget(allCheck);
    mainLayout->removeItem(subVLayout);

    //添加客户端列表成员
    connect(enter, &QPushButton::clicked, this, [=](){
        QString userString = lineEdit0->text();
        QString passwordString = lineEdit1->text();
        QString netString = lineEdit2->text();
        int rowCount = serverTable->rowCount();
        serverTable->insertRow(rowCount);
        QTableWidgetItem *item = new QTableWidgetItem(userString);
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
        if (col == 3) {//只有第3列需要有点击响应
            QTableWidgetItem *item = serverTable->item(row, col);
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
            serverTable = new QTableWidget(this);
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
    serverTable = new QTableWidget();
    itemTable = new QTableWidget();
    allCheck = new QCheckBox("全选");
    //palette = new QPalette();
    //QPixmap pixmap(QString(":/background.png"));
    //palette->setBrush(QPalette::Background, QBrush(pixmap));
    //*palettes = this->palette();
    //*color = palettes->color(this->backgroundRole());
    //跳转到登陆界面
    JumpWindowToLogin();
}

SubWindow::~SubWindow()
{
}

