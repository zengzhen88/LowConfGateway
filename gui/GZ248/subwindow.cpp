#include "subwindow.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <stdio.h>
#include "common.h"
#include "httpserver.h"
#include <QRegExp>
#include <QRegExpValidator>
#include <QCheckBox>
#include <QComboBox>
#include <QPainter>
#include <QScreen>
#include <QPainterPath>
#include <QMap>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProgressDialog>
#include <QMessageBox>
#include <QThread>
#include <QHostInfo>
#include <QHostAddress>
#include <QNetworkInterface>
#include <httpserver.h>
#include <QApplication>
#include <QDateTime>

const char *PowerMode2Name[] = {
    "DC",
    "BAT"
};

const char *NetState2Name[] = {
    "CONNSER",
    "CONNET",
    "UNNET",
};

const char *SubWindow2Name[] = {
    "Login",
    "ServerList",
    "MainContext",
    "Context",
};

//keyPressEvent

void SubWindow::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
}

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
        //JumpWindowToMainContext();
        break;
    }
    case SubWindowType_Context:
    {
        //JumpWindowToContext(currentIndex);
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
}

int32_t SubWindow::ClearQnavigationWidget(void) {
    return 0;
}

void SubWindow::ClearWidget(void) {
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
    if (clickConfig)
        clickConfig->sTreeView->hide();
    serverTable->hide();
    contextTable->hide();
    enter->disconnect();
    cancel->disconnect();
    search->disconnect();
    allCheck->disconnect();
}

int32_t SubWindow::SetCurrentIndex(int32_t number) {
    currentIndex = number;
    return 0;
}

