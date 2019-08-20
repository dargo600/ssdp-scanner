#ifndef REMOTEMAIN_H
#define REMOTEMAIN_H
#pragma once

#include <QMainWindow>

namespace Ui {
class RemoteMain;
}

class RemoteMain : public QMainWindow
{
    Q_OBJECT

public:
    RemoteMain(QWidget *parent = nullptr);
};

#endif // REMOTEMAIN_H
