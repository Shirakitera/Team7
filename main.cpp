#include "mainwindow.h"
#include <QApplication>
#include <QtWidgets>
#include "mainwin.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    mainwin w1;
    w1.show();
    return a.exec();
}