int32_t SubWindow::JumpWindowToContextTransmit(int32_t number, sConfigTogether *config, const QString &topics) {
    switch (number) {
    case ModuleDataAttr_GetTemperature:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_GetTemperature));
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        printf ("topic:%s\n", topic.name().toStdString().c_str());
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
        }

        QProgressDialog dialog(tr("获取温度中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 200000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QString retval((QString)("获取温度:") + (QString::number(config->sTemperature)));
        QMessageBox::information(this,
                                 tr("系统提示"),
                                 tr(config->signalSync == SignalSync_OK ? retval.toStdString().c_str() : "获取温度失败"),
                                 QMessageBox::Ok);

        config->signalSync = SignalSync_Init;
        break;
    }
    case ModuleDataAttr_GetModuleVersion:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_GetModuleVersion));
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
        }

        QProgressDialog dialog(tr("获取版本信息中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 5000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QString retval((QString)("获取版本信息:") + (QString)config->sVersion);
        QMessageBox::information(this,
                                 tr("系统提示"),
                                 tr(config->signalSync == SignalSync_OK ? retval.toStdString().c_str() : "获取版本信息失败"),
                                 QMessageBox::Ok);

        config->signalSync = SignalSync_Init;
        break;
    }
    case ModuleDataAttr_GetModuleInfo:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_GetModuleInfo));
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        printf ("topic:%s\n", topic.name().toStdString().c_str());
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
        }

        QProgressDialog dialog(tr("获取模块信息中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 200000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QString retval((QString)("获取模块信息:") + (QString)config->sInfo);
        QMessageBox::information(this,
                                 tr("系统提示"),
                                 tr(config->signalSync == SignalSync_OK ? retval.toStdString().c_str() : "获取模块信息失败"),
                                 QMessageBox::Ok);

        config->signalSync = SignalSync_Init;
        break;
    }
    case ModuleDataAttr_SetModuleInfo:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_SetModuleInfo));
        likeObject.insert("info", lineEdit0->text());
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        printf ("topic:%s\n", topic.name().toStdString().c_str());
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
        }
        QProgressDialog dialog(tr("设置模块信息中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 200000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QMessageBox::information(this, tr("系统提示"), tr(config->signalSync == SignalSync_OK ? "设置成功" : "设置失败"), QMessageBox::Ok);
        config->signalSync = SignalSync_Init;
        break;
    }
    case ModuleDataAttr_GetPower:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_GetPower));
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        printf ("topic:%s\n", topic.name().toStdString().c_str());
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
        }

        QProgressDialog dialog(tr("获取供电模式信息中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 200000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QString retval((QString)("获取供电模式<mode:") + (QString)(config->sMode) + "><level:" + QString::number(config->sLevel) + ">");
        printf (".,...:%s\n", retval.toStdString().c_str());
        QMessageBox::information(this,
                                 tr("系统提示"),
                                 tr(config->signalSync == SignalSync_OK ? retval.toStdString().c_str() : "获取供电模式信息失败"),
                                 QMessageBox::Ok);

        config->signalSync = SignalSync_Init;
        break;
    }
    case ModuleDataAttr_Reboot:
    {
        break;
    }
    case ModuleDataAttr_NetState:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_NetState));
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        printf ("topic:%s\n", topic.name().toStdString().c_str());
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
        }

        QProgressDialog dialog(tr("获取服务器连接状态中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 200000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QString retval((QString)("服务连接状态:") + (QString(NetState2Name[config->sNetState])));
        QMessageBox::information(this,
                                 tr("系统提示"),
                                 tr(config->signalSync == SignalSync_OK ? retval.toStdString().c_str() : "服务器连接失败"),
                                 QMessageBox::Ok);

        config->signalSync = SignalSync_Init;
        break;
    }
    case ModuleDataAttr_GetEthCfg:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_GetEthCfg));
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        printf ("topic:%s\n", topic.name().toStdString().c_str());
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
        }

        QProgressDialog dialog(tr("获取有线网络配置中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 200000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QString retval((QString)("<有线网络配置>\n") + "ip:" + config->sEthAddress + "\n" + "netmask:" + config->sEthNetmask + "\n" + "gateway:" + config->sEthGateway + "\n");
        QMessageBox::information(this,
                                 tr("系统提示"),
                                 tr(config->signalSync == SignalSync_OK ? retval.toStdString().c_str() : "获取有线网络配置失败"),
                                 QMessageBox::Ok);

        config->signalSync = SignalSync_Init;
        break;
    }
    case ModuleDataAttr_SetEthCfg:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_SetEthCfg));
        likeObject.insert("address", lineEdit0->text());
        likeObject.insert("netmask", lineEdit1->text());
        likeObject.insert("gateway", lineEdit2->text());
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        printf ("topic:%s\n", topic.name().toStdString().c_str());
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
        }

        QProgressDialog dialog(tr("设置有线网络配置中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 200000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QMessageBox::information(this,
                                 tr("系统提示"),
                                 tr(config->signalSync == SignalSync_OK ? "设置有线网络配置成功" : "设置有线网络配置失败"),
                                 QMessageBox::Ok);

        config->signalSync = SignalSync_Init;
        break;
    }
    case ModuleDataAttr_GetWifiCfg:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_GetWifiCfg));
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        printf ("topic:%s\n", topic.name().toStdString().c_str());
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
        }

        QProgressDialog dialog(tr("获取无线网络配置中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 200000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QString retval((QString)("<无线网络配置>\n") + "\n" + "user:" + config->sWifiSsid + "\n" + "passwd:" + config->sWifiPassword + "\n" + "ip:" + config->sWifiAddress + "\n" + "netmask:" + config->sWifiNetmask + "\n" + "gateway:" + config->sWifiGateway + "\n");
        QMessageBox::information(this,
                                 tr("系统提示"),
                                 tr(config->signalSync == SignalSync_OK ? retval.toStdString().c_str() : "获取无线网络配置失败"),
                                 QMessageBox::Ok);

        config->signalSync = SignalSync_Init;
        break;
    }
    case ModuleDataAttr_SetWifiCfg:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_SetWifiCfg));
        likeObject.insert("ssid", lineEdit0->text());
        likeObject.insert("password", lineEdit1->text());
        likeObject.insert("address", lineEdit2->text());
        likeObject.insert("netmask", lineEdit3->text());
        likeObject.insert("gateway", lineEdit4->text());
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        printf ("topic:%s\n", topic.name().toStdString().c_str());
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
        }

        QProgressDialog dialog(tr("设置无线网络配置中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 200000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QMessageBox::information(this,
                                 tr("系统提示"),
                                 tr(config->signalSync == SignalSync_OK ? "设置无线网络配置成功" : "获取无线网络配置失败"),
                                 QMessageBox::Ok);

        config->signalSync = SignalSync_Init;
        break;
    }
    case ModuleDataAttr_GetMqttCfg:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_GetMqttCfg));
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        printf ("topic:%s\n", topic.name().toStdString().c_str());
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
        }

        QProgressDialog dialog(tr("获取MQTT网络配置中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 200000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QString retval((QString)("<MQTT网络配置>\n") + "\n" + "user:" + config->sMqttUser + "\n" + "passwd:" + config->sMqttPassword + "\n" + "url:" + config->sMqttUrl + "\n");
        QMessageBox::information(this,
                                 tr("系统提示"),
                                 tr(config->signalSync == SignalSync_OK ? retval.toStdString().c_str() : "获取MQTT网络配置失败"),
                                 QMessageBox::Ok);

        config->signalSync = SignalSync_Init;
        break;
    }
    case ModuleDataAttr_SetMqttCfg:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_SetMqttCfg));
        likeObject.insert("user", lineEdit0->text());
        likeObject.insert("password", lineEdit1->text());
        likeObject.insert("url", lineEdit2->text());
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        printf ("topic:%s\n", topic.name().toStdString().c_str());
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
                //sNetState = (NetState)root.value("NetState").toInt();
        }

        QProgressDialog dialog(tr("设置MQTT网络配置中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 200000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QMessageBox::information(this,
                                 tr("系统提示"),
                                 tr(config->signalSync == SignalSync_OK ? "设置MOTT网络配置成功" : "获取MQTT网络配置失败"),
                                 QMessageBox::Ok);

        config->signalSync = SignalSync_Init;
        break;
    }
    case ModuleDataAttr_Update:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_Update));
        likeObject.insert("url", config->updateFileString); //copy update file
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        printf ("topic:%s\n", topic.name().toStdString().c_str());
        printf ("url:%s\n", (config->updateFileString).toStdString().c_str());
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
                //sNetState = (NetState)root.value("NetState").toInt();
        }

        QProgressDialog dialog(tr("系统升级中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 200000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QMessageBox::information(this,
                                 tr("系统提示"),
                                 tr(config->signalSync == SignalSync_OK ? "系统升级成功" : "系统升级失败"),
                                 QMessageBox::Ok);

        config->signalSync = SignalSync_Init;
        break;
    }
    case ModuleDataAttr_ReportData:
    {
        break;
    }
    case ModuleDataAttr_GetScanTimeout:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_GetScanTimeout));
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        printf ("topic:%s\n", topic.name().toStdString().c_str());
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
        }

        QProgressDialog dialog(tr("获取下发数据超时时间中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 200000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QString retval((QString)("获取下发数据超时时间:") + (QString)config->sScanTimeout);
        QMessageBox::information(this,
                                 tr("系统提示"),
                                 tr(config->signalSync == SignalSync_OK ? retval.toStdString().c_str() : "获取下发数据超时时间失败"),
                                 QMessageBox::Ok);

        config->signalSync = SignalSync_Init;
        break;
    }
    case ModuleDataAttr_SetScanTimeout:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_SetScanTimeout));
        likeObject.insert("sec", lineEdit0->text());
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        printf ("topic:%s\n", topic.name().toStdString().c_str());
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
        }
        QProgressDialog dialog(tr("设置下发数据超时时间中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 200000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QMessageBox::information(this, tr("系统提示"), tr(config->signalSync == SignalSync_OK ? "设置成功" : "设置失败"), QMessageBox::Ok);
        config->signalSync = SignalSync_Init;
        break;
    }
    case ModuleDataAttr_PtSend:
    {
        QJsonObject likeObject;
        likeObject.insert("htype", toEnumString(ModuleDataAttr_PtSend));
        likeObject.insert("mac", lineEdit0->text());
        likeObject.insert("seq", config->seq++);
        likeObject.insert("data", lineEdit1->text());
        QJsonArray likeArray;
        likeArray.append(likeObject);
        QJsonDocument doc;
        doc.setArray(likeArray);
        QMqttTopicName topic;
        topic.setName(topics);
        printf ("topic:%s\n", topic.name().toStdString().c_str());
        config->client->publish(topic, doc.toJson(), 0, 0);
        QJsonObject object;
        QJsonDocument doc1 = QJsonDocument::fromJson(doc.toJson());
        QJsonArray roots = doc1.array();
        for (int32_t index = 0; index < roots.size(); index++) {
            QJsonObject root = roots.at(index).toObject();
            QString name = root.value("htype").toString();
        }

        QProgressDialog dialog(tr("设置下发数据中..."), tr("取消"), 0, 200000, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 200000; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                config->signalSync = SignalSync_FAILURE;
                break;
            }
            if (config->signalSync) break;
            QThread::usleep(10);
        }
        QMessageBox::information(this,
                                 tr("系统提示"),
                                 tr(config->signalSync == SignalSync_OK ? "设置下发数据成功" : "设置下发数据失败"),
                                 QMessageBox::Ok);

        config->signalSync = SignalSync_Init;
        break;
    }
    default:break;
    }
    return 0;
}

