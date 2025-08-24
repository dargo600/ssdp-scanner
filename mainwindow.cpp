#include "mainwindow.h"


static const char* const SSDP_QUERY_IP = "239.255.255.250";
static const uint16_t SSDP_PORT = 1900;

MainWindow::MainWindow(QWidget *parent)
    : QDialog(parent),
    groupAddress4(QStringLiteral("239.255.255.250"))
{
    socket4 = new QUdpSocket(this);
    if (!socket4->bind(QHostAddress::AnyIPv4, 1900, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
    {
        qCritical() << "Failed to bind socket:" << socket4->errorString();
    }
    if (!socket4->joinMulticastGroup(groupAddress4)) {
        qCritical() << "Failed to join multicast group:" << socket4->errorString();
    }
    connect(socket4, &QUdpSocket::readyRead, this, &MainWindow::readPendingDatagrams);

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

MainWindow::~MainWindow()
{
    if (socket4) {
        socket4->leaveMulticastGroup(groupAddress4);
        qDebug() << "Leaving Multicast group\n";
        socket4->close();
    }
}

void MainWindow::startSending()
{
    startButton->setEnabled(false);
    timer.start(1000 * 5); // Send every 5 seconds
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
        "MX: 2\r\n"\
        "ST: ssdp:all\r\n\r\n");
    qint64 bytesSent = socket4->writeDatagram(datagramData.constData(), groupAddress4, SSDP_PORT);
    if (bytesSent == -1) {
        qCritical() << "Failed to send datagram:" << socket4->errorString();
    } else {
        qDebug() << bytesSent << "bytes to " << groupAddress4.toString() << ":"<< SSDP_PORT;
    }
}

void MainWindow::updateReceivedMessages(const QString &serverString, const quint16 serverPort, const QString &responseData)
{
    QMutexLocker locker(&mutex);
    if (!receivedResponses.contains(serverString)) {
        QString senderString = serverString + QStringLiteral(":%1").arg(serverPort);
        qDebug() << serverString << "adding\n";
        receivedResponses.insert(serverString, responseData);
        receivedMessages->append("\n" +senderString + "\n" + responseData + "\n");
    }
}

void MainWindow::readPendingDatagrams(void)
{
    QByteArray datagram;

    while (socket4->hasPendingDatagrams()) {
        QHostAddress senderAddress;
        quint16 senderPort;
        datagram.resize(qsizetype(socket4->pendingDatagramSize()));
        socket4->readDatagram(datagram.data(), datagram.size(), &senderAddress, &senderPort);
        updateReceivedMessages(senderAddress.toString(), senderPort, datagram.constData());
    }
}
