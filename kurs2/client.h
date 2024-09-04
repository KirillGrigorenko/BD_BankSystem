#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QTableWidgetItem>
#include "admin.h"
#include <QSqlDatabase>

namespace Ui {
class client;
}

class client : public QDialog
{
    Q_OBJECT

public:
    explicit client(QWidget *parent = nullptr);
    ~client();
private slots:
    void on_btnEXit_clicked();
    void dbconnect();
    void add();
    void del();
    void edit();
    void onTableItemClicked(QTableWidgetItem *item);
    void on_btnSearch_clicked();
    void loadClientKreditData();
    void loadClientVkladData();
    void on_btnCLientKredit_clicked();
    void on_btnClientVklad_clicked();
    void getClientIDByFIO(const QString &fio);
    void selectAll();
    void on_btnSchet_clicked();

private:
    Ui::client *ui;
    QSqlDatabase dbconn;
    int clientID;

};

#endif // CLIENT_H
