#include "clientewindow.h"
#include "ui_clientewindow.h"


ClienteWindow::ClienteWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClienteWindow),
    origen(false),
    yaenviado(false)
{
    ui->setupUi(this);
    ui->label_3->setDisabled(true);
    ui->pushButton_2->setDisabled(true);
    ui->lineCompartir->setDisabled(true);
    ui->nUsers->setDisabled(true);
    ui->nUsers->setValidator( new QIntValidator(0, 30, this) );
    ui->linePort->setValidator( new QIntValidator(0, 65535, this) );
    QRegExp rx("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?).){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    ui->lineDir->setValidator(new QRegExpValidator(rx));
    ui->linePort->setText("1024");
    ui->lineDir->setText("127.0.0.1");
    last_file = new QFile();
    reset_contador_datos_leidos();
    informacion = false;

    connect(this, &ClienteWindow::startProgress, this, &ClienteWindow::showProgress);
    connect(&uip,&transfers::dc,this,&ClienteWindow::disconnectNow);
}

ClienteWindow::~ClienteWindow()
{
    delete ui;
}

void ClienteWindow::Msg2Client()
{
    int enviados = 0;
    int recibidos = 0;
    qDebug() << origen;
    while(tcpSocket->canReadLine()){
          QByteArray msgtype;

          if(get_info())
              msgtype = "";
          else
              msgtype = tcpSocket->readLine();

          qDebug() << msgtype;


      if(msgtype == "Estas conectado\n"){
          qDebug() << "Recibido - Estas conectado ";

          if(origen){
              QByteArray can_start_transfer("Puedo enviar?\n");
              can_start_transfer.append(QByteArray().setNum(users));
              tcpSocket->write(can_start_transfer.append("\n"));
          }

      }else if  (msgtype == "Puedes enviar\n"){
          qDebug() << "Enviando";
          QDirIterator it(directory, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Hidden, QDirIterator::Subdirectories);
          while(it.hasNext()){

              QFileInfo tmp = it.next();
              if(tmp.isDir()){
                  QString mensaje("Directorio\n");
                  int tmp_dir_size = get_directory().size();
                  QString tmp_where_to_create = tmp.absoluteFilePath().remove(0,tmp_dir_size);
                  mensaje.append(tmp_where_to_create);
                  mensaje.append("\n");
                  QByteArray mensaje_qbyte;
                  mensaje_qbyte.append(mensaje);
                  tcpSocket->write(mensaje_qbyte);
              }else{
                  QString name(tmp.absoluteFilePath());
                  QFile tmp_fichero(name);
                  if(tmp_fichero.open(QIODevice::ReadOnly)){

                      QByteArray mensaje("Fichero informacion\n");
                      QString size_file(QByteArray().setNum(tmp_fichero.size()));

                      int tmp_file_size = get_directory().size();
                      QString tmp_full = tmp.absoluteFilePath();
                      QString tmp_where_to_create = tmp_full.remove(0,tmp_file_size);
                      mensaje.append(tmp_where_to_create);
                      mensaje.append("//");
                      mensaje.append(size_file);
                      mensaje.append("\n");

                      qDebug() << "fichero info";
                      tcpSocket->write(mensaje);

                      while(tmp_fichero.pos() < tmp_fichero.size()){
                            QByteArray datos_leidos;
                            datos_leidos = tmp_fichero.read(4194304);

                            if(datos_leidos.size() <= 1024){
                                tcpSocket->write(datos_leidos);
                                enviados++;
                                qDebug() << "Paquete enviado de  <= 1024";

                            }else{

                                QByteArray datos_enviados;
                                int contador=0;
                                qDebug() << datos_leidos.size();
                                qDebug() << tmp_fichero.size();
                                while(contador <= datos_leidos.size()){
                                    datos_enviados = datos_leidos;
                                    datos_enviados = datos_enviados.remove(contador+1024,datos_enviados.size());
                                    datos_enviados = datos_enviados.remove(0, contador);
                                    contador += 1024;
                                    tcpSocket->write(datos_enviados);
                                    enviados++;
                                    qDebug() << "Paquete enviado de > 1024";
                                }
                            }
                      }
                  }
                  tmp_fichero.close();
                  qDebug() << enviados;
              }
          }

      set_yaenviado(true);
      }else if(msgtype == "Nuevo cliente conectado\n"){
          qDebug() << "New Client";
          if(!get_yaenviado() && origen){
//              if(last_file != NULL){
//                  delete last_file;
//                  last_file = NULL;
//              }
              QByteArray can_start_transfer("Puedo enviar?\n");
              can_start_transfer.append(QByteArray().setNum(users));
              tcpSocket->write(can_start_transfer.append("\n"));
          }
      }else if(msgtype == "Espera para enviar\n"){
          qDebug() << "Esperando a nuevo cliente";
        //Se queda esperando el cliente
      }else if(msgtype == "Directorio\n"){
          qDebug() << "Informacion de directorio";
//                  if(last_file->exists()){
//                      delete last_file;
//                      last_file = NULL;
//                  }

                QString dirpath(tcpSocket->readLine());
                qDebug() <<  "dirparth ->" +dirpath;
                dirpath.chop(1);
                dirpath.prepend(get_directory());
                QDir directory_to_create(dirpath);
                directory_to_create.mkpath(dirpath);

      }else if(msgtype == "Fichero informacion\n"){
          qDebug() << "Informacion de fichero";

              QString tmpread;
              tmpread = tcpSocket->readLine();
              qDebug() << tmpread;
              QList<QString> list_s;
              list_s = tmpread.split("//");
              QString tam_aux =list_s.value(1);
              tam_aux.chop(1);
              set_filesize(tam_aux.toInt());
              QString tmp_path = list_s.at(0);
              tmp_path.prepend(get_directory());
              reset_contador_datos_leidos();
              changeInfo(list_s);
              qApp->processEvents();

                  if(last_file->exists()){
                      delete last_file;
                      last_file = NULL;
                  }

              last_file =new QFile(tmp_path); //en cualquier otro tipo de mensaje, si no es nulo, destruirlo y ponerlo a nulo
              last_file->open(QIODevice::WriteOnly);
              set_info(true);

            }else{

                  QByteArray data;

                  //if(filesize - contador_datos_leidos > 1024)
                      //data = tcpSocket->read(1024);
                  //else
                      data = tcpSocket->read(get_filesize() - get_contador_datos_leidos());

                  last_file->write(data);
                  inc_contador_datos_leidos(data.size());
                  dataCount(get_contador_datos_leidos(),get_filesize());
                  qApp->processEvents();
                  qDebug() << get_contador_datos_leidos();
                  qDebug() << get_filesize();

                  if(get_filesize() == get_contador_datos_leidos()){
                      set_info(false);
                  }
                  data.clear();

                }
          }

    if(recibidos != 0)
        qDebug() << recibidos;

}

void ClienteWindow::changeInfo(QList<QString> value_)
{
    uip.UpdateInfo(value_);
}

void ClienteWindow::dataCount(int newcount_, int size)
{
    uip.updateDataCount(newcount_, size);
}

void ClienteWindow::showProgress()
{
    this->setEnabled(false);
    uip.show();
}

void ClienteWindow::disconnectNow()
{
    this->setEnabled(true);
    tcpSocket->disconnect();
    set_info(false);
    set_yaenviado(false);
    set_filesize(0);
    reset_contador_datos_leidos();
    set_directory(NULL);

}



/*
      - TCP
      - Protocolo texto plano
      - Protocolo propio
      - Basado en tipos de mensajes : Connect, Begin Data, Middle Data, End Data, Disconnect
      - Cliente origen = 0 / destino = 1
      - Tamaño fijo de paquete : ???
      - Delimitadores : Saltos de línea

      Ejemplo:

      Connect   //Tipo de mensaje
      54100     //Puerto de conexión
      0         //Tipo cliente
      DATA BODY // Ruta de origen o destino

      Creame un dir:

      Directorio
      Ruta de directorio - remove del tamaño del lugar
*/

void ClienteWindow::on_pushButton_clicked()
{
    if( (ui->lineDir->text().size() != 0 && ui->linePort->text().size() != 0 && ui->lineCompartir->text().size() != 0 && ui->nUsers->text().size() != 0)
         || (ui->lineDir->text().size() != 0 && ui->linePort->text().size() != 0 && ui->lineDestino->text().size() != 0)){
    tcpSocket = new QTcpSocket(this);
    tcpSocket->connectToHost(ui->lineDir->text(), ui->linePort->text().toInt());
    connect(tcpSocket, &QTcpSocket::readyRead, this, &ClienteWindow::Msg2Client);

    ip = ui->lineDir->text();
    port = ui->linePort->text().toInt();

    if(ui->lineCompartir->text().size() != 0){
        set_directory(ui->lineCompartir->text());
        users = ui->nUsers->text().toInt();
        origen = true;
    }else{
        set_directory(ui->lineDestino->text());
        origen = false;
    }
    qDebug() << "Envio de mensaje de conexion";

    if(!origen){
       emit startProgress();
    }else if(ui->pushButton->text() == "Conectar"){
        ui->pushButton->setText("Desconectar");
    }else{
        ui->pushButton->setText("Conectar");
    }



    }

}

void ClienteWindow::on_pushButton_2_clicked()
{
   QString tmp(QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
   if(tmp != ""){
    set_directory(tmp);
    ui->lineCompartir->setText(tmp);
    ui->lineDestino->setText("");
   }
}

void ClienteWindow::set_directory(QString dir)
{
    directory = dir;
}

QString ClienteWindow::get_directory(void)
{
    return directory;
}

void ClienteWindow::set_info(bool ax)
{
    informacion = ax;
}

bool ClienteWindow::get_info()
{
    return  informacion;
}

void ClienteWindow::set_yaenviado(bool ax)
{
    yaenviado = ax;
}

bool ClienteWindow::get_yaenviado()
{
    return yaenviado;
}

void ClienteWindow::set_filesize(qint64 ax)
{
    filesize = ax;
}

qint64 ClienteWindow::get_filesize()
{
    return  filesize;
}

void ClienteWindow::reset_contador_datos_leidos()
{
    contador_datos_leidos = 0;
}

int ClienteWindow::get_contador_datos_leidos()
{
    return contador_datos_leidos;
}

void ClienteWindow::inc_contador_datos_leidos(int inc)
{
    contador_datos_leidos += inc;
}

void ClienteWindow::on_checkOrigen_clicked()
{
    if(ui->checkOrigen->isChecked()){
        ui->lineDestino->setDisabled(true);
        ui->pushDestino->setDisabled(true);
        ui->label_destino->setDisabled(true);
        ui->label_3->setDisabled(false);
        ui->pushButton_2->setDisabled(false);
        ui->lineCompartir->setDisabled(false);
        ui->nUsers->setDisabled(false);
        ui->lineDestino->setText("");
    }else{
        ui->lineDestino->setDisabled(false);
        ui->pushDestino->setDisabled(false);
        ui->label_destino->setDisabled(false);
        ui->label_3->setDisabled(true);
        ui->pushButton_2->setDisabled(true);
        ui->lineCompartir->setDisabled(true);
        ui->nUsers->setDisabled(true);

        ui->lineCompartir->setText("");
    }
}

void ClienteWindow::on_pushDestino_clicked()
{
    QString tmp(QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
    if(tmp != ""){
     set_directory(tmp);
     ui->lineDestino->setText(tmp);
     ui->lineCompartir->setText("");
    }
}

/*
 *Siempre se recibe menos en el cliente
 *No puedo enviar varios archivos
 *El envío tras volver a comprobar los usuarios en línea, esta descolocado
 *
 *
 * Puedo enviar?\n
 * n_usuarios\n
 *
 * Información del fichero:
 *
 * Fichero direccion\n
 * file_size\n
 * file_path\n
 *
 * Trozos del fichero:
 *
 * Fichero datos\n
 * cantidad datos enviados\n
 * contenido fichero
*/
