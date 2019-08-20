#ifndef SCANSSDP_H
#define SCANSSDP_H

#include <QUdpSocket>
#include <QHostAddress>
#include <QObject>
#include <unordered_map>
#include <QLabel>

class ScanSSDP : public QObject
{
    Q_OBJECT

public:
    explicit ScanSSDP(QObject *parent = nullptr);
    ~ScanSSDP();
    bool isScanning();
    void sendSSDP();
    void search();
    std::unordered_map<std::string, std::string> getServers();
signals:

public slots:
    void readyRead();

private:
    void processSSDPResponses();
    std::string parseLocation(std::string response);

    bool scanning;
    QUdpSocket *udpSocket;
    QHostAddress *sendToAddress;
    std::unordered_map<std::string, std::string> servers;
};

#endif // SCANSSDP_H
