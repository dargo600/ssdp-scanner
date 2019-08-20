#ifndef REMOTEPANEL_H
#define REMOTEPANEL_H

#include <QtWidgets>

#include "scanssdp.h"

class RemotePanel : public QWidget
{
    Q_OBJECT

public:
    explicit RemotePanel(QWidget *parent = nullptr);
    ~RemotePanel();

signals:

public slots:
    void scanDevices();
    void search();

private:
    void updateDetectedIps();

    static const int MIN_W = 480;
    static const int MIN_H = 240;
    QLineEdit *searchInput;
    QPushButton *buttonScanDevices;
    QPushButton *buttonSearch;
    QLabel *detectedIPLabel;
    ScanSSDP *ssdpHandler;

};

#endif // REMOTEPANEL_H
