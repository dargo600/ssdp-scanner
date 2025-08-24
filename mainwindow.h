#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma once

#include <QTextEdit>
#include <QtWidgets>
#include <QtNetwork>
#include <QtCore>
#include <QMutex>

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void readPendingDatagrams();
    void startSending();
    void stopSending();
    void sendDatagram();


private:
    void updateReceivedMessages(const QString &serverString, const quint16 serverPort, const QString &responseData);
    QTextEdit *receivedMessages = nullptr;
    QPushButton *startButton = nullptr;
    QUdpSocket *socket4 = nullptr;
    QTimer timer;
    QHostAddress groupAddress4;
    QHash<QString, QString> receivedResponses;
    QMutex mutex; // Mutex to protect access to serverResponses
    int messageNo = 1;
    bool receivedDatagram = false;

};

#endif // MAINWINDOW_H
