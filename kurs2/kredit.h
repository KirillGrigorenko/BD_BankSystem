#ifndef KREDIT_H
#define KREDIT_H

#include <QDialog>
#include <QTableWidgetItem>
#include "admin.h"
#include "client.h"
#include <QSqlDatabase>

namespace Ui {
class Kredit;
}

class Kredit : public QDialog
{
    Q_OBJECT

public:
    explicit Kredit(QWidget *parent = nullptr);
    ~Kredit();

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
    Ui::Kredit *ui;
    QSqlDatabase dbconn;
};

#endif // KREDIT_H