#define COMMON_WIDTH (200)
#define COMMON_HEIGHT (40)

int32_t SubWindow::JumpWindowToContext(QString &match, sConfigTogether *config, const QString &topic) {
    windowType = SubWindowType_Context;
    int number = toChineseStringEnum(match.toStdString().c_str());
    printf ("num:%d mach:%s\n", number, match.toStdString().c_str());
    switch (number) {
    case ModuleDataAttr_GetTemperature:
    {        
        ClearWidget();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(enter, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("获取温度");
        enter->show();
        contextTable->show();
        cancel->setText("返回");
        cancel->show();
        enter->setFocus();
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t labelX = 2, labelY = 0, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        enter->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        contextTable->selectRow(currentIndex);

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex, config, topic);
        });

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
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(enter, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("获取模块版本");
        enter->show();
        cancel->setText("返回");
        contextTable->show();
        cancel->show();

        enter->setFocus();
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t labelX = 2, labelY = 0, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        enter->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        contextTable->selectRow(currentIndex);

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex, config, topic);
        });
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
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(enter, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("获取模块信息");
        enter->show();
        contextTable->show();
        cancel->setText("返回");
        cancel->show();

        enter->setFocus();
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t labelX = 2, labelY = 0, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        enter->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        contextTable->selectRow(currentIndex);

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex, config, topic);
        });

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
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(lineEdit0, 0, Qt::AlignTop);
        rightLayout->addWidget(enter, 0, Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("确定");
        lineEdit0->setPlaceholderText("请输入模块版本:");
        lineEdit0->clear();
        lineEdit0->show();
        enter->show();
        contextTable->show();
        cancel->setText("返回");
        cancel->show();
        lineEdit0->setFocus(); //强制tab焦点
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(lineEdit0);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t labelX = 2, labelY = 0, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
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
            JumpWindowToContextTransmit(currentIndex, config, topic);
        });
        break;
    }
    case ModuleDataAttr_GetPower:
    {
        PowerSupplyMode mode;
        int level;
        ClearWidget();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(enter, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("获取供电模式");
        enter->show();
        contextTable->show();
        cancel->setText("返回");
        cancel->show();

        enter->setFocus();
        mainLayout->removeWidget(contextTable);
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(rightWidget);

        int32_t labelX = 2, labelY = 0, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        enter->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex, config, topic);
        });

        break;
    }
    case ModuleDataAttr_Reboot:
    {
        ClearWidget();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(enter, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("重启");
        enter->show();
        cancel->setText("返回");
        contextTable->show();
        cancel->show();

        enter->setFocus();
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t labelX = 2, labelY = 0, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        enter->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex, config, topic);
        });
        break;
    }
    case ModuleDataAttr_NetState:
    {
        ClearWidget();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(enter, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("获取网络状态");
        enter->show();
        cancel->setText("返回");
        contextTable->show();
        cancel->show();

        enter->setFocus();
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(rightWidget);
        mainLayout->removeWidget(contextTable);

        int32_t labelX = 2, labelY = 0, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        enter->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex, config, topic);
        });

        break;
    }
    case ModuleDataAttr_GetEthCfg:
    {
        ClearWidget();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(enter, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(cancel, 0, Qt::AlignTop | Qt::AlignLeft);
        mainLayout->addWidget(rightWidget);
        enter->setText("获取有线配置");
        enter->show();
        cancel->setText("返回");
        cancel->show();
        contextTable->show();

        enter->setFocus();
        rightLayout->removeWidget(enter);
        rightLayout->removeWidget(cancel);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t labelX = 2, labelY = 0, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        enter->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex, config, topic);
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

        lineEdit0->setFocus(); //强制tab焦点
        rightLayout->removeWidget(label);
        rightLayout->removeWidget(lineEdit0);
        rightLayout->removeWidget(lineEdit1);
        rightLayout->removeWidget(lineEdit2);
        rightLayout->removeWidget(enter);
        rightLayout->removeWidget(cancel);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t left, right, top, bottom;
        //rightLayout->setContentsMargins(2, 2, 2, 2);
        rightLayout->getContentsMargins(&left, &top, &right, &bottom);
        int32_t labelX = left + 2, labelY = top + 0, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        label->setGeometry(labelX, labelY, labelW, labelH);
        int32_t lineEdit0X = labelX, lineEdit0Y = labelY + labelH, lineEdit0W = COMMON_WIDTH, lineEdit0H = COMMON_HEIGHT;
        lineEdit0->setGeometry(lineEdit0X, lineEdit0Y, lineEdit0W, lineEdit0H);
        int32_t lineEdit1X = labelX, lineEdit1Y = lineEdit0Y + lineEdit0H, lineEdit1W = COMMON_WIDTH, lineEdit1H = COMMON_HEIGHT;
        lineEdit1->setGeometry(lineEdit1X, lineEdit1Y, lineEdit1W, lineEdit1H);
        int32_t lineEdit2X = labelX, lineEdit2Y = lineEdit1Y + lineEdit1H, lineEdit2W = COMMON_WIDTH, lineEdit2H = COMMON_HEIGHT;
        lineEdit2->setGeometry(lineEdit2X, lineEdit2Y, lineEdit2W, lineEdit2H);
        int32_t enterX = labelX, enterY = lineEdit2Y + lineEdit1H, enterW = 40, enterH = COMMON_HEIGHT;
        enter->setGeometry(enterX, enterY, enterW, enterH);

        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex, config, topic);
        });
        break;
    }
    case ModuleDataAttr_GetWifiCfg:
    {
        ClearWidget();
        //navigationWidget->show();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(enter, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("获取无线网络配置");
        enter->show();
        contextTable->show();

        enter->setFocus();
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        cancel->setText("返回");
        cancel->show();
        int32_t labelX = 2, labelY = 0, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        enter->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex, config, topic);
        });
        break;
    }
    case ModuleDataAttr_SetWifiCfg:
    {
        ClearWidget();
        rightLayout->addWidget(label, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit0, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit1, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit2, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit3, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit4, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(enter, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(contextTable);
        mainLayout->addWidget(rightWidget);
        enter->setText("确定");
        label->setText("请输入无线配置:");
        lineEdit0->setPlaceholderText("无线账号");
        lineEdit1->setPlaceholderText("无线密码");
        lineEdit2->setPlaceholderText("网络地址");
        lineEdit3->setPlaceholderText("子网掩码");
        lineEdit4->setPlaceholderText("网关地址");

        //rightWidget->setFocus();
        //contextTable->eventFilter()
        //focusNextChild();
        lineEdit0->setFocus(); //强制tab焦点
        setTabOrder(lineEdit0, lineEdit1);
        setTabOrder(lineEdit1, lineEdit2);
        setTabOrder(lineEdit2, lineEdit3);
        setTabOrder(lineEdit3, lineEdit4);
        setTabOrder(lineEdit4, enter);
        setTabOrder(enter, cancel);
        setTabOrder(cancel, lineEdit0);
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
        int32_t labelX = left + 2, labelY = top + 2, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        label->setGeometry(labelX, labelY, labelW, labelH);
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
        cancel->setGeometry(returnX, returnY, returnW, returnH);


        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex, config, topic);
        });
        break;
    }
    case ModuleDataAttr_GetMqttCfg:
    {
        ClearWidget();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(enter, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("获取MQTT配置");
        enter->show();
        contextTable->show();

        enter->setFocus();
        rightLayout->removeWidget(enter);
        rightLayout->removeWidget(cancel);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        cancel->setText("返回");
        cancel->show();
        int32_t labelX = 2, labelY = 0, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        enter->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;;
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex, config, topic);
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

        lineEdit0->setFocus(); //强制tab焦点
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);
        rightLayout->removeWidget(lineEdit0);
        rightLayout->removeWidget(lineEdit1);
        rightLayout->removeWidget(lineEdit2);
        mainLayout->removeWidget(contextTable);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(rightWidget);


        int32_t left, right, top, bottom;
        //rightLayout->setContentsMargins(2, 2, 2, 2);
        rightLayout->getContentsMargins(&left, &top, &right, &bottom);
        int32_t labelX = left + 2, labelY = top + 0, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        label->setGeometry(labelX, labelY, labelW, labelH);
        int32_t lineEdit0X = labelX, lineEdit0Y = labelY + labelH + 0, lineEdit0W = COMMON_WIDTH, lineEdit0H = COMMON_HEIGHT;
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
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex, config, topic);
        });
        break;
    }
    case ModuleDataAttr_Update:
    {
        ClearWidget();
        mainLayout->addWidget(contextTable);
        mainLayout->addWidget(rightWidget);
        contextTable->show();
#if 0
        sFileSystemMode = new QFileSystemModel();
        sTreeView = new QTreeView(this);
        sTreeView->setModel(sFileSystemMode);
        sTreeView->setIconSize(QSize(16, 16));
        sFileSystemMode->setRootPath("/");
        QModelIndex rootIndex = sFileSystemMode->index("/");
        sTreeView->expand(rootIndex);
#endif
        rightLayout->addWidget(config->sTreeView);
        config->sTreeView->show();

        connect(config->sTreeView, &QTreeView::doubleClicked, this, [=](const QModelIndex &index) {
            //printf ("Item doubleClicked<<<<<<<<<<<<<<<\n");
            if (!index.isValid())
                return;

            QString ip = this->getLocalIP();
            //QString configFileName=":/webapp.ini";
            //QSettings* listenerSettings=new QSettings(configFileName, QSettings::IniFormat, this);
            //listenerSettings->beginGroup("listener");
            //quint16 port=listenerSettings->value("port").toUInt() & 0xFFFF;
            //printf (".......port:%d\n", port);

            QFileInfo files = config->sFileSystemMode->fileInfo(index);
            config->updateFileString = (QString)("http://") + ip + (QString)(":") + QString::number(8080) + (QString)("/") + files.fileName();
            //updateFileString = (QString)("http://192.168.0.107:8080/whttpserver/downloadFile/hello_world.bin");
            QString pathString = files.absolutePath();
            printf ("#file:%s dir:%s\n", config->updateFileString.toStdString().c_str(), pathString.toStdString().c_str());


            if (strstr(config->updateFileString.toStdString().c_str(), ".bin")) {
                JumpWindowToContextTransmit(currentIndex, config, topic);
            }
        });

        rightLayout->removeWidget(config->sTreeView);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;;
        cancel->setGeometry(returnX, returnY, returnW, returnH);
        int32_t labelX = 2, labelY = 0, labelW = rightWidget->width(), labelH = rightWidget->height() - labelY - returnH;
        config->sTreeView->setGeometry(labelX, labelY, labelW, labelH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });
        break;
    }
    case ModuleDataAttr_ReportData:
    {
        break;
    }
    case ModuleDataAttr_PtSend:
    {
        ClearWidget();
        //navigationWidget->show();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(label, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit0, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(lineEdit1, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(enter, 0, Qt::AlignTop | Qt::AlignLeft);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("确定");
        label->setText("请输入需要下发的数据:");
        lineEdit0->setPlaceholderText("MAC地址");
        lineEdit1->setPlaceholderText("有效数据");
        lineEdit0->clear();
        lineEdit1->clear();
        label->show();
        lineEdit0->show();
        lineEdit1->show();
        contextTable->show();
        enter->show();

        lineEdit0->setFocus(); //强制tab焦点
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(label);
        rightLayout->removeWidget(lineEdit0);
        rightLayout->removeWidget(lineEdit1);
        mainLayout->removeWidget(contextTable);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(rightWidget);


        int32_t left, right, top, bottom;
        //rightLayout->setContentsMargins(2, 2, 2, 2);
        rightLayout->getContentsMargins(&left, &top, &right, &bottom);
        int32_t labelX = left + 2, labelY = top + 0, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        label->setGeometry(labelX, labelY, labelW, labelH);
        int32_t lineEdit0X = labelX, lineEdit0Y = labelY + labelH + 0, lineEdit0W = COMMON_WIDTH, lineEdit0H = COMMON_HEIGHT;
        lineEdit0->setGeometry(lineEdit0X, lineEdit0Y, lineEdit0W, lineEdit0H);
        int32_t lineEdit1X = labelX, lineEdit1Y = lineEdit0Y + lineEdit0H, lineEdit1W = COMMON_WIDTH, lineEdit1H = COMMON_HEIGHT;
        lineEdit1->setGeometry(lineEdit1X, lineEdit1Y, lineEdit1W, lineEdit1H);
        int32_t enterX = lineEdit1X, enterY = lineEdit1Y + lineEdit1H, enterW = 40, enterH = COMMON_HEIGHT;
        enter->setGeometry(enterX, enterY, enterW, enterH);

        cancel->setText("返回");
        cancel->show();
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex, config, topic);
        });
        break;
    }
    case ModuleDataAttr_SetScanTimeout:
    {
        ClearWidget();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(lineEdit0, 0, Qt::AlignTop);
        rightLayout->addWidget(enter, 0, Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("确定");
        lineEdit0->setPlaceholderText("请输入下发数据超时时间:");
        lineEdit0->clear();
        lineEdit0->show();
        enter->show();
        contextTable->show();
        cancel->setText("返回");
        cancel->show();
        lineEdit0->setFocus(); //强制tab焦点
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(lineEdit0);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t labelX = 2, labelY = 0, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
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
            JumpWindowToContextTransmit(currentIndex, config, topic);
        });
        break;
    }
    case ModuleDataAttr_GetScanTimeout:
    {
        ClearWidget();
        mainLayout->addWidget(contextTable);
        rightLayout->addWidget(enter, 0, Qt::AlignLeft | Qt::AlignTop);
        rightLayout->addWidget(cancel, 0, Qt::AlignLeft | Qt::AlignTop);
        mainLayout->addWidget(rightWidget);
        enter->setText("获取下发数据超时时间");
        enter->show();
        contextTable->show();
        cancel->setText("返回");
        cancel->show();

        enter->setFocus();
        rightLayout->removeWidget(cancel);
        rightLayout->removeWidget(enter);
        mainLayout->removeWidget(contextTable);
        mainLayout->removeWidget(rightWidget);

        int32_t labelX = 2, labelY = 0, labelW = COMMON_WIDTH, labelH = COMMON_HEIGHT;
        enter->setGeometry(labelX, labelY, labelW, labelH);
        int32_t returnX = rightWidget->width() - 100, returnY = rightWidget->height() - 40, returnW = 80, returnH = COMMON_HEIGHT;
        cancel->setGeometry(returnX, returnY, returnW, returnH);

        contextTable->selectRow(currentIndex);

        connect(enter, &QPushButton::clicked, this, [=](){
            JumpWindowToContextTransmit(currentIndex, config, topic);
        });

        //返回到客户端列表成员
        connect(cancel, &QPushButton::clicked, this, [=](){
            ClearQnavigationWidget();
            JumpWindowToServerList();
        });
        break;
    }
    default:break;
    }

    return 0;
}

