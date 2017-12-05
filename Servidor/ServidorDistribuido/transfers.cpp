#include "transfers.h"
#include "ui_transfers.h"

transfers::transfers(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::transfers)
{

    ui->setupUi(this);
    setup();
}

transfers::~transfers()
{
    delete ui;
}

void transfers::setup()
{
    ui->progressBar->setRange(0,100);
    ui->progressBar->setValue(0);
}


void transfers::UpdateInfo(QList<QString> value_)
{
    ui->lineEdit_5->setText(value_.at(1));
    ui->lineEdit_3->setText(value_.at(0));
}

void transfers::updateDataCount(int contador_, int size)
{
    int c = contador_ / size;
    ui->progressBar->setValue(c * 100);
}



void transfers::on_pushButton_clicked()
{
    this->close();
    emit dc();

}
