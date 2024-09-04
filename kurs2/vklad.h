#ifndef VKLAD_H
#define VKLAD_H

#include <QDialog>
#include <QTableWidgetItem>
#include "admin.h"
#include <QSqlDatabase>

namespace Ui {
class Vklad;
}

class Vklad : public QDialog
{
    Q_OBJECT

public:
    explicit Vklad(QWidget *parent = nullptr);
    ~Vklad();
private slots:
    void dbconnect();
    void add();
    void del();
    void edit();
    void onTableItemClicked(QTableWidgetItem *item);
    void on_btnExit_clicked();
    void selectAll();
    void on_btnSearch_clicked();

private:
    Ui::Vklad *ui;
    QSqlDatabase dbconn;
};

#endif // VKLAD_H
