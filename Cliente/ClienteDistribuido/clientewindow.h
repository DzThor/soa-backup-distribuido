#ifndef CLIENTEWINDOW_H
#define CLIENTEWINDOW_H

#include <QMainWindow>

#include <QTcpSocket>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include "../../Servidor/ServidorDistribuido/transfers.h"

#include "ui_transfers.h"



namespace Ui {
class ClienteWindow;
}

class ClienteWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClienteWindow(QWidget *parent = 0);
    ~ClienteWindow();
    void set_directory(QString dir);
    QString get_directory(void);
    void set_info(bool ax);
    bool get_info();
    void set_yaenviado(bool ax);
    bool get_yaenviado();
    void set_filesize(qint64 ax);
    qint64 get_filesize();
    void reset_contador_datos_leidos();
    int get_contador_datos_leidos();
    void inc_contador_datos_leidos(int inc);

    void changeInfo(QList<QString> value_);
    void dataCount(int newcount_, int size);
    void showProgress();
    void disconnectNow();


    QString ip;
    quint16 port;
    bool origen;
    qint16 users;
    QTcpSocket* tcpSocket;
    QFile* last_file;


private:
    Ui::ClienteWindow *ui;
    transfers uip;
    bool informacion;
    QString directory;
    bool yaenviado;
    qint64 filesize;
    QByteArray datos_pendientes;
    int contador_datos_leidos;

public slots:
    void Msg2Client();
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_checkOrigen_clicked();
    void on_pushDestino_clicked();
signals:
    void startProgress();
};

#endif // CLIENTEWINDOW_H
