#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QTableWidgetItem>
#include "admin.h"
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void clearLoginFields();

private slots:
    void on_btnEntry_clicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase dbconn;

};
#endif // MAINWINDOW_H
