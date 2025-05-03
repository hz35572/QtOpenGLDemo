#include "HelloTriangle.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HelloTriangleWidget w;
    w.show();
    return a.exec();
}
