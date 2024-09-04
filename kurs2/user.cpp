#include "user.h"
#include "ui_user.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <string>
#include "userclient.h"
#include "userkredit.h"
#include "uservklad.h"

user::user(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::user),
    clientID(-1) // Инициализация переменной clientID
{
    ui->setupUi(this);

    connect(ui->btnExit, &QPushButton::clicked, this, &user::on_btnExit_clicked);
    dbconn = QSqlDatabase::addDatabase("QPSQL");
    dbconn.setDatabaseName("kurs");
    dbconn.setHostName("localhost");
    dbconn.setUserName("student");
    dbconn.setPassword("1");


    if (!dbconn.open())
    {
        qDebug() << "Database connection failed:" << dbconn.lastError().text();
    }
    else
    {
        qDebug() << "Database connected successfully!";
    }
}

void user::setClientID(int id)
{
    clientID = id;
    ui->lbClientID->setText(QString("%1").arg(clientID));
}

user::~user()
{
    dbconn.close();
    QSqlDatabase::removeDatabase("my_connection");
    delete ui;
}

void user::on_btnExit_clicked()
{
    close();
    MainWindow *mainWindow = new MainWindow();
    mainWindow->show();
}

void user::on_btnClient_clicked()
{
    close();
    UserCLient uscl;
    uscl.setModal(true);
    uscl.setClientID(clientID);
    uscl.exec();
}

void user::on_btnUserKredit_clicked()
{
    close();
    userkredit uskr;
    uskr.setModal(true);
    uskr.setClientID(clientID);
    uskr.exec();
}

void user::on_btnUserVklad_clicked()
{
    close();
    uservklad usvk;
    usvk.setModal(true);
    usvk.setClientID(clientID);
    usvk.exec();
}
