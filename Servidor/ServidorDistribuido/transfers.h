#ifndef TRANSFERS_H
#define TRANSFERS_H

#include <QMainWindow>

namespace Ui {
class transfers;
}

class transfers : public QMainWindow
{
    Q_OBJECT

public:
    explicit transfers(QWidget *parent = 0);
    ~transfers();
    void setup();
    void UpdateInfo(QList<QString> value_);
    void updateDataCount(int contador_, int size);

private slots:
    void on_pushButton_clicked();

private:
    Ui::transfers *ui;
signals:
    void dc();

};

#endif // TRANSFERS_H
