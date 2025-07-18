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

    QString msg = tr("Ready to mutlicast datagrams to groups %1 on port 1900").arg(groupAddress4.toString());
    statusLabel = new QLabel(msg);
    startButton = new QPushButton(tr("&Start"));
    auto stopButton = new QPushButton(tr("&Stop"));
    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(startButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(stopButton, QDialogButtonBox::RejectRole);


    connect(startButton, &QPushButton::clicked, this, &MainWindow::startSending);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopSending);
    connect(&timer, &QTimer::timeout, this, &MainWindow::sendDatagram);


    auto mainLayout =new QVBoxLayout;
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    /*
    connect(ui->actionScan, &QPushButton::clicked, this, &MainWindow::requestNewSSDPScan);

    connect(&scanThread, &ScanSSDP::newScan, this, &MainWindow::showNewScan);
    connect(&scanThread, &ScanSSDP::error, this, &MainWindow::displayError)

*/
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
    statusLabel->setText(tr("Now sending datagram %1").arg(messageNo));
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
    messageNo++;

}

void MainWindow::readPendingDatagrams(void)
{
    QByteArray datagram;

    while (udpSocket4.hasPendingDatagrams()) {
        datagram.resize(qsizetype(udpSocket4.pendingDatagramSize()));
        udpSocket4.readDatagram(datagram.data(), datagram.size());
        statusLabel->setText(tr("Received IPV4 datagram: \"%1\"").arg(datagram.constData()));
    }
}

/**
void MainWindow::onActionScanTriggered()
{
       ScanSSDP *scan = new ScanSSDP(ui->listView);
    scan->sendSSDP();

    QString text = tr("A SSDP Scanning application that listens on 239.255.255.250:1300");
    QMessageBox::about(this, tr("FIXME"), text);

}

void MainWindow::onActionAboutTriggered()
{
    QMessageBox::about(this, tr("About SSDP Scanner"), QString("SSDP Scanner Scans for traffic being sent on 239.255.255.250:1900"));

 const QString viewerNames = m_factory->viewerNames().join(", "_L1);
    const QString mimeTypes = m_factory->supportedMimeTypes().join(u'\n');
    QString text = tr("A Widgets application to display and print JSON, "
                      "text and PDF files. Demonstrates various features to use "
                      "in widget applications: Using QSettings, query and save "
                      "user preferences, manage file histories and control cursor "
                      "behavior when hovering over widgets.\n\n"
                      "This version has loaded the following plugins:\n%1\n"
                      "\n\nIt supports the following mime types:\n%2")
                   jjjjjjjjj    .arg(viewerNames, mimeTypes);

    if (auto *def = m_factory->defaultViewer())
        text += tr("\n\nOther mime types will be displayed with %1.").arg(def->viewerName());

    QMessageBox::about(this, tr("About Document Viewer Demo"), text);

}
**/

