#ifndef QNAVIGATIONWIDGET_H
#define QNAVIGATIONWIDGET_H
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QSlider>
#include <QWheelEvent>

class QNavigationWidget : public QWidget
{
    Q_OBJECT

public:
    QNavigationWidget(QWidget *parent=0);
    ~QNavigationWidget();

    void addItem(const QString &iconPath,const QString &title);
    void setWidth(const int &width);
    void setBackgroundColor(const QColor &color);
    void setSelectColor(const QColor &color);
    void setMouseInColor(const QColor &color);
    void setRowHeight(const int &height);

protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void leaveEvent(QEvent *);
    void wheelEvent(QWheelEvent *);

private:
    QList<QString> listIcons;
    QList<QString> listItems;
    QColor backgroundColor;
    QColor selectedColor;
    QColor mouseInColor;
    int rowHeight;
    int currentIndex;
    int mouseMoveIndex;
    //QProgressBar *progressBar;

signals:
    void currentItemChanged(const int &index);
};

#endif // QNAVIGATIONWIDGET_H
