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

enum SubWindowType {
    SubWindowType_Login,
    SubWindowType_ServerList,
    SubWindowType_MainContext,
    SubWindowType_Context,
    SubWindowType_Cnt
};

class SubWindow : public QMainWindow
{
    Q_OBJECT   

protected:
    void paintEvent(QPaintEvent *);
    void changeEvent(QEvent *event);
    void resizeEvent(QResizeEvent * resizeEvent);
public:
    SubWindow(QWidget *parent = nullptr);
    int32_t JumpWindowToMainContext(void);
    int32_t JumpWindowToContext(int32_t number);
    int32_t JumpWindowToContextTransmit(int32_t number);
    int32_t SetCurrentIndex(int32_t number);
    int32_t JumpMainWindow(void);
    int32_t JumpWindowToLogin();
    int32_t JumpWindowToServerList();
    int32_t ClearQnavigationWidget(void);
    void ClearWidget(void);
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
    QMap<long, long> serverTableMap;
    QTableWidget *contextTable;
    QCheckBox *allCheck;
    const QPalette *palettes;
    QColor *color;
    QColor backgroundColor;
    /*second window*/
    QMainWindow *secondWindow;

    SubWindowType windowType;

//private slots:
//    void lineEdit0Draw(QString str);
};
#endif // SUBWINDOW_H