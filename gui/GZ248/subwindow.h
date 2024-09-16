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

class SubWindow : public QMainWindow
{
    Q_OBJECT

public:
    SubWindow(QWidget *parent = nullptr);
    int32_t JumpWindowToMainContext(void);
    int32_t JumpWindowToContext(int32_t number);
    int32_t JumpWindowToContextTransmit(int32_t number);
    int32_t SetCurrentIndex(int32_t number);
    int32_t JumpMainWindow(void);
    int32_t JumpWindowToLogin();
    int32_t JumpWindowToServerList();
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
    QTableWidget *itemTable;
    QCheckBox *allCheck;
    const QPalette *palettes;
    QColor *color;

    /*second window*/
    QMainWindow *secondWindow;

//private slots:
//    void lineEdit0Draw(QString str);
};
#endif // SUBWINDOW_H
