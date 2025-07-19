#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma once

#include <QTextEdit>
#include <QtWidgets>
#include <QtNetwork>
#include <QtCore>

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void readPendingDatagrams();
    void startSending();
    void stopSending();
    void sendDatagram();


private:
    QTextEdit *receivedMessages = nullptr;
    QPushButton *startButton = nullptr;
    QUdpSocket udpSocket4;
    QTimer timer;
    QHostAddress groupAddress4;
    int messageNo = 1;
    bool receivedDatagram = false;

};

#endif // MAINWINDOW_H
