#-------------------------------------------------
#
# Project created by QtCreator 2017-04-18T09:42:30
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ClienteDistribuido
TEMPLATE = app


SOURCES += main.cpp\
        clientewindow.cpp \
    ../../Servidor/ServidorDistribuido/transfers.cpp

HEADERS  += clientewindow.h \
    ../../Servidor/ServidorDistribuido/transfers.h

FORMS    += clientewindow.ui \
    ../../Servidor/ServidorDistribuido/transfers.ui
