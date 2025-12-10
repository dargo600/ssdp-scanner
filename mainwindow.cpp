#include "mainwindow.h"
/*
 * mainwindow.h needs to exist in order to build properly
 * as QT updates the class based on the _Q_OBJECT value.
 * The header file is below for convenience and should match the .h file
 * with comments removed.
 * This can be built by following instructions in the README.md file which is
 * also found here: https://github.com/dargo600/ssdp-scanner Or by doing:
 * ./build.sh
 * which will create a buildDir/ subdirectory and build the project there.
 * The resulting binary will be in buildDir/ssdpscanner
 * The README.md provides a png image to display what the GUI looks like when
 * run.
 */
#ifdef DISPLAY_HEADER_FILE_FOR_REFERENCE_ONLY
// #ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma once

#include <QMutex>
#include <QTextEdit>
#include <QtCore>
#include <QtNetwork>
#include <QtWidgets>

class MainWindow : public QDialog
{
    // Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

   private slots:
    void readPendingDatagrams();
    void startSending();
    void stopSending();
    void sendDatagram();

   private:
    void updateReceivedMessages(const QString &serverString,
                                const quint16 serverPort,
                                const QString &responseData);
    QTextEdit *receivedMessages = nullptr;
    QPushButton *startButton = nullptr;
    QUdpSocket *socket4 = nullptr;
    QTimer timer;
    QHostAddress groupAddress4;
    QHash<QString, QString> receivedResponses;
    QMutex mutex;  // Mutex to protect access to serverResponses
    int messageNo = 1;
    bool receivedDatagram = false;
};

// #endif // MAINWINDOW_H
#endif

#define SSDP_QUERY_IP "239.255.255.250"

static const uint16_t SSDP_PORT = 1900;

/**
 * @brief The MainWindow class
 *
 * This class implements a simple SSDP Scanner using QUdpSocket
 * to send M-SEARCH requests and receive responses.
 * It provides a GUI with a text edit to display received messages
 * and buttons to start and stop sending requests.  It uses a QTimer
 * to periodically send requests every 5 seconds.
 * The class ensures thread-safe access to received responses
 * using a QMutex.  It also avoids displaying duplicate entries in the display
 * by maintaining a hash of received responses and only displaying the new ones
 * that were added to the hash map.
 *
 * The slot mechanism is used to handle incoming datagrams
 * and button clicks.  It uses an observer type pattern to update subscribers
 * that were specified using slots.  This behaves similarly to libevent though
 * with QT has its own event loop, it also runs in a different thread then the
 * main thread. This program appeared to run 7 threads according to GDB's info
 * threads command.  Futher information on that is in the README.md file.
 */
 * @param parent[in] The parent widget for the dialog.
 */
