#include "servidorwindow.h"
#include "transfers.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServidorWindow w;

    w.show();

    return a.exec();
}
