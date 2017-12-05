#include "connectedclient.h"

ConnectedClient::ConnectedClient():
mySocket(),
dirIP(),
port(),
last_file_size(0),
informacion(false),
contador(0){
}

ConnectedClient::ConnectedClient(QTcpSocket* inqueue):
mySocket(inqueue),
  dirIP(),
  port(),
  last_file_size(0),
  informacion(false),
  contador(0)
{

}

QTcpSocket *ConnectedClient::getSocket(void){
    return mySocket;
}

void ConnectedClient::send2User(QByteArray &data)
{
    mySocket->write(data);
}

void ConnectedClient::setIP(QHostAddress ip)
{
    dirIP = ip;
}

QHostAddress ConnectedClient::getIP()
{
    return dirIP;
}

void ConnectedClient::setPort(quint16 port_)
{
    port = port_;
}

quint16 ConnectedClient::getPort()
{
    return port;
}

void ConnectedClient::set_last_file_size(int size)
{
    last_file_size = size;
}

int ConnectedClient::get_last_file_size()
{
    return last_file_size;
}

bool ConnectedClient::get_info(){
    return informacion;
}

void ConnectedClient::set_info(bool ax){
    informacion = ax;
}


int ConnectedClient::get_contador(){
    return contador;
}

void ConnectedClient::inc_contador(int inc){
    contador = contador + inc;
}

void ConnectedClient::reset_contador(){
    contador = 0;
}

//void ConnectedClient::SetUsers(qint16 users)
//{
//    NUsers = users;
//}

//qint16 ConnectedClient::GetUsers()
//{
//    return NUsers;
//}