MainWindow::MainWindow(QWidget *parent)
    : QDialog(parent), groupAddress4(QStringLiteral(SSDP_QUERY_IP))
 {
     socket4 = new QUdpSocket(this);
     if (!socket4->bind(
             QHostAddress::AnyIPv4, SSDP_PORT,
             QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
     {
         qCritical() << "Failed to bind socket:" << socket4->errorString();
     }
     if (!socket4->joinMulticastGroup(groupAddress4))
     {
         qCritical() << "Failed to join multicast group:"
                     << socket4->errorString();
     }
     connect(socket4, &QUdpSocket::readyRead, this,
             &MainWindow::readPendingDatagrams);

     receivedMessages = new QTextEdit(
         "Press Start to start doing SSDP Requests.\nWaiting on "
         "Datagrams...\n\n");
     startButton = new QPushButton(tr("&Start"));
     auto stopButton = new QPushButton(tr("&Stop"));
     auto buttonBox = new QDialogButtonBox;
     buttonBox->addButton(startButton, QDialogButtonBox::ActionRole);
     buttonBox->addButton(stopButton, QDialogButtonBox::RejectRole);

     connect(startButton, &QPushButton::clicked, this,
             &MainWindow::startSending);
     connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopSending);
     connect(&timer, &QTimer::timeout, this, &MainWindow::sendDatagram);

     auto mainLayout = new QVBoxLayout;
     mainLayout->addWidget(receivedMessages);
     mainLayout->addWidget(buttonBox);
     setLayout(mainLayout);
 }

 /**
  * @brief MainWindow::~MainWindow
  *
  * Destructor for MainWindow class.
  * Leaves the multicast group and closes the socket.
  * Note socket4 is a child of MainWindow and will be deleted automatically
  * as it is a QObject child like all of the other QT widgets which also
  * get deleted automatically.
  */
 MainWindow::~MainWindow()
 {
     if (socket4)
     {
         socket4->leaveMulticastGroup(groupAddress4);
         qDebug() << "Leaving Multicast group\n";
         socket4->close();
     }
 }

 /**
  * @brief MainWindow::startSending
  * This slot is called to start sending SSDP M-SEARCH datagrams.
  * It starts the timer that triggers sending datagrams
  * and is connected to the Start button.
  */
 void MainWindow::startSending()
 {
     startButton->setEnabled(false);
     timer.start(1000 * 5);  // Send every 5 seconds
 }

 /**
  * @brief MainWindow::stopSending
  *
  * This slot is called to stop sending SSDP M-SEARCH datagrams.
  * It stops the timer that triggers sending datagrams and is connected
  * to the Stop button.
  */
 void MainWindow::stopSending()
 {
     startButton->setEnabled(true);
     timer.stop();
 }

 /**
  * @brief MainWindow::sendDatagram
  *
  * This slot is called to send an SSDP M-SEARCH datagram
  * to the multicast address and port.
  */
 void MainWindow::sendDatagram(void)
 {
     std::string datagramStr =
         "M-SEARCH * HTTP/1.1\r\n"
         "HOST: " +
         std::string(SSDP_QUERY_IP) + ":" + std::to_string(SSDP_PORT) +
         "\r\n"
         "MAN: \"ssdp:discover\"\r\n"
         "MX: 2\r\n"
         "ST: ssdp:all\r\n\r\n";
     QByteArray datagramData(datagramStr.c_str());
     qint64 bytesSent = socket4->writeDatagram(datagramData.constData(),
                                               groupAddress4, SSDP_PORT);
     if (bytesSent == -1)
     {
         qCritical() << "Failed to send datagram:" << socket4->errorString();
     }
     else
     {
         qDebug() << bytesSent << "bytes to " << groupAddress4.toString() << ":"
                  << SSDP_PORT;
     }
 }

 /**
  * This is a helper function to update the received messages display.
  * It ensures that duplicate entries are not added to the display
  * by checking the receivedResponses hash map.
  * It uses a mutex to protect access to the hash map
  */
 void MainWindow::updateReceivedMessages(const QString &serverString,
                                         const quint16 serverPort,
                                         const QString &responseData)
 {
     QMutexLocker locker(&mutex);
     if (!receivedResponses.contains(serverString))
     {
         QString senderString =
             serverString + QStringLiteral(":%1").arg(serverPort);
         qDebug() << serverString << "adding\n";
         receivedResponses.insert(serverString, responseData);
         receivedMessages->append("\n" + senderString + "\n" + responseData +
                                  "\n");
     }
 }

 /**
  * @brief MainWindow::readPendingDatagrams
  *
  * This slot is called when there are pending datagrams to read.
  * It reads all pending datagrams from the UDP socket and updates
  * the received messages display.
  */
 void MainWindow::readPendingDatagrams(void)
 {
     QByteArray datagram;

     while (socket4->hasPendingDatagrams())
     {
         QHostAddress senderAddress;
         quint16 senderPort;
         datagram.resize(qsizetype(socket4->pendingDatagramSize()));
         socket4->readDatagram(datagram.data(), datagram.size(), &senderAddress,
                               &senderPort);
         updateReceivedMessages(senderAddress.toString(), senderPort,
                                datagram.constData());
     }
 }

 int main(int argc, char *argv[])
 {
     QApplication a(argc, argv);
     a.thread()->setObjectName("MainThread");
     QCoreApplication::setApplicationName(QString("SSDP Scanner"));
     QCoreApplication::setApplicationVersion(QString("0.1"));

     MainWindow w;
     w.show();

     return a.exec();
 }