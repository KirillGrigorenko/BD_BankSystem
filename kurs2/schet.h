#ifndef SCHET_H
#define SCHET_H

#include <QDialog>
#include <QTableWidgetItem>
#include "admin.h"
#include <QSqlDatabase>

namespace Ui {
class Schet;
}

class Schet : public QDialog
{
    Q_OBJECT

private slots:
    void dbconnect();
    void selectAll();
    void add();
    void del();
    void edit();
    void onTableItemClicked(QTableWidgetItem *item);
    void SecondTable();

    void on_btnExit_clicked();
    void on_btnSearch_clicked();

public:
    explicit Schet(QWidget *parent = nullptr);
    ~Schet();
    void loadClientData(int idClient);

private:
    Ui::Schet *ui;
    QSqlDatabase dbconn;
    int clientID;
};

#endif // SCHET_H
