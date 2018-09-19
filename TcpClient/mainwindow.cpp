#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Client");
    mTcpClient = std::unique_ptr<QTcpSocket>(new QTcpSocket);
    mTcpClient->abort();

    ui->btnConnect->setEnabled(true);
    ui->btnSend->setEnabled(false);

    connect(mTcpClient.get(), &QTcpSocket::readyRead, this, &MainWindow::readData);
    connect(mTcpClient.get(), static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error), this, &MainWindow::readError);
    init();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::readData() {
    QByteArray buffer =  mTcpClient->readAll();
    if (!buffer.isEmpty()) {
        ui->recvData->append(buffer);
    }
}

void MainWindow::readError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error)
    mTcpClient->disconnectFromHost();
    ui->btnConnect->setText(tr("连接"));
    QMessageBox msgBox;
    msgBox.setText(tr("failed to connect server because %1").arg(mTcpClient->errorString()));
    msgBox.exec();
}

void MainWindow::btnClear() {
    ui->recvData->clear();
}

void MainWindow::btnConnect() {
    if (ui->btnConnect->text() == tr("连接")) {
        mTcpClient->connectToHost(ui->IPSet->text(), ui->portSet->text().toInt());
        if (mTcpClient->waitForConnected(1000)) {
            ui->btnSend->setEnabled(true);
            ui->btnConnect->setText(tr("断开"));
        }
    } else {
        mTcpClient->disconnectFromHost();
        if (mTcpClient->state() == QAbstractSocket::UnconnectedState || mTcpClient->waitForDisconnected(1000)) {
            ui->btnConnect->setText(tr("连接"));
            ui->btnSend->setEnabled(true);
        }
    }
}

void MainWindow::btnSend() {
    QString data = ui->sendData->toPlainText();
    if (!data.isEmpty()) {
        mTcpClient->write(data.toLatin1());
    }
}

void MainWindow::init() {
    ui->IPSet->setText(QNetworkInterface::allAddresses().at(2).toString());
    ui->portSet->setText("8080");
    connect(ui->btnConnect, &QPushButton::clicked, this, &MainWindow::btnConnect);
    connect(ui->btnClear, &QPushButton::clicked, this, &MainWindow::btnClear);
    connect(ui->btnSend, &QPushButton::clicked, this, &MainWindow::btnSend);
}
