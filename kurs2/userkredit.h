#ifndef USERKREDIT_H
#define USERKREDIT_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QSqlDatabase>


namespace Ui {
class userkredit;
}

class userkredit : public QDialog
{
    Q_OBJECT

public:
    explicit userkredit(QWidget *parent = nullptr);
    ~userkredit();
    void setClientID(int id); // Метод для установки ID клиента


private slots:
    void on_btnExit_clicked();
    void onTableItemClicked(QTableWidgetItem *item);
    void dbconnect();
    void selectAll();
private:
    Ui::userkredit *ui;
    QSqlDatabase dbconn;
    int clientID; // Добавляем переменную clientID
};

#endif // USERKREDIT_H
