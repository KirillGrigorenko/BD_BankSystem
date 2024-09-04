#ifndef CLIENTKREDIT_H
#define CLIENTKREDIT_H

#include <QDialog>
#include <QTableWidgetItem>
#include "admin.h"
#include <QSqlDatabase>

namespace Ui {
class ClientKredit;
}

class ClientKredit : public QDialog
{
    Q_OBJECT

public:
    explicit ClientKredit(QWidget *parent = nullptr);
    ~ClientKredit();
     void loadClientData(int idClient);

private slots:
    void dbconnect();
    void selectAll();
    void add();
    void del();
    void edit();
    void onTableItemClicked(QTableWidgetItem *item);
    void on_btnExit_clicked();
    void on_btnSearch_clicked();

private:
    Ui::ClientKredit *ui;
    QSqlDatabase dbconn;
     int clientID;

};

#endif // CLIENTKREDIT_H
