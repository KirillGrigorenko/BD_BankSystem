#include "uservklad.h"
#include "ui_uservklad.h"
#include "user.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <string>

uservklad::uservklad(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uservklad)
{
    ui->setupUi(this);
    connect(ui->btnConnect, SIGNAL(clicked(bool)), this, SLOT(dbconnect()));
    connect(ui->btnSelectAll, SIGNAL(clicked(bool)), this, SLOT(selectAll()));
    connect(ui->twVklad, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onTableItemClicked(QTableWidgetItem*)));

    ui->btnConnect->setVisible(false);

    // Количество столбцов
    ui->twVklad->setColumnCount(5);
    // Возможность прокрутки
    ui->twVklad->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twVklad->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twVklad->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twVklad->setHorizontalHeaderItem(0,new QTableWidgetItem("Вклад"));
    ui->twVklad->setHorizontalHeaderItem(1,new QTableWidgetItem("Название"));
    ui->twVklad->setHorizontalHeaderItem(2,new QTableWidgetItem("Страна"));
    ui->twVklad->setHorizontalHeaderItem(3,new QTableWidgetItem("Ставка"));
    ui->twVklad->setHorizontalHeaderItem(4,new QTableWidgetItem("Срок"));
    ui->twVklad->setColumnWidth(0, 15);
    ui->twVklad->setColumnWidth(1, 200);
    ui->twVklad->setColumnWidth(2, 75);
    ui->twVklad->setColumnWidth(3, 100);
    ui->twVklad->setColumnWidth(4, 50);
    // Последний столбец растягивается при изменении размера формы
    ui->twVklad->horizontalHeader()->setStretchLastSection(true);
    // Разрешаем сортировку пользователю
    ui->twVklad->setSortingEnabled(true);
    ui->twVklad->sortByColumn(0);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twVklad->setEditTriggers(QAbstractItemView::NoEditTriggers);

    selectAll();
}

void uservklad::dbconnect()
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
void uservklad::selectAll()
{
    ui->twVklad->clearContents();

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
               k.IDVclad,
               k.Nazv,
               k.Country,
               k.Loan,
               k.Term
           FROM
               VkladiKlienta kk
           JOIN
               Vklad k ON kk.IDVclad = k.IDVclad
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
           ui->twVklad->setRowCount(query.size());
       else
           ui->twVklad->setRowCount(0);

    int i = 0;
    while (query.next())
    {

        ui->twVklad->setItem(i, 0, new QTableWidgetItem(query.value("IDVclad").toString()));
        ui->twVklad->setItem(i, 1, new QTableWidgetItem(query.value("Nazv").toString()));
        ui->twVklad->setItem(i, 2, new QTableWidgetItem(query.value("Country").toString()));
        ui->twVklad->setItem(i, 3, new QTableWidgetItem(query.value("Loan").toString()));
        ui->twVklad->setItem(i, 4, new QTableWidgetItem(query.value("Term").toString()));i++;
    }

    for (int row = 0; row < ui->twVklad->rowCount(); ++row)
    {
        ui->twVklad->setRowHidden(row, false);
    }
}

void uservklad::onTableItemClicked(QTableWidgetItem *item)
{
    if (item == nullptr)
    {
        return; // Защита от нулевого указателя
    }

    // Получаем индекс строки, которая была выбрана
    int row = item->row();

    // Получаем данные из выбранной строки
    QString krid = ui->twVklad->item(row, 0)->text();
    QString nazv = ui->twVklad->item(row, 1)->text();
    QString co = ui->twVklad->item(row, 2)->text();
    QString lo = ui->twVklad->item(row, 3)->text();
    QString te = ui->twVklad->item(row, 4)->text();

    // Отображаем данные в соответствующих Line Edit
    ui->leID->setText(krid);
    ui->leNazv->setText(nazv);
    ui->leCountry->setText(co);
    ui->leLoan->setText(lo);
    ui->leTerm->setText(te);
}


void uservklad::on_btnExit_clicked()
{
    close();
    user *usr = new user(this);
    usr->setModal(true);
    usr->setClientID(clientID);
    usr->exec();
}

void uservklad::setClientID(int id)
{
    clientID = id;
    selectAll();
}


uservklad::~uservklad()
{
    if (dbconn.isOpen())
        dbconn.close();
    delete ui;
}
