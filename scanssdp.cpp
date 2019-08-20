#include <QtNetwork>
#include <QHostAddress>
#include <QUdpSocket>
#include <QString>
#include <QByteArray>

#include "scanssdp.h"

using namespace std;

static const char* const SSDP_QUERY_IP = "239.255.255.250";
static const char* const END_LINE = "\r\n";
static const char* const SSDP_QUERY = "M-SEARCH * HTTP/1.1\r\n" \
                    "HOST: 239.255.255.250:1900\r\n" \
                    "MAN: \"ssdp:discover\"\r\n" \
                    "MX: 1\r\n";
static const char* const ROKU_ST = "ST: roku:ecp\r\n";
static const char* const ALL_ST = "ST: ssdp:all\r\n";
static const quint16 SSDP_PORT = 1900;


ScanSSDP::ScanSSDP(QObject *parent) :
    QObject(parent)
{
    udpSocket = new QUdpSocket(this);

    sendToAddress = new QHostAddress(SSDP_QUERY_IP);
    udpSocket->bind(QHostAddress::AnyIPv4, 50950, QUdpSocket::ShareAddress);
    udpSocket->joinMulticastGroup(*sendToAddress);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    scanning = true;
    sendSSDP();
}

ScanSSDP::~ScanSSDP() {
    delete udpSocket;
    delete sendToAddress;
}

bool ScanSSDP::isScanning() {
    return scanning;
}

void ScanSSDP::sendSSDP() {
    QByteArray data;
    scanning = true;
    data.append(SSDP_QUERY);
    data.append(ALL_ST);
    data.append(END_LINE);
    qDebug() << "Sending\n" << SSDP_QUERY << ALL_ST << END_LINE;
    const int TRIES = 1;

    for (int i = 0; i < TRIES; ++i) {
        auto writeOk = udpSocket->writeDatagram(data.data(), *sendToAddress, SSDP_PORT);
        if (writeOk == -1) {
            printf("Writing Datagram failed\n");
            return;
        }
    }
    qDebug() << "Sent " << TRIES << " of ssdp queries";
}

void ScanSSDP::readyRead() {
    std::chrono::duration<double> max_elapsed_seconds { 2 };
    auto start = std::chrono::system_clock::now();
    auto end = start;
    std::chrono::duration<double> elapsed_seconds = end - start;
    while (elapsed_seconds <= max_elapsed_seconds) {
        while (udpSocket->hasPendingDatagrams()) {
            processSSDPResponses();
        }
        end = std::chrono::system_clock::now();
        elapsed_seconds = end - start;
    }
    double elapsed = static_cast<double>(elapsed_seconds.count());
    qDebug() << "Done processing in " << elapsed;
    scanning = false;
}

void ScanSSDP::processSSDPResponses() {
    QHostAddress sender;
    quint16 senderPort;
    QByteArray buf(udpSocket->pendingDatagramSize(), Qt::Uninitialized);
    QDataStream str(&buf, QIODevice::ReadOnly);
    udpSocket->readDatagram(buf.data(), buf.size(), &sender, &senderPort);
    string ip = sender.toString().toStdString();
    if (servers.find(ip) == servers.end()) {
        string location = parseLocation(string(buf.toStdString()));
        servers.insert(pair<string, string>(ip, location));
    }
}

string ScanSSDP::parseLocation(string response) {
    string location = "";
    size_t found = response.find("LOCATION: ");
    if (found != string::npos) {
        location.append(response.substr(found + strlen("LOCATION: ")));
        size_t newLine = location.find("\r\n");
        if (newLine != string::npos) {
           location = location.substr(0, newLine);
           qDebug() << "Location is " << location.c_str();
        }
    }

    return location;
}

unordered_map<string, string> ScanSSDP::getServers() {
    return servers;
}

void ScanSSDP::search() {

}
