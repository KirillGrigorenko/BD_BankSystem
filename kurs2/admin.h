#ifndef ADMIN_H
#define ADMIN_H

#include <QDialog>

#include "mainwindow.h"

namespace Ui {
class admin;
}

class admin : public QDialog
{
    Q_OBJECT

public:
    explicit admin(QWidget *parent = nullptr);
    ~admin();

private slots:
    void on_btnExit_clicked();
    void on_btnClient_clicked();
    void on_btnVklad_clicked();
    void on_btnSchet_clicked();
    void on_btnKredit_clicked();

private:
    Ui::admin *ui;
    QMainWindow *mainWindow;
};

#endif // ADMIN_H
