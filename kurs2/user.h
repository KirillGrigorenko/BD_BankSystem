#ifndef USER_H
#define USER_H

#include <QDialog>
#include <QTableWidgetItem>
#include "mainwindow.h"
#include <QSqlDatabase>

namespace Ui {
class user;
}

class user : public QDialog
{
    Q_OBJECT

public:
    explicit user(QWidget *parent = nullptr);
    ~user();

    void setClientID(int id); // Метод для установки ID клиента

private slots:

    void on_btnExit_clicked();

    void on_btnClient_clicked();

    void on_btnUserKredit_clicked();

    void on_btnUserVklad_clicked();

private:
    Ui::user *ui;
    QSqlDatabase dbconn;
     int clientID;
};

#endif // USER_H
