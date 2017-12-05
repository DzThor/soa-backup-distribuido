#include "servidorwindow.h"
#include "ui_servidorwindow.h"
#include "transfers.h"
#include "ui_transfers.h"

ServidorWindow::ServidorWindow(QWidget *parent) :
    QMainWindow(parent),
    myClients(),
    ui(new Ui::ServidorWindow),
    uip(new transfers)
{
    waitingClients = new QVector<QPair<ConnectedClient,int>>;
    ui->setupUi(this);

    connect(this, &ServidorWindow::startProgress, this, &ServidorWindow::showProgress);
    connect(&uip,&transfers::dc,this,&ServidorWindow::disconnectNow);


}

ServidorWindow::~ServidorWindow()
{
    delete ui;
}

void ServidorWindow::createConnection()
{
    while(myServer->hasPendingConnections()){
        ConnectedClient newClient(myServer->nextPendingConnection());
//        QString ip = newClient.GetSocket()->localAddress().toString().remove(0,7);
//        qDebug() << ip;

        newClient.setIP(newClient.getSocket()->peerAddress());
        newClient.setPort(newClient.getSocket()->peerPort());
        newClient.reset_contador();
        newClient.set_last_file_size(0);
        myClients.push_back(newClient);

        qDebug() << "Cliente creado";

        QByteArray tmp("Estas conectado\n");
        newClient.send2User(tmp);
        qDebug() << "Enviado - Estas conectado";
        connect(newClient.getSocket(), &QTcpSocket::readyRead, this, &ServidorWindow::Msg2Server);
        //connect(uip, &transfers::disconnect, this, &ServidorWindow::disconnectNow);

        QByteArray announce_new_client("Nuevo cliente conectado\n");

        for (int j=0;j<waitingClients->size();j++){
            if(myClients.size()-1 >= waitingClients->at(j).second){
                waitingClients->takeAt(j).first.getSocket()->write(announce_new_client);
                break;
            }

        }
    }
}


void ServidorWindow::Msg2Server()
{
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());

    int reenviados = 0;

    ConnectedClient* a_;
        for(auto& a: myClients){
            if(socket == a.getSocket()){
                a_ = &a;
            }
        }

    while(socket->canReadLine()){

        QByteArray msg;

        if(a_->get_info())
            msg = "";
        else
            msg = socket->readLine();

        if(msg == "Puedo enviar?\n"){
            QString tmp = socket->readLine();
            tmp.chop(1);
            if(myClients.size()-1 >= tmp.toInt()){
                socket->write(QByteArray("Puedes enviar\n"));
            }else{
                for(int i=0;i<myClients.size();i++){
                    QTcpSocket* tmps = myClients.value(i).getSocket();
                    if(socket == tmps)
                        waitingClients->push_back(QPair<ConnectedClient,int>(tmps,tmp.toInt()));
                }
            }
        }else if(msg == "Directorio\n"){
                qDebug() << "Es un directorio";
                QByteArray dirpath("Directorio\n");
                dirpath.append(socket->readLine());
                for(int i=0; i<myClients.size();i++){
                    ConnectedClient socket_tmp = myClients.at(i);
                    if(socket_tmp.getIP() != socket->peerAddress() || (quint16)socket_tmp.getPort() != socket->peerPort()){
                        socket_tmp.getSocket()->write(dirpath);
                        reenviados++;
                    }
                }
        }else if(msg == "Fichero informacion\n"){
            qDebug() << "Fichero info";
                QByteArray filepath("Fichero informacion\n");
                QString tmpread = socket->readLine();
                QList<QString> list_s;
                list_s = tmpread.split("//");
                filepath.append(tmpread);
                ConnectedClient socket_tmp;
                QTcpSocket* tmps;
                qDebug() << tmpread;
                a_->set_info(true);
                a_->reset_contador();
                QString tam =list_s.value(1);
                int tam_ = tam.toInt();
                a_->set_last_file_size(tam_);

                changeInfo(list_s);
                qApp->processEvents();

                for(int i=0; i<myClients.size();i++){
                    socket_tmp = myClients.at(i);
                    tmps = myClients.value(i).getSocket();

                    if(socket_tmp.getIP() != socket->peerAddress() || (quint16)socket_tmp.getPort() != socket->peerPort()){
                        socket_tmp.getSocket()->write(filepath);
                        reenviados++;
                    }
                }



        }else{

                QByteArray data;

                if(a_->get_last_file_size() - a_->get_contador() > 1024)
                    data = socket->read(1024);
                else
                    data = socket->read(a_->get_last_file_size() - a_->get_contador());

                a_->inc_contador(data.size());
                qDebug() << data.size();
                qDebug() << a_->get_contador();
                qDebug() << a_->get_last_file_size();

                dataCount(a_->get_contador(), a_->get_last_file_size());
                qApp->processEvents();

                for(int i=0; i<myClients.size();i++){
                    ConnectedClient socket_tmp = myClients.at(i);
                        if(socket_tmp.getIP() != socket->peerAddress() || (quint16)socket_tmp.getPort() != socket->peerPort()){
                            socket_tmp.getSocket()->write(data);
                            reenviados++;
                        }
                }



                if(a_->get_contador() >= a_->get_last_file_size()){
                    a_->set_info(false);
                }

    }
    }
}

void ServidorWindow::changeInfo(QList<QString> value_)
{
    uip.UpdateInfo(value_);
}

void ServidorWindow::dataCount(int contador_, int size)
{
    uip.updateDataCount(contador_, size);
}

//void ServidorWindow::fileTransfered()
//{

//}

void ServidorWindow::showProgress()
{
    this->setEnabled(false);
    uip.show();

}

void ServidorWindow::disconnectNow()
{
    this->setEnabled(true);

        qDebug() << "Stopped listening";
        ui->pushButton->setEnabled(true);
        ui->lineEdit->setEnabled(true);
        myServer->disconnect();
        myClients.clear();
}

void ServidorWindow::on_pushButton_clicked()
{
    myServer = new QTcpServer(this);
    myClients = QList<ConnectedClient>();

    if(myServer->listen(QHostAddress::Any,ui->lineEdit->text().toUInt())){
        qDebug() << "Success at listenning";
        ui->pushButton->setDisabled(true);
        ui->lineEdit->setDisabled(true);
    }else{
        qDebug() << "Failure at listenning";
    }

    connect(myServer, &QTcpServer::newConnection, this, &ServidorWindow::createConnection);
    emit showProgress();

}