QString SubWindow::getLocalIP(void)
{
    QString localIP;
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();

    for(auto address : addresses)
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress::Null && address != QHostAddress::LocalHost)
        {// 如果需要获取IPv6则需要将QAbstractSocket::IPv4Protocol换成QAbstractSocket::IPv6Protocol
            localIP = address.toString();
            break;
        }
    }

    return localIP;
}

int32_t SubWindow::RecvMqttMessage(sConfigTogether *config, const QByteArray message, const QMqttTopicName topic) {
    QJsonObject object;
    QJsonDocument doc = QJsonDocument::fromJson(message);
    QJsonArray roots = doc.array();

    for (int32_t index = 0; index < roots.size(); index++) {
        QJsonObject root = roots.at(index).toObject();
        QString name = root.value("htype").toString();
        if (!strcmp(name.toStdString().c_str(), "GetTemperature")) {
            config->sTemperature = root.value("temperature").toDouble();

            printf ("htype:%s\n", name.toStdString().c_str());
            printf ("temperature:%lf\n", config->sTemperature);
            config->signalSync = SignalSync_OK;
        }
        else if (!strcmp(name.toStdString().c_str(), "GetModuleVersion")) {
            config->sVersion = root.value("ver").toString();

            printf ("htype:%s\n", name.toStdString().c_str());
            printf ("version:%s\n", config->sVersion.toStdString().c_str());
            config->signalSync = SignalSync_OK;
        }
        else if (!strcmp(name.toStdString().c_str(), "GetModuleInfo")) {
            config->sInfo = root.value("info").toString();

            printf ("htype:%s\n", name.toStdString().c_str());
            printf ("info:%s\n", config->sInfo.toStdString().c_str());
            config->signalSync = SignalSync_OK;
        }
        else if (!strcmp(name.toStdString().c_str(), "SetModuleInfo")) {
            QString ack = root.value("status").toString();

            printf ("htype:%s\n", name.toStdString().c_str());
            printf ("status:%s\n", ack.toStdString().c_str());

            if (!strcmp(ack.toStdString().c_str(), "OK")) {
               config->signalSync = SignalSync_OK;
            }
            else {
               config->signalSync = SignalSync_FAILURE;
            }
        }
        else if (!strcmp(name.toStdString().c_str(), "GetPower")) {
            config->sMode = root.value("mode").toString();
            config->sLevel = root.value("battery_level").toInt();

            printf ("htype:%s\n", name.toStdString().c_str());
            printf ("mode:%s\n", config->sMode);
            printf ("battery_level:%d\n", config->sLevel);
            config->signalSync = SignalSync_OK;
        }
        else if (!strcmp(name.toStdString().c_str(), "GetWifiCfg")) {
            config->sEthAddress = root.value("address").toString();
            config->sEthNetmask = root.value("netmask").toString();
            config->sEthGateway = root.value("gateway").toString();

            printf ("htype:%s\n", name.toStdString().c_str());
            printf ("sEthAddress:%s\n", config->sEthAddress.toStdString().c_str());
            printf ("sEthNetmask:%s\n", config->sEthNetmask.toStdString().c_str());
            printf ("sEthGateway:%s\n", config->sEthGateway.toStdString().c_str());
            config->signalSync = SignalSync_OK;
        }
        else if (!strcmp(name.toStdString().c_str(), "SetWifiCfg")) {
            QString ack = root.value("status").toString();

            printf ("htype:%s\n", name.toStdString().c_str());
            printf ("status:%s\n", ack.toStdString().c_str());

            if (!strcmp(ack.toStdString().c_str(), "OK")) {
                config->signalSync = SignalSync_OK;
            }
            else {
               config->signalSync = SignalSync_FAILURE;
            }
        }
        else if (!strcmp(name.toStdString().c_str(), "GetEthCfg")) {
            config->sWifiSsid = root.value("ssid").toString();
            config->sWifiPassword = root.value("password").toString();
            config->sWifiAddress = root.value("address").toString();
            config->sWifiNetmask = root.value("netmask").toString();
            config->sWifiGateway = root.value("gateway").toString();

            printf ("htype:%s\n", name.toStdString().c_str());
            printf ("sWifiSsid:%s\n", config->sWifiSsid.toStdString().c_str());
            printf ("sWifiPassword:%s\n", config->sWifiPassword.toStdString().c_str());
            printf ("sWifiAddress:%s\n", config->sWifiAddress.toStdString().c_str());
            printf ("sWifiNetmask:%s\n", config->sWifiNetmask.toStdString().c_str());
            printf ("sWifiGateway:%s\n", config->sWifiGateway.toStdString().c_str());
            config->signalSync = SignalSync_OK;
        }
        else if (!strcmp(name.toStdString().c_str(), "SetEthCfg")) {
            QString ack = root.value("status").toString();

            printf ("htype:%s\n", name.toStdString().c_str());
            printf ("status:%s\n", ack.toStdString().c_str());

            if (!strcmp(ack.toStdString().c_str(), "OK")) {
                config->signalSync = SignalSync_OK;
            }
            else {
               config->signalSync = SignalSync_FAILURE;
            }
        }
        else if (!strcmp(name.toStdString().c_str(), "GetMqttCfg")) {
            config->sMqttUser = root.value("user").toString();
            config->sMqttPassword = root.value("password").toString();
            config->sMqttUrl = root.value("url").toString();

            printf ("htype:%s\n", name.toStdString().c_str());
            printf ("sMqttUser:%s\n", config->sMqttUser.toStdString().c_str());
            printf ("sMqttPassword:%s\n", config->sMqttPassword.toStdString().c_str());
            printf ("sMqttUrl:%s\n", config->sMqttUrl.toStdString().c_str());
            config->signalSync = SignalSync_OK;
        }
        else if (!strcmp(name.toStdString().c_str(), "SetMqttCfg")) {
            QString ack = root.value("status").toString();

            printf ("htype:%s\n", name.toStdString().c_str());
            printf ("status:%s\n", ack.toStdString().c_str());

            if (!strcmp(ack.toStdString().c_str(), "OK")) {
                config->signalSync = SignalSync_OK;
            }
            else {
                config->signalSync = SignalSync_FAILURE;
            }
        }
        else if (!strcmp(name.toStdString().c_str(), "GetScanTimeout")) {
            config->sScanTimeout = root.value("sec").toInt();

            printf ("htype:%s\n", name.toStdString().c_str());
            printf ("sScanTimeout:%d\n", config->sScanTimeout);
            config->signalSync = SignalSync_OK;
        }
        else if (!strcmp(name.toStdString().c_str(), "SetScanTimeout")) {
            QString ack = root.value("status").toString();

            printf ("htype:%s\n", name.toStdString().c_str());
            printf ("status:%s\n", ack.toStdString().c_str());

            if (!strcmp(ack.toStdString().c_str(), "OK")) {
                config->signalSync = SignalSync_OK;
            }
            else {
                config->signalSync = SignalSync_FAILURE;
            }
        }
        else if (!strcmp(name.toStdString().c_str(), "PTSend")) {
            QString ack = root.value("status").toString();

            printf ("htype:%s\n", name.toStdString().c_str());
            printf ("status:%s\n", ack.toStdString().c_str());

            if (!strcmp(ack.toStdString().c_str(), "OK")) {
                config->signalSync = SignalSync_OK;
            }
            else {
                config->signalSync = SignalSync_FAILURE;
            }
        }
    }

    return 0;
}

