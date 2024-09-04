#include "userclient.h"
#include "ui_userclient.h"
#include "user.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <string>

UserCLient::UserCLient(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserCLient)
{
    ui->setupUi(this);

    connect(ui->btnConnect, SIGNAL(clicked(bool)), this, SLOT(dbconnect()));
    connect(ui->btnSelectAll, SIGNAL(clicked(bool)), this, SLOT(selectAll()));
    connect(ui->btnEdit, SIGNAL(clicked(bool)), this, SLOT(edit()));
    connect(ui->twClient, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onTableItemClicked(QTableWidgetItem*)));

     ui->btnConnect->setVisible(false);
     ui->lbClientID->setVisible(false);
     ui->lbID->setVisible(false);
     ui->leID->setVisible(false);

    // Количество столбцов
    ui->twClient->setColumnCount(5);
    // Возможность прокрутки
    ui->twClient->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twClient->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twClient->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twClient->setHorizontalHeaderItem(0, new QTableWidgetItem("ФИО"));
    ui->twClient->setHorizontalHeaderItem(1, new QTableWidgetItem("Телефон"));
    ui->twClient->setHorizontalHeaderItem(2, new QTableWidgetItem("Дата"));
    ui->twClient->setHorizontalHeaderItem(3, new QTableWidgetItem("Пароль"));
    ui->twClient->setHorizontalHeaderItem(4, new QTableWidgetItem("Логин"));
    ui->twClient->setColumnWidth(0, 300);
    ui->twClient->setColumnWidth(1, 150);
    ui->twClient->setColumnWidth(2, 100);
    ui->twClient->setColumnWidth(3, 80);
    ui->twClient->setColumnWidth(4, 50);
    // Последний столбец растягивается при изменении размера формы
    ui->twClient->horizontalHeader()->setStretchLastSection(true);
    // Разрешаем сортировку пользователю
    ui->twClient->setSortingEnabled(true);
    ui->twClient->sortByColumn(0);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twClient->setEditTriggers(QAbstractItemView::NoEditTriggers);

    selectAll();
}

void UserCLient::dbconnect()
{
    if (!dbconn.isOpen())
    {
        qDebug() << "Connecting to database...";

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
}

void UserCLient::selectAll()
{
    ui->twClient->clearContents();

    if (!dbconn.isOpen())
    {
        dbconnect();
        if (!dbconn.isOpen())
        {
            QMessageBox::critical(this, "Ошибка", dbconn.lastError().text());
            return;
        }
    }

    QSqlQuery query(dbconn);
    QString sqlstr = QString("SELECT * FROM Client WHERE IDClient = %1").arg(clientID);
    if (!query.exec(sqlstr))
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    if (query.isActive())
        ui->twClient->setRowCount(query.size());
    else
        ui->twClient->setRowCount(0);

    int i = 0;
    while (query.next())
    {
        ui->twClient->setItem(i, 0, new QTableWidgetItem(query.value("FIO").toString()));
        ui->twClient->setItem(i, 1, new QTableWidgetItem(query.value("Phone").toString()));
        ui->twClient->setItem(i, 2, new QTableWidgetItem(query.value("Birth").toString()));
        ui->twClient->setItem(i, 3, new QTableWidgetItem(query.value("Login").toString()));
        ui->twClient->setItem(i, 4, new QTableWidgetItem(query.value("Pass").toString()));
        i++;
    }

    for (int row = 0; row < ui->twClient->rowCount(); ++row)
    {
        ui->twClient->setRowHidden(row, false);
    }
}

void UserCLient::onTableItemClicked(QTableWidgetItem *item)
{
    if (item == nullptr)
    {
        return; // Защита от нулевого указателя
    }

    // Получаем индекс строки, которая была выбрана
    int row = item->row();

    // Получаем данные из выбранной строки
    QString tit = ui->twClient->item(row, 0)->text();
    QString ci = ui->twClient->item(row, 2)->text();
    QString in = ui->twClient->item(row, 1)->text();
    QString lo = ui->twClient->item(row, 3)->text();
    QString pa = ui->twClient->item(row, 4)->text();

    // Отображаем данные в соответствующих Line Edit
    ui->leFIO->setText(ci);
    ui->leData->setText(tit);
    ui->lePhone->setText(in);
    ui->leLogin->setText(lo);
    ui->lePassword->setText(pa);
}

UserCLient::~UserCLient()
{
    if (dbconn.isOpen())
        dbconn.close();
    delete ui;
}

void UserCLient::on_btnExit_clicked()
{
    close();

    user *usr = new user(this);
    usr->setModal(true);
    usr->setClientID(clientID);
    usr->exec();
}

void UserCLient::setClientID(int id)
{
    clientID = id;
    ui->lbClientID->setText(QString("%1").arg(clientID));
    selectAll();
}

void UserCLient::on_btnEdit_clicked()
{
    if (!dbconn.isOpen())
    {
        dbconnect();
        if (!dbconn.isOpen())
        {
            QMessageBox::critical(this, "Ошибка", dbconn.lastError().text());
            return;
        }
    }


    QSqlQuery query(dbconn);

    QString sqlstr = "UPDATE Client SET Login = ?, Pass = ? WHERE IDClient = ?";
    query.prepare(sqlstr);
    query.bindValue(0, ui->leLogin->text());
    query.bindValue(1, ui->lePassword->text());
    query.bindValue(2, clientID);

    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    // Обновить записи в компоненте таблицы
    selectAll();
}
