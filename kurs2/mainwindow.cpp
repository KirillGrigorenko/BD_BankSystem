#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "admin.h"
#include "user.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <string>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dbconn=QSqlDatabase::addDatabase("QPSQL");
    dbconn.setDatabaseName("kurs");
    dbconn.setHostName("localhost");
    dbconn.setUserName("student");
    dbconn.setPassword("1");

    if (!dbconn.open()) {
         QMessageBox::critical(this, "Ошибка подключения", dbconn.lastError().text());
         // Обработка ошибки подключения
         return;
     }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearLoginFields()
{
    ui->leLogin->clear();
    ui->lePassword->clear();
}


void MainWindow::on_btnEntry_clicked()
{
    QString login = ui->leLogin ->text();
    QString password = ui->lePassword ->text();

    QSqlQuery query(dbconn);

    if (login == "admin" && password == "1234") {
        QMessageBox::information(this, "Успешно", "Вы вошли как админ");
        hide();
        admin adm(this);
        adm.setModal(true);
        adm.exec();
        return;
    }
    query.prepare("SELECT IDClient FROM Client WHERE Login = :login AND Pass = :password");
    query.bindValue(":login", login);
    query.bindValue(":password", password);

    if (!query.exec()) {
            QMessageBox::warning(this, "Ошибка", "Ошибка при выполнении запроса");
            return;
        }

    if (query.next()) {
        int clientID = query.value(0).toInt(); // Получаем IDClient из результата запроса
        QMessageBox::information(this, "Успешно", "Вы вошли как пользователь");
        hide();
        user usr(this);
        usr.setModal(true);
        usr.setClientID(clientID);
        usr.exec();
        show();
        }else {
            
        QMessageBox::warning(this, "Ошибка", "Неправильный логин или пароль");
            }
}