int32_t SubWindow::JumpWindowToMainContext(sConfigTogether *config, const QString &topic) {
    ClearWidget();
    mainLayout->addWidget(contextTable);
    mainLayout->addWidget(rightWidget);
    windowType = SubWindowType_MainContext;

    setCentralWidget(mainWidget);
    contextTable->show();
    mainLayout->removeWidget(rightWidget);
    mainLayout->removeWidget(contextTable);

    //单元格被点击处理
    connect(contextTable, &QTableWidget::cellClicked, this, [=](int32_t row, int32_t col){
        QTableWidgetItem *item = contextTable->item(row, col);
        QString match = item->text();
        printf ("mach:%s\n", match.toStdString().c_str());
        SetCurrentIndex(row);
        //rightWidget->setFocus();
        contextTable->setFocusPolicy(Qt::NoFocus);
        JumpWindowToContext(match, config, topic);
        contextTable->selectRow(row);
        //contextTable->setFocusPolicy(Qt::WheelFocus);
    });

    return 0;
}

int32_t SubWindow::JumpWindowToServerList(void) {
    ClearWidget();
    rightLayout->addWidget(lineEdit0, 0, Qt::AlignTop | Qt::AlignLeft);
    rightLayout->addWidget(lineEdit1, 0, Qt::AlignTop | Qt::AlignLeft);
    rightLayout->addWidget(lineEdit2, 0, Qt::AlignTop | Qt::AlignLeft);
    rightLayout->addWidget(lineEdit4, 0, Qt::AlignTop | Qt::AlignLeft);
    rightLayout->addWidget(lineEdit5, 0, Qt::AlignTop | Qt::AlignLeft);
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
    lineEdit2->setPlaceholderText("主机地址");
    lineEdit4->setPlaceholderText("主机端口");
    lineEdit5->setPlaceholderText("主题字段");
    allCheck->setText("全选");
    allCheck->show();
    lineEdit0->clear();
    lineEdit1->clear();
    lineEdit2->clear();
    lineEdit3->clear();
    lineEdit4->clear();
    lineEdit5->clear();
    lineEdit0->show();
    lineEdit1->show();
    lineEdit2->show();
    lineEdit4->show();
    lineEdit5->show();
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
    int32_t label3X = left + 2, label3Y = label2Y + label2H, label3W = 200, label3H = 20;
    lineEdit4->setGeometry(label3X, label3Y, label3W, label3H);
    int32_t label4X = left + 2, label4Y = label3Y + label3H, label4W = 200, label4H = 20;
    lineEdit5->setGeometry(label4X, label4Y, label4W, label4H);
    int32_t enterX = label4W + 10, enterY = label4Y, enterW = 100, enterH = 20;
    enter->setGeometry(enterX, enterY, enterW, enterH);
    int32_t cancelX = enterX + enterW + 10, cancelY = label4Y, cancelW = 100, cancelH = 20;
    cancel->setGeometry(cancelX, cancelY, cancelW, cancelH);
    int32_t checkX = cancelX + cancelW + 10, checkY = cancelY, checkW = 50, checkH = 20;
    allCheck->setGeometry(checkX, checkY, checkW, checkH);
    int32_t searchH = 20;
    int32_t tableX = left + 2, tableY = cancelY + cancelH, tableW = mainWidget->width() - 10, tableH = mainWidget->height() - labelH - label1H - label2H - label3H - label4H - enterH - searchH;
    int32_t searchX = left + 2, searchY = tableY + tableH, searchW = 200;//, searchH = 20;
    serverTable->setGeometry(tableX, tableY, tableW, tableH);
    lineEdit3->setGeometry(searchX, searchY, searchW, searchH);

    rightLayout->removeWidget(lineEdit0);
    rightLayout->removeWidget(lineEdit1);
    rightLayout->removeWidget(lineEdit2);
    rightLayout->removeWidget(lineEdit3);
    rightLayout->removeWidget(lineEdit4);
    rightLayout->removeWidget(lineEdit5);
    rightLayout->removeWidget(enter);
    rightLayout->removeWidget(cancel);
    rightLayout->removeWidget(allCheck);
    rightLayout->removeWidget(serverTable);
    mainLayout->removeWidget(rightWidget);

    lineEdit0->setFocus(); //强制tab焦点
    setTabOrder(lineEdit0, lineEdit1);
    setTabOrder(lineEdit1, lineEdit2);
    setTabOrder(lineEdit2, lineEdit4);
    setTabOrder(lineEdit4, lineEdit5);
    setTabOrder(lineEdit5, lineEdit3);
    setTabOrder(lineEdit3, enter);
    setTabOrder(enter, cancel);
    setTabOrder(cancel, allCheck);

    //添加客户端列表成员
    connect(enter, &QPushButton::clicked, this, [=](){

        QString userString = lineEdit0->text();      //获取用户名称
        QString passwordString = lineEdit1->text();  //获取用户密码
        QString netString = lineEdit2->text();       //获取用户网址
        QString portString = lineEdit4->text();      //获取用户端口
        QString topic = lineEdit5->text();           //获取主题

        for (int32_t row = 0; row < serverTable->rowCount(); row++) {
            if (!strcmp(serverTable->item(row, 0)->text().toStdString().c_str(), userString.toStdString().c_str())
                    && !strcmp(serverTable->item(row, 1)->text().toStdString().c_str(), passwordString.toStdString().c_str())
                    && !strcmp(serverTable->item(row, 2)->text().toStdString().c_str(), netString.toStdString().c_str())
                    && !strcmp(serverTable->item(row, 3)->text().toStdString().c_str(), portString.toStdString().c_str())) {

                QProgressDialog dialog(tr("添加新客户端中..."), tr("取消"), 0, 2, this);
                dialog.setWindowTitle(tr("进度条对话框"));
                dialog.setWindowModality(Qt::WindowModal);
                dialog.show();
                for (int32_t index = 0; index < 2; index++) {
                    dialog.setValue(index);
                    QCoreApplication::processEvents();
                    if (dialog.wasCanceled()) {
                        break;
                    }
                    //QThread::usleep(10);
                }
                QMessageBox::information(this,
                                         tr("系统提示"),
                                         tr("添加新客户端中失败"),
                                         QMessageBox::Ok);
                break;
            }
        }

        int rowCount = serverTable->rowCount();
        serverTable->insertRow(rowCount);
        QTableWidgetItem *item = new QTableWidgetItem(userString);

        sConfigTogether *config = new sConfigTogether();

        config->topic = topic;
        config->topicack = config->topic + (QString)"Ack";
        printf ("topic:%s topicack:%s\n", config->topic.toStdString().c_str(), config->topicack.toStdString().c_str());

        config->sFileSystemMode = new QFileSystemModel();
        config->sTreeView = new QTreeView(this);
        config->sTreeView->setModel(config->sFileSystemMode);
        config->sTreeView->setIconSize(QSize(16, 16));
#if defined(_WIN32)
        config->sFileSystemMode->setRootPath("C:\\");
        QModelIndex rootIndex = config->sFileSystemMode->index("C:\\");
#else
        config->sFileSystemMode->setRootPath("/");
        QModelIndex rootIndex = config->sFileSystemMode->index("/");
#endif
        config->sTreeView->expand(rootIndex);

#ifdef QTMQTT_
        config->client = new QMqttClient(this);
        config->client->setHostname(netString);
        config->client->setPort(portString.toInt());
        config->client->setUsername(userString);
        config->client->setPassword(passwordString);
        qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
        config->client->setClientId(QString::number(timestamp, 16));//用当前时间做clientId

        serverTableMap.insert(item, config);
        topicTableMap.insert(config, config->topicack);
        connect(config->client, &QMqttClient::stateChanged, this, [=](QMqttClient::ClientState state){
            printf ("stateChanged:%d config->client->state:%d\n", state, config->client->state());
        });

        connect(config->client, &QMqttClient::disconnected, this, [=](){
            printf ("config->client->disconnect\n");
        });
#if 0
        int a = 0;
        connect(config->client, &QMqttClient::connected, [&a](){
            printf ("config->client->connect:%d\n", a);
        });
#else
        connect(config->client, &QMqttClient::connected, this, [=](){
            QMqttTopicFilter topics;
            topics.setFilter(config->topicack);
            printf ("config->client->connect(%s)\n", topics.filter().toStdString().c_str());
            config->client->subscribe(topics, 0);
        });
#endif
        connect(config->client, &QMqttClient::messageReceived, this, [=](const QByteArray &message, const QMqttTopicName &topic) {
            sConfigTogether *sConfig = (sConfigTogether *)topicTableMap.key(topic.name());
            if (sConfig) {
                RecvMqttMessage(sConfig, message, topic);
            }
        });
        connect(config->client, &QMqttClient::pingResponseReceived, this, [=](){
            printf ("config->client->ping server trigger\n");
        });
        config->client->setKeepAlive(60);
        config->client->connectToHost();
#endif
        serverTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        serverTable->setItem(rowCount, TableItem_0, item);
        item->setCheckState(Qt::Unchecked);
        item = new QTableWidgetItem(passwordString);
        serverTable->setItem(rowCount, TableItem_1, item);
        item = new QTableWidgetItem(netString);
        serverTable->setItem(rowCount, TableItem_2, item);
        item = new QTableWidgetItem(portString);
        serverTable->setItem(rowCount, TableItem_3, item);
        item = new QTableWidgetItem(topic);
        serverTable->setItem(rowCount, TableItem_4, item);
        item = new QTableWidgetItem(QString("进入"));
        serverTable->setItem(rowCount, TableItem_5, item);

        QProgressDialog dialog(tr("添加新客户端中..."), tr("取消"), 0, 2, this);
        dialog.setWindowTitle(tr("进度条对话框"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for (int32_t index = 0; index < 2; index++) {
            dialog.setValue(index);
            QCoreApplication::processEvents();
            if (dialog.wasCanceled()) {
                break;
            }
            //QThread::usleep(10);
        }
        QMessageBox::information(this,
                                 tr("系统提示"),
                                 tr("添加新客户端中成功"),
                                 QMessageBox::Ok);
    });
    //删除客户端列表成员
    connect(cancel, &QPushButton::clicked, this, [=](){
        int32_t sRow = serverTable->rowCount();
        for (int32_t row = sRow - 1; row >= 0; row--) {
            //获取勾选框
            QTableWidgetItem *item = serverTable->item(row, 0);
            if (item->checkState() == Qt::Checked) {
                serverTable->removeRow(row);
                sConfigTogether *sConfig = (sConfigTogether *)serverTableMap.value(item);
                //QMqttClient *client = (QMqttClient *)serverTableMap.value(item);
                if (sConfig && sConfig->client) {
                    serverTableMap.remove(item);
                    topicTableMap.remove(sConfig);
                    sConfig->client->disconnect();
                    delete sConfig->client;
                    delete sConfig->sTreeView;
                    delete sConfig->sFileSystemMode;
                    delete sConfig;
                    printf ("delete QMqttClient\n");
                }
            }
        }
        if (serverTable->rowCount() == TableItem_0) {
            //全选后恢复至不全选
            if (allCheck->checkState() == Qt::Checked) {
                allCheck->setCheckState(Qt::Unchecked);
            }
        }
    });
    //复选框状态变化
    connect(allCheck, &QCheckBox::stateChanged, this, [=](int32_t state){
        int32_t sRow = serverTable->rowCount();
        for (int32_t row = sRow - 1; row >= TableItem_0; row--) {
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
        if (col == TableItem_5) {//只有第3列需要有点击响应
            QTableWidgetItem *item = serverTable->item(row, TableItem_0);
            sConfigTogether *sConfig = (sConfigTogether *)serverTableMap.value(item);
            //QMqttClient *client = (QMqttClient *)serverTableMap.value(item);
            item = serverTable->item(row, TableItem_4); //获取topic
            const QString topic = item->text();
            clickConfig = sConfig; //记录当前正在被使用的配置
            JumpWindowToMainContext(sConfig, topic);
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
    int32_t hW = subHWidget->width(), hH = subHWidget->height();

    lineEdit0->setGeometry(labelX, labelY, 200, 20);
    lineEdit1->setGeometry(labelX, labelY + 20, 200, 20);
    enter->setGeometry(labelX, labelY + 40, 50, 20);
    cancel->setGeometry(labelX + 200 - 50, labelY + 40, 50, 20);
    rightLayout->removeWidget(lineEdit0);
    rightLayout->removeWidget(lineEdit1);
    rightLayout->removeWidget(enter);
    rightLayout->removeWidget(cancel);
    mainLayout->removeWidget(rightWidget);

    connect(enter, &QPushButton::clicked, this, [=](){
        //账号密码有效性检测
        if (!strcmp(lineEdit0->text().toStdString().c_str(), "admin")
                && !strcmp(lineEdit1->text().toStdString().c_str(), "123456")) {
            //printf ("enter\n");
            //创建客户端列表
            serverTable->setRowCount(0); // 初始不设置行，动态添加
            serverTable->setColumnCount(6);
            serverTable->setHorizontalHeaderLabels(QStringList() << "用户账号" << "用户密码" << "主机地址" << "主机端口" << "主题字段" << "操作方式");
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
    resize(800, 600);

    mainWidget = new QWidget;
    rightWidget = new QWidget;
    subVWidget = new QWidget;
    subHWidget = new QWidget;
     //垂直布局器 QVBoxLayout：将内部的控件按照垂直方向排布，一行一个。
    rightLayout = new QVBoxLayout(rightWidget);
    // 水平布局器 QHBoxLayout：将内部的控件按照水平方向排布，一列一个。
    mainLayout = new QHBoxLayout(mainWidget);
#include <QDateTime>
    subVLayout = new QVBoxLayout(NULL);
    // 水平布局器 QHBoxLayout：将内部的控件按照水平方向排布，一列一个。
    subHLayout = new QHBoxLayout(NULL);
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

    contextTable->setRowCount(0); // 初始不设置行，动态添加
    contextTable->setColumnCount(1);
    contextTable->setHorizontalHeaderLabels(QStringList() << "功能列表");
    // 启用拖拽
    contextTable->setDragEnabled(true);
    contextTable->setAcceptDrops(true);
    contextTable->setDropIndicatorShown(true);

    printf (".................row:%d col:%d\n", contextTable->rowCount(), contextTable->columnCount());
    for (int32_t index = 0; index < ModuleDataAttr_Cnt; index++) {
        if (!strcmp(toEnumChineseString((ModuleDataAttr)index), "Ack")) continue;
        int32_t rowCount = contextTable->rowCount();
        contextTable->insertRow(rowCount);
        QTableWidgetItem *item = new QTableWidgetItem(QString::fromUtf8(toEnumChineseString((ModuleDataAttr)index)));
        contextTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        contextTable->setItem(rowCount, 0, item);
    }

    clickConfig = NULL;
    //sHttpServer = NULL;
printf ("%s %d\n", __func__, __LINE__);
    //signalSync = SignalSync_Init;

    //runHttpServer("/home/zeng/share/whttp-server-master/whttp-server-core/whttpserver/downloadFile/");
printf ("%s %d\n", __func__, __LINE__);
    //跳转到登陆界面
    JumpWindowToLogin();
printf ("%s %d\n", __func__, __LINE__);
}

SubWindow::~SubWindow()
{
}

