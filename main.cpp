#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName(QString("SSDP Scanner"));
    QCoreApplication::setApplicationVersion(QString("0.1"));

    MainWindow w;
    w.show();

    return a.exec();
}
