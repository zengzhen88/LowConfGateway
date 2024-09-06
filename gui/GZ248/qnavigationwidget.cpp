
#include <QPainter>
#include <QDebug>
#include "qnavigationwidget.h"
#include <qpainter.h>
#include <QPainterPath>


void QNavigationWidget::paintEvent(QPaintEvent *)
{
    int32_t w = width();
    int32_t h = height();
    int32_t size = h / rowHeight; //display item num
    printf ("w:%d h:%d size:%d\n", w, h, size);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Draw background color.
    painter.setPen(Qt::NoPen);
    painter.setBrush(backgroundColor);
    painter.drawRect(rect());

    // Draw Items
    int count = 0;
    int first = 0;
    for (const QString &str : listItems) {
        if (count <= currentIndex - size) {
            //whell
            count++;
            continue;
        }
        if (first >= size) {
            count++;
            continue;
        }

        QPainterPath itemPath;
        //QPainterPath *itemPath = QPainterPath();
        itemPath.addRect(QRect(0, first * rowHeight, width(), rowHeight));

        if (currentIndex == count)
        {
            painter.setPen("#FFFFFF");
            painter.fillPath(itemPath, selectedColor);
        }
        else if(mouseMoveIndex == count)
        {
            painter.setPen("#FFFFFF");
            painter.fillPath(itemPath, mouseInColor);
        }
        else
        {
            painter.setPen("#202020");
            painter.fillPath(itemPath, backgroundColor);
        }

        //painter.drawText(QRect(40, count * rowHeight, width()-40, rowHeight), Qt::AlignVCenter | Qt::AlignHCenter, str);
        //painter.drawPixmap(QRect(20, (count * rowHeight+(rowHeight-20)/2), 20, 20),QPixmap(listIcons[count]));
        painter.drawText(QRect(45, first * rowHeight, width()-40, rowHeight), Qt::AlignVCenter, str);

        ++count;
        first++;
    }
}

QNavigationWidget::QNavigationWidget(QWidget *parent) : QWidget(parent)
{
    backgroundColor = "#E4E4E4";
    selectedColor = "#2CA7F8";
    mouseInColor = "#C4C4C4";
    rowHeight = 40;
    currentIndex = 0;
    mouseMoveIndex = -1;

    setMouseTracking(true);
    setFixedWidth(150);
}

QNavigationWidget::~QNavigationWidget()
{
}

void QNavigationWidget::addItem(const QString &iconPath,const QString &title)
{
    listIcons << iconPath;
    listItems << title;

    update();
}

void QNavigationWidget::setWidth(const int &width)
{
    setFixedWidth(width);
}

void QNavigationWidget::setBackgroundColor(const QColor &color)
{
    backgroundColor = color;

    update();
}

void QNavigationWidget::setSelectColor(const QColor &color)
{
    selectedColor = color;

    update();
}

void QNavigationWidget::setMouseInColor(const QColor &color)
{
    mouseInColor = color;

    update();
}

void QNavigationWidget::setRowHeight(const int &height)
{
    rowHeight = height;

    update();
}

void QNavigationWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (e->y() / rowHeight < listItems.count())
    {
        mouseMoveIndex = e->y() / rowHeight;
    }
    else
    {
        mouseMoveIndex = -1;
    }

    update();
}

void QNavigationWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->y() / rowHeight < listItems.count())
    {
        currentIndex = e->y() / rowHeight;
        emit currentItemChanged(currentIndex);
        update();
    }
}

void QNavigationWidget::leaveEvent(QEvent *)
{
    if(mouseMoveIndex !=-1 )
    {
        mouseMoveIndex = -1;
        update();
    }
}

void QNavigationWidget::wheelEvent(QWheelEvent *event) {
    QPoint delta = event->angleDelta();
    if (delta.y() > 0) {
        // 处理向上滚动
        printf ("delta.y:%d\n", delta.y());
        currentIndex--;
        if (currentIndex <= 0) {
            currentIndex = 0;
        }
        emit currentItemChanged(currentIndex);
        update();
    } else if (delta.y() < 0) {
        // 处理向下滚动
         printf ("1delta.y:%d\n", delta.y());
        currentIndex++;
        if (currentIndex >= listItems.size()) {
            currentIndex = listItems.size() - 1;
        }
        emit currentItemChanged(currentIndex);
        update();
    }
}
