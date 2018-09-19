#pragma once

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <memory>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void btnConnectClick();
    void btnSendClick();
    void btnClearClick();
    void readData();
    void newConnectionClient();
    void disconnectClient();

private:
    void initUI();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<QTcpServer> mTcpServer = nullptr;
    QList<QTcpSocket*> mTcpClientList;
    QTcpSocket *currentClient;
};


