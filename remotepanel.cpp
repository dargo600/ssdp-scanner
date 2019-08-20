#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include "remotepanel.h"

using namespace std;

RemotePanel::RemotePanel(QWidget *parent)
    : QWidget(parent)
{
    setMinimumWidth(MIN_W);
    setMinimumHeight(MIN_H);

    QVBoxLayout * layout = new QVBoxLayout;
    setLayout(layout);
    QHBoxLayout * layoutRow = new QHBoxLayout;
    layout->addLayout(layoutRow);
    searchInput = new QLineEdit(this);
    layoutRow->addWidget(searchInput);
    layoutRow->addSpacerItem(new QSpacerItem(10, 1, QSizePolicy::Minimum, QSizePolicy::Maximum));
    layoutRow = new QHBoxLayout;
    layout->addLayout(layoutRow);
    layoutRow->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Maximum));
    buttonScanDevices = new QPushButton("Scan Devices", this);
    layoutRow->addWidget(buttonScanDevices);

    layoutRow->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Maximum));
    buttonSearch = new QPushButton("Search", this);
    layoutRow->addWidget(buttonSearch);

    connect(buttonScanDevices, &QPushButton::clicked, this, &RemotePanel::scanDevices);
    connect(buttonSearch, &QPushButton::clicked, this, &RemotePanel::search);

    layoutRow->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Maximum));
    layout->addSpacerItem(new QSpacerItem(1, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    detectedIPLabel = new QLabel(this);
    layout->addWidget(detectedIPLabel, 0, Qt::AlignCenter);
    layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    ssdpHandler = new ScanSSDP(detectedIPLabel);

    detectedIPLabel->setText("Ready for Scan");
}

RemotePanel::~RemotePanel()
{
    delete searchInput;
    delete buttonSearch;
    delete buttonScanDevices;
    delete detectedIPLabel;
    delete ssdpHandler;
}

void RemotePanel::scanDevices()
{
    detectedIPLabel->setText("Starting Scan...");
    ssdpHandler->sendSSDP();
    updateDetectedIps();
}

void RemotePanel::updateDetectedIps()
{
    string buf;
    unordered_map <string, string> servers = ssdpHandler->getServers();
    for (auto const& ip : servers) {
        buf.append(ip.first + " " + ip.second + "\n");
    }
    detectedIPLabel->setText(QString::fromUtf8(buf.c_str()));
}

void RemotePanel::search()
{
    ssdpHandler->search();
}
