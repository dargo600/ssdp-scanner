#include "remotemain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RemoteMain w;
    w.show();

    return a.exec();
}
