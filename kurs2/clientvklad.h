#ifndef CLIENTVKLAD_H
#define CLIENTVKLAD_H

#include <QDialog>

#include <QTableWidgetItem>
#include "admin.h"
#include "client.h"
#include <QSqlDatabase>

namespace Ui {
class clientvklad;
}

class clientvklad : public QDialog
{
    Q_OBJECT

public:
    explicit clientvklad(QWidget *parent = nullptr);
    ~clientvklad();
    void loadClientData(int idClient);

private slots:
    void on_btnExit_clicked();
    void dbconnect();
    void selectAll();
    void add();
    void del();
    void edit();
    void onTableItemClicked(QTableWidgetItem *item);
    void on_btnSearch_clicked();

private:
    Ui::clientvklad *ui;
    QSqlDatabase dbconn;
    int clientID;
};

#endif // CLIENTVKLAD_H
