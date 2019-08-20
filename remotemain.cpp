#include "remotemain.h"

#include "remotepanel.h"

RemoteMain::RemoteMain(QWidget *parent)
    : QMainWindow(parent)
{
    RemotePanel *panel = new RemotePanel(this);

    setCentralWidget(panel);
}

