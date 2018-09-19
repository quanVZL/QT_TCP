#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Sever");
    mTcpServer = std::unique_ptr<QTcpServer>(new QTcpServer);
    ui->ipData->setText(QNetworkInterface::allAddresses().at(2).toString());
    ui->portData->setText("8080");
    ui->connectBtn->setEnabled(true);
    ui->sendBtn->setEnabled(false);

    connect(mTcpServer.get(), &QTcpServer::newConnection, this, &MainWindow::newConnectionClient);
    initUI();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::btnConnectClick() {
    if (ui->connectBtn->text() == "监听") {
        bool connectState = mTcpServer->listen(QHostAddress::Any, ui->portData->toPlainText().toInt());
        if (connectState) {
            ui->connectBtn->setText(tr("断开"));
            ui->sendBtn->setEnabled(true);
        }
    } else {
        auto clientCount = mTcpClientList.length();
        for (int i = 0; i < clientCount; ++i) {
            mTcpClientList[i]->disconnectFromHost();
            if (!mTcpClientList[i]->waitForDisconnected(1000)) {
                qWarning() << "disconnection failed!";
            }
            mTcpClientList.removeAt(i);
        }
        mTcpServer->close();
        ui->connectBtn->setText(tr("监听"));
        ui->sendBtn->setEnabled(false);
    }
}

void MainWindow::btnSendClick() {
    QString data = ui->sendData->toPlainText();
    if (data.isEmpty()) return;
    auto clientCount = mTcpClientList.length();

    if (ui->clientList->currentIndex() == 0) {
        for (int i = 0; i < clientCount; ++i) {
            mTcpClientList[i]->write(data.toLatin1());
        }
    } else {
        QString clientIp = ui->clientList->currentText().split(":")[0];
        int clientPort = ui->clientList->currentText().split(":")[1].toInt();
        for (int i = 0; i < clientCount; ++i) {
            if (mTcpClientList[i]->peerAddress().toString().split("::ffff:")[1] == clientIp &&
                    mTcpClientList[i]->peerPort() == clientPort) {
                mTcpClientList[i]->write(data.toLatin1());
                return;
            }
        }
    }
}

void MainWindow::btnClearClick() {
    ui->recvText->clear();
}

void MainWindow::readData() {
    auto clientCount = mTcpClientList.length();
    for (int i = 0; i < clientCount; ++i) {
        QByteArray buffer = mTcpClientList[i]->readAll();
        if (buffer.isEmpty()) continue;

        static QString ipPort, ipPortPrev;
        ipPort = tr("%1%2").arg(mTcpClientList[i]->peerAddress().toString().split("::ffff:")[1]).arg(mTcpClientList[i]->peerPort());
        if (ipPort != ipPortPrev) {
            ui->recvText->append(ipPort);
        }
        ui->recvText->append(buffer);
        ipPortPrev = ipPort;
    }
}

void MainWindow::newConnectionClient() {
    currentClient = mTcpServer->nextPendingConnection();
    mTcpClientList.append(currentClient);
    ui->clientList->addItem(tr("%1%2").arg(currentClient->peerAddress().toString().split("::ffff:")[1]).arg(currentClient->peerPort()));
    connect(currentClient, &QTcpSocket::readyRead, this, &MainWindow::readData);
    connect(currentClient, &QTcpSocket::disconnected, this, &MainWindow::disconnectClient);
}

void MainWindow::disconnectClient() {
    auto clientCount = mTcpClientList.length();
    for (int i = 0; i < clientCount; ++i) {
        if (mTcpClientList[i]->state() == QAbstractSocket::UnconnectedState) {
            ui->clientList->removeItem(ui->clientList->findText(tr("%1%2").arg(mTcpClientList[i]->peerAddress().toString().split("::ffff:")[1])
                                                            .arg(mTcpClientList[i]->peerPort())));
            mTcpClientList[i]->destroyed();;
            mTcpClientList.removeAt(i);
        }
    }
}

void MainWindow::initUI() {
    connect(ui->connectBtn, &QPushButton::clicked, this, &MainWindow::btnConnectClick);
    connect(ui->sendBtn, &QPushButton::clicked, this, &MainWindow::btnSendClick);
    connect(ui->clearBtn, &QPushButton::clicked, this, &MainWindow::btnClearClick);
}
