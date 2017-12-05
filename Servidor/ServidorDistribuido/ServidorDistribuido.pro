#-------------------------------------------------
#
# Project created by QtCreator 2017-04-18T09:41:32
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ServidorDistribuido
TEMPLATE = app


SOURCES += main.cpp\
        servidorwindow.cpp \
    connectedclient.cpp \
    transfers.cpp

HEADERS  += servidorwindow.h \
    connectedclient.h \
    transfers.h

FORMS    += servidorwindow.ui \
    transfers.ui
