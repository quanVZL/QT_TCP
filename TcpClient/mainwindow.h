#pragma once

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <memory>
#include <QDebug>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void readData();
    void readError(QAbstractSocket::SocketError error);
    void btnConnect();
    void btnClear();
    void btnSend();

private:
    void init();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<QTcpSocket> mTcpClient = nullptr;
};