/**
 * MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onActionOpenTriggered);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onActionAboutTriggered);
    connect(ui->actionAboutQt, &QAction::triggered, this, &MainWindow::onActionAboutQtTriggered);

    m_recentFiles.reset(new RecentFiles(ui->actionRecent));
    connect(m_recentFiles.get(), &RecentFiles::countChanged, this, [&](int count){
        ui->actionRecent->setText(tr("%n recent files", nullptr, count));
    });

    readSettings();
    m_factory.reset(new ViewerFactory(ui->viewArea, this));
    const QStringList &viewers = m_factory->viewerNames();

    const QString msg = tr("Available viewers: %1").arg(viewers.join(", "_L1));
    statusBar()->showMessage(msg);

    auto *menu = new RecentFileMenu(this, m_recentFiles.get());
    ui->actionRecent->setMenu(menu);
    connect(menu, &RecentFileMenu::fileOpened, this, &MainWindow::openFile);
    QWidget *w = ui->mainToolBar->widgetForAction(ui->actionRecent);
    auto *button = qobject_cast<QToolButton *>(w);
    if (button)
        connect(ui->actionRecent, &QAction::triggered, button, &QToolButton::showMenu);
}

bool MainWindow::hasPlugins() const
{
    return m_factory ? !m_factory->viewers().isEmpty() : false;
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::onActionOpenTriggered()
{
    QFileDialog fileDialog(this, tr("Open Document"), m_currentDir.absolutePath());
    while (fileDialog.exec() == QDialog::Accepted
           && !openFile(fileDialog.selectedFiles().constFirst())) {
    }
}

bool MainWindow::openFile(const QString &fileName)
{
    QFile *file = new QFile(fileName);
    if (!file->exists()) {
        statusBar()->showMessage(tr("File %1 could not be opened")
                                 .arg(QDir::toNativeSeparators(fileName)));
        delete file;
        return false;
    }

    QFileInfo fileInfo(*file);
    m_currentDir = fileInfo.dir();
    m_recentFiles->addFile(fileInfo.absoluteFilePath());

    // If a viewer is already open, clean it up and save its settings
    resetViewer();
    m_viewer = m_factory->viewer(file);
    if (!m_viewer) {
        statusBar()->showMessage(tr("File %1 can't be opened.")
                                 .arg(QDir::toNativeSeparators(fileName)));
        return false;
    }

    for (const QMetaObject::Connection &connection : m_viewerConnections)
        disconnect(connection);

    m_viewerConnections = {
        connect(m_viewer, &AbstractViewer::printingEnabledChanged, ui->actionPrint,
                &QAction::setEnabled),
        connect(ui->actionPrint, &QAction::triggered, m_viewer, &AbstractViewer::print),
        connect(m_viewer, &AbstractViewer::showMessage, statusBar(), &QStatusBar::showMessage)
    };

    m_viewer->initViewer(ui->actionBack, ui->actionForward, ui->menuHelp->menuAction(), ui->tabWidget);
    restoreViewerSettings();
    ui->scrollArea->setWidget(m_viewer->widget());
    return true;
}

void MainWindow::onActionAboutTriggered()
{
    const QString viewerNames = m_factory->viewerNames().join(", "_L1);
    const QString mimeTypes = m_factory->supportedMimeTypes().join(u'\n');
    QString text = tr("A Widgets application to display and print JSON, "
                      "text and PDF files. Demonstrates various features to use "
                      "in widget applications: Using QSettings, query and save "
                      "user preferences, manage file histories and control cursor "
                      "behavior when hovering over widgets.\n\n"
                      "This version has loaded the following plugins:\n%1\n"
                      "\n\nIt supports the following mime types:\n%2")
                    .arg(viewerNames, mimeTypes);

    if (auto *def = m_factory->defaultViewer())
        text += tr("\n\nOther mime types will be displayed with %1.").arg(def->viewerName());

    QMessageBox::about(this, tr("About Document Viewer Demo"), text);
}

void MainWindow::onActionAboutQtTriggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::readSettings()
{
    QSettings settings;

    // Restore working directory
    if (settings.contains(settingsDir))
        m_currentDir = QDir(settings.value(settingsDir).toString());
    else
        m_currentDir = QDir::current();

    // Restore QMainWindow state
    if (settings.contains(settingsMainWindow)) {
        QByteArray mainWindowState = settings.value(settingsMainWindow).toByteArray();
        restoreState(mainWindowState);
    }

    // Restore recent files
    m_recentFiles->restoreFromSettings(settings, settingsFiles);
}

void MainWindow::saveSettings() const
{
    QSettings settings;

    // Save working directory
    settings.setValue(settingsDir, m_currentDir.absolutePath());

    // Save QMainWindow state
    settings.setValue(settingsMainWindow, saveState());

    // Save recent files
    m_recentFiles->saveSettings(settings, settingsFiles);

    settings.sync();
}

void MainWindow::saveViewerSettings() const
{
    if (!m_viewer)
        return;

    QSettings settings;
    settings.beginGroup(settingsViewers);
        settings.setValue(m_viewer->viewerName(), m_viewer->saveState());
    settings.endGroup();
    settings.sync();
}

void MainWindow::resetViewer() const
{
    if (!m_viewer)
        return;

    saveViewerSettings();
    m_viewer->cleanup();
}

void MainWindow::restoreViewerSettings()
{
    if (!m_viewer)
        return;

    QSettings settings;
    settings.beginGroup(settingsViewers);
    QByteArray viewerSettings = settings.value(m_viewer->viewerName(), QByteArray()).toByteArray();
    settings.endGroup();
    if (!viewerSettings.isEmpty())
        m_viewer->restoreState(viewerSettings);
} */
