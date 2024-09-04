#ifndef USERVKLAD_H
#define USERVKLAD_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QSqlDatabase>

namespace Ui {
class uservklad;
}

class uservklad : public QDialog
{
    Q_OBJECT

public:
    explicit uservklad(QWidget *parent = nullptr);
    ~uservklad();
    void setClientID(int id); // Метод для установки ID клиента


private slots:
    void on_btnExit_clicked();
    void onTableItemClicked(QTableWidgetItem *item);
    void dbconnect();
    void selectAll();

private:
    Ui::uservklad *ui;
    QSqlDatabase dbconn;
    int clientID; // Добавляем переменную clientID
};

#endif // USERVKLAD_H
