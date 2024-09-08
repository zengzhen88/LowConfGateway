#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QMainWindow *parent = nullptr);
    int32_t JumpSubWindow(int32_t number);
    int32_t JumpTransmit(int32_t number);
    int32_t SetCurrentIndex(int32_t number);
    void ClearWidget(void);
    ~MainWindow();
    QVBoxLayout *rightLayout;
    QHBoxLayout *mainLayout;
    QVBoxLayout *subVLayout;
    QHBoxLayout *subHLayout;
    QPushButton *enter;
    QPushButton *cancel;
    QLineEdit *lineEdit0;
    QLineEdit *lineEdit1;
    QLineEdit *lineEdit2;
    QLineEdit *lineEdit3;
    QLineEdit *lineEdit4;
    QLineEdit *lineEdit5;
    QLabel *label;

    int32_t currentIndex;

//private slots:
//    void lineEdit0Draw(QString str);
};
#endif // MAINWINDOW_H
