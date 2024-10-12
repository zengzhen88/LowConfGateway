#include "subwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    printf ("%s %d\n", __func__, __LINE__);
    QApplication a(argc, argv);
    setbuf(stdout, NULL);
    SubWindow w;
    w.show();
    return a.exec();
}
