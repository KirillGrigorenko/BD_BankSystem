#include "userkredit.h"
#include "ui_userkredit.h"
#include "user.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <string>


userkredit::userkredit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userkredit)
{
    ui->setupUi(this);
    connect(ui->btnConnect, SIGNAL(clicked(bool)), this, SLOT(dbconnect()));
    connect(ui->btnSelectAll, SIGNAL(clicked(bool)), this, SLOT(selectAll()));
    connect(ui->twKredit, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onTableItemClicked(QTableWidgetItem*)));

    ui->btnConnect->setVisible(false);

    // Количество столбцов
    ui->twKredit->setColumnCount(5);
    // Возможность прокрутки
    ui->twKredit->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twKredit->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twKredit->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы

    ui->twKredit->setHorizontalHeaderItem(0,new QTableWidgetItem("Кредит"));
    ui->twKredit->setHorizontalHeaderItem(1,new QTableWidgetItem("Название"));
    ui->twKredit->setHorizontalHeaderItem(2,new QTableWidgetItem("Страна"));
    ui->twKredit->setHorizontalHeaderItem(3,new QTableWidgetItem("Ставка"));
    ui->twKredit->setHorizontalHeaderItem(4,new QTableWidgetItem("Срок"));
    ui->twKredit->setColumnWidth(0, 15);
    ui->twKredit->setColumnWidth(1, 200);
    ui->twKredit->setColumnWidth(2, 75);
    ui->twKredit->setColumnWidth(3, 100);
    ui->twKredit->setColumnWidth(4, 50);
    // Последний столбец растягивается при изменении размера формы
    ui->twKredit->horizontalHeader()->setStretchLastSection(true);
    // Разрешаем сортировку пользователю
    ui->twKredit->setSortingEnabled(true);
    ui->twKredit->sortByColumn(0);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twKredit->setEditTriggers(QAbstractItemView::NoEditTriggers);

    selectAll();
}

void userkredit::dbconnect()
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

void userkredit::selectAll()
{
    ui->twKredit->clearContents();

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
       QString sqlstr = R"(
           SELECT
               k.IDKredit,
               k.Nazv,
               k.Country,
               k.Loan,
               k.Term
           FROM
               KredityKlienta kk
           JOIN
               Kredit k ON kk.IDKredit = k.IDKredit
           WHERE
               kk.IDClient = ?
       )";
       query.prepare(sqlstr);
       query.bindValue(0, clientID);

       if (!query.exec())
       {
           QMessageBox::critical(this, "Ошибка", query.lastError().text());
           return;
       }

       if (query.isActive())
           ui->twKredit->setRowCount(query.size());
       else
           ui->twKredit->setRowCount(0);

    int i = 0;
    while (query.next())
    {

        ui->twKredit->setItem(i, 0, new QTableWidgetItem(query.value("IDKredit").toString()));
        ui->twKredit->setItem(i, 1, new QTableWidgetItem(query.value("Nazv").toString()));
        ui->twKredit->setItem(i, 2, new QTableWidgetItem(query.value("Country").toString()));
        ui->twKredit->setItem(i, 3, new QTableWidgetItem(query.value("Loan").toString()));
        ui->twKredit->setItem(i, 4, new QTableWidgetItem(query.value("Term").toString()));i++;
    }

    for (int row = 0; row < ui->twKredit->rowCount(); ++row)
    {
        ui->twKredit->setRowHidden(row, false);
    }
}

void userkredit::onTableItemClicked(QTableWidgetItem *item)
{
    if (item == nullptr)
    {
        return; // Защита от нулевого указателя
    }

    // Получаем индекс строки, которая была выбрана
    int row = item->row();

    // Получаем данные из выбранной строки
    QString krid = ui->twKredit->item(row, 0)->text();
    QString nazv = ui->twKredit->item(row, 1)->text();
    QString co = ui->twKredit->item(row, 2)->text();
    QString lo = ui->twKredit->item(row, 3)->text();
    QString te = ui->twKredit->item(row, 4)->text();

    // Отображаем данные в соответствующих Line Edit
    ui->leID->setText(krid);
    ui->leNazv->setText(nazv);
    ui->leCountry->setText(co);
    ui->leLoan->setText(lo);
    ui->leTerm->setText(te);
}


void userkredit::on_btnExit_clicked()
{
    close();
    user *usr = new user(this);
    usr->setModal(true);
    usr->setClientID(clientID);
    usr->exec();
}

void userkredit::setClientID(int id)
{
    clientID = id;
    selectAll();
}


userkredit::~userkredit()
{
    if (dbconn.isOpen())
        dbconn.close();
    delete ui;
}
