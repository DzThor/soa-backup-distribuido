#ifndef SERVIDORWINDOW_H
#define SERVIDORWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include <QByteArray>
#include <QList>
#include <QVector>

#include "transfers.h"
#include "connectedclient.h"

namespace Ui {
class ServidorWindow;
}

class ServidorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServidorWindow(QWidget *parent = 0);
    ~ServidorWindow();
    int reenviados;


private:
    Ui::ServidorWindow *ui;
    transfers uip;
    QTcpServer* myServer;
    QList<ConnectedClient> myClients;
    QVector<QPair<ConnectedClient,int>>* waitingClients;


public slots:
    void createConnection();
    void Msg2Server();
    void changeInfo(QList<QString> value_);
    void dataCount(int newcount_, int size);
    //void fileTransfered();
    void showProgress();
    void disconnectNow();
private slots:
    void on_pushButton_clicked();

signals:
    void startProgress();
};

#endif // SERVIDORWINDOW_H
