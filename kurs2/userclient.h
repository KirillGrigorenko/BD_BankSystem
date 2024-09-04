#ifndef USERCLIENT_H
#define USERCLIENT_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QSqlDatabase>

namespace Ui {
class UserCLient;
}

class UserCLient : public QDialog
{
    Q_OBJECT

public:
    explicit UserCLient(QWidget *parent = nullptr);
    ~UserCLient();

    void setClientID(int id); // Метод для установки ID клиента

private slots:
    void on_btnExit_clicked();
    void on_btnEdit_clicked();
    void onTableItemClicked(QTableWidgetItem *item);
    void dbconnect();
    void selectAll();

private:
    Ui::UserCLient *ui;
    QSqlDatabase dbconn;
    int clientID; // Добавляем переменную clientID
};

#endif // USERCLIENT_H
