#ifndef CONNECTEDCLIENT_H
#define CONNECTEDCLIENT_H

#include <QTcpSocket>
#include <QByteArray>
#include <QHostAddress>

class ConnectedClient
{
public:
    ConnectedClient();
    ConnectedClient(QTcpSocket* inqueue);
    QTcpSocket* getSocket(void);
    void send2User(QByteArray &data);
    void setIP(QHostAddress ip);
    QHostAddress getIP(void);
    void setPort(quint16 port_);
    quint16 getPort(void);
    void set_last_file_size(int size);
    int get_last_file_size(void);
    int get_contador();
    void inc_contador(int inc);
    void reset_contador();
    bool get_info();
    void set_info(bool ax);
    //void SetUsers(qint16 users);
    //qint16 GetUsers(void);
private:
    QTcpSocket* mySocket;
    QHostAddress dirIP;
    quint16 port;
    uint nUsers;
    int last_file_size;
    bool informacion;
    int contador;
};

#endif // CONNECTEDCLIENT_H
