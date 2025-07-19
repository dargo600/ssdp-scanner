#include "mainwindow.h"


static const char* const SSDP_QUERY_IP = "239.255.255.250";
static const uint16_t SSDP_PORT = 1900;

MainWindow::MainWindow(QWidget *parent)
    : QDialog(parent),
    groupAddress4(QStringLiteral("239.255.255.250"))
{
    udpSocket4.bind(QHostAddress::AnyIPv4, 0);
    udpSocket4.joinMulticastGroup(groupAddress4);
    connect(&udpSocket4, &QUdpSocket::readyRead, this, &MainWindow::readPendingDatagrams);

    receivedMessages = new QTextEdit("Press Start to start doing SSDP Requests.\nWaiting on Datagrams...\n\n");
    startButton = new QPushButton(tr("&Start"));
    auto stopButton = new QPushButton(tr("&Stop"));
    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(startButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(stopButton, QDialogButtonBox::RejectRole);


    connect(startButton, &QPushButton::clicked, this, &MainWindow::startSending);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopSending);
    connect(&timer, &QTimer::timeout, this, &MainWindow::sendDatagram);


    auto mainLayout =new QVBoxLayout;
    mainLayout->addWidget(receivedMessages);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

void MainWindow::startSending()
{
    startButton->setEnabled(false);
    timer.start(1000);
}

void MainWindow::stopSending()
{
    startButton->setEnabled(true);
    timer.stop();
}


void MainWindow::sendDatagram(void)
{
    QByteArray datagramData ("M-SEARCH * HTTP/1.1\r\n" \
        "HOST: 239.255.255.250:1900\r\n"\
        "MAN: \"ssdp:discover\"\r\n"\
        "MX: 1\r\n"\
        "ST: ssdp:all\r\n\r\n");
    qint64 bytesSent = udpSocket4.writeDatagram(datagramData.constData(), groupAddress4, SSDP_PORT);
    if (bytesSent == -1) {
        qCritical() << "Failed to send datagram:" << udpSocket4.errorString();
    } else {
        qDebug() << bytesSent << "bytes to " << groupAddress4.toString() << ":"<< SSDP_PORT;
    }
}

void MainWindow::readPendingDatagrams(void)
{
    QByteArray datagram;

    while (udpSocket4.hasPendingDatagrams()) {
        datagram.resize(qsizetype(udpSocket4.pendingDatagramSize()));
        udpSocket4.readDatagram(datagram.data(), datagram.size());
        receivedMessages->append(tr("\nReceived IPV4 datagram: \"%1\"").arg(datagram.constData()));
    }
}
