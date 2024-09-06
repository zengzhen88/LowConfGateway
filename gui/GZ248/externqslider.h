#ifndef EXTERNQSLIDER_H
#define EXTERNQSLIDER_H

#include <QApplication>
#include <QSlider>
#include <QWheelEvent>

class ExternQSlider : public QSlider {
protected:
    virtual void wheelEvent(QWheelEvent *event) {
        // 不调用父类的wheelEvent，从而禁用鼠标滚轮对此slider的影响
    }
};

#endif // EXTERNQSLIDER_H
