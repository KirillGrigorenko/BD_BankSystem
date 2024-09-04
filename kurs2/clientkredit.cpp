#include "clientkredit.h"
#include "ui_clientkredit.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <string>
#include "client.h"

ClientKredit::ClientKredit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClientKredit)
{
    ui->setupUi(this);

    ui->btnConnect->setVisible(false);

    connect(ui->btnConnect,SIGNAL(clicked(bool)),this, SLOT(dbconnect()));
    connect(ui->btnSelectAll,SIGNAL(clicked(bool)),this, SLOT(selectAll()));
    connect(ui->btnAdd,SIGNAL(clicked(bool)),this, SLOT(add()));
    connect(ui->btnDel,SIGNAL(clicked(bool)),this, SLOT(del()));
    connect(ui->btnEdit, SIGNAL(clicked(bool)), this, SLOT(edit()));
    connect(ui->twClientKredit, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onTableItemClicked(QTableWidgetItem*)));
    connect(ui->twSec, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onTableItemClicked(QTableWidgetItem*)));

    // Количество столбцов
    ui->twClientKredit->setColumnCount(5);
    // Возможность прокрутки
    ui->twClientKredit->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twClientKredit->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twClientKredit->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twClientKredit->setHorizontalHeaderItem(0,new QTableWidgetItem("Кредит"));
    ui->twClientKredit->setHorizontalHeaderItem(1,new QTableWidgetItem("Название"));
    ui->twClientKredit->setHorizontalHeaderItem(2,new QTableWidgetItem("Страна"));
    ui->twClientKredit->setHorizontalHeaderItem(3,new QTableWidgetItem("Ставка"));
    ui->twClientKredit->setHorizontalHeaderItem(4,new QTableWidgetItem("Срок"));
    ui->twClientKredit->setColumnWidth(0, 10);
    ui->twClientKredit->setColumnWidth(1, 265);
    ui->twClientKredit->setColumnWidth(2, 100);
    ui->twClientKredit->setColumnWidth(3, 100);
    ui->twClientKredit->setColumnWidth(4, 50);
    // Последний столбец растягивается при изменении размера формы
    ui->twClientKredit->horizontalHeader()->setStretchLastSection(true);
    // Разрешаем сортировку пользователю
    ui->twClientKredit->setSortingEnabled(true);
    ui->twClientKredit->sortByColumn(0);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twClientKredit->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Количество столбцов
    ui->twSec->setColumnCount(5);
    // Возможность прокрутки
    ui->twSec->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twSec->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twSec->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twSec->setHorizontalHeaderItem(0,new QTableWidgetItem("КредитКлиент"));
    ui->twSec->setHorizontalHeaderItem(1,new QTableWidgetItem("Кредит"));
    ui->twSec->setHorizontalHeaderItem(2,new QTableWidgetItem("Дата открытия"));
    ui->twSec->setHorizontalHeaderItem(3,new QTableWidgetItem("Дата закрытия"));
    ui->twSec->setHorizontalHeaderItem(4,new QTableWidgetItem("Погашенная часть"));
    ui->twSec->setColumnWidth(0, 5);
    ui->twSec->setColumnWidth(1, 5);
    ui->twSec->setColumnWidth(2, 85);
    ui->twSec->setColumnWidth(3, 85);
    ui->twSec->setColumnWidth(4, 20);
    // Последний столбец растягивается при изменении размера формы
    ui->twSec->horizontalHeader()->setStretchLastSection(true);
    // Разрешаем сортировку пользователю
    ui->twSec->setSortingEnabled(true);
    ui->twSec->sortByColumn(0);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twSec->setEditTriggers(QAbstractItemView::NoEditTriggers);

}
void ClientKredit::edit()
{
    // Подключение к БД, если оно не установлено
    if (!dbconn.isOpen())
    {
        dbconnect(); // Функция dbconnect() должна открывать соединение с базой данных
        if (!dbconn.isOpen())
        {
            QMessageBox::critical(this, "Ошибка", dbconn.lastError().text());
            return;
        }

    }

    int currow = ui->twSec->currentRow();

    if( currow < 0 )
    {
        QMessageBox::critical(this,"Ошибка","Не выбран пользователь!");
        return;
    }

    // Подготовка SQL запроса для обновления данных в таблице KredityKlienta
    QSqlQuery query(dbconn);
    QString sqlstr = "UPDATE KredityKlienta SET DateOpening = ?, DateClose = ?, RepaidPart = ? WHERE IDClient = ? AND IDKredit = ?";
    query.prepare(sqlstr);

    // Привязка значений к параметрам запроса
    query.bindValue(0, QDate::fromString(ui->leDateOp->text(), "yyyy-MM-dd")); // Предполагается, что дата хранится в текстовом формате "yyyy-MM-dd"
    query.bindValue(1, QDate::fromString(ui->leDateClose->text(), "yyyy-MM-dd"));
    query.bindValue(2, ui->lePart->text().toInt()); // Преобразование текста в целое число
    query.bindValue(3, clientID); // Переменная clientID должна быть определена и содержать ID клиента
    query.bindValue(4, ui->leIDKredit->text().toInt()); // Преобразование текста в целое число

    // Выполнение запроса
    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    // Обновление данных в компоненте таблицы
    selectAll();
}

void ClientKredit::dbconnect()
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

void ClientKredit::loadClientData(int idClient)
{
    clientID = idClient;
    selectAll(); // Загружаем данные кредитов данного клиента при установке ID клиента

}


void ClientKredit::selectAll()
{
    ui->twClientKredit->clearContents();
    ui->leSearch->clear();

    if (!dbconn.isOpen())
    {
        dbconnect();
        if (!dbconn.isOpen())
        {
            QMessageBox::critical(this, "Ошибка", dbconn.lastError().text());
            return;
        }
    }

    // Загрузка списка кредитов
    QSqlQuery query(dbconn);
    QString sqlstr = "SELECT * FROM Kredit";
    if (!query.exec(sqlstr))
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    if (query.isActive())
        ui->twClientKredit->setRowCount(query.size());
    else
        ui->twClientKredit->setRowCount(0);

    int i = 0;
    while (query.next())
    {
        ui->twClientKredit->setItem(i, 0, new QTableWidgetItem(query.value("IDKredit").toString()));
        ui->twClientKredit->setItem(i, 1, new QTableWidgetItem(query.value("Nazv").toString()));
        ui->twClientKredit->setItem(i, 2, new QTableWidgetItem(query.value("Country").toString()));
        ui->twClientKredit->setItem(i, 3, new QTableWidgetItem(query.value("Loan").toString()));
        ui->twClientKredit->setItem(i, 4, new QTableWidgetItem(query.value("Term").toString()));
        i++;
    }

    // Загрузка списка кредитов клиента
    QSqlQuery quer(dbconn);
    QString sqlst = "SELECT * FROM KredityKlienta WHERE IDClient = ?";
    quer.prepare(sqlst);
    quer.bindValue(0, clientID); // Привязываем clientID как int

    if (!quer.exec())
    {
        QMessageBox::critical(this, "Ошибка", quer.lastError().text());
        return;
    }

    if (quer.isActive())
        ui->twSec->setRowCount(quer.size());
    else
        ui->twSec->setRowCount(0);

    int k = 0;
    while (quer.next())
    {
        ui->twSec->setItem(k, 0, new QTableWidgetItem(quer.value("IDKreditKlienta").toString()));
        ui->twSec->setItem(k, 1, new QTableWidgetItem(quer.value("IDKredit").toString()));
        ui->twSec->setItem(k, 2, new QTableWidgetItem(quer.value("DateOpening").toString()));
        ui->twSec->setItem(k, 3, new QTableWidgetItem(quer.value("DateClose").toString()));
        ui->twSec->setItem(k, 4, new QTableWidgetItem(quer.value("RepaidPart").toString()));
        k++;
    }

    for (int ro = 0; ro < ui->twSec->rowCount(); ++ro)
    {
        ui->twSec->setRowHidden(ro, false);
    }
}



void ClientKredit::add()
{
    // Подключиться к БД
    if( !dbconn.isOpen() )
    {
        dbconnect();
        if( !dbconn.isOpen() )
        {
            QMessageBox::critical(this,"Ошибка",dbconn.lastError().text());
            return;
        }
    }

    QSqlQuery query(dbconn);
    QString sqlstr = "INSERT INTO KredityKlienta(IDClient, IDKredit, DateOpening, DateClose, RepaidPart) VALUES (?, ?, ?, ?, ?)";
    query.prepare(sqlstr);

    // Привязка параметров к запросу
       query.bindValue(0, clientID); // Используем полученный IDClient
       query.bindValue(1, ui->leIDKredit->text().toInt());
       query.bindValue(2, QDate::fromString(ui->leDateOp->text(), "yyyy-MM-dd"));
       query.bindValue(3, QDate::fromString(ui->leDateClose->text(), "yyyy-MM-dd"));
       query.bindValue(4, ui->lePart->text().toInt());

    if( !query.exec() )
    {
        QMessageBox::critical(this,"Ошибка",query.lastError().text());
        return;
    }

    selectAll();
}





void ClientKredit::onTableItemClicked(QTableWidgetItem *item) {
    if (!item) {
        return; // Защита от нулевого указателя
    }

    // Проверяем, от какой таблицы пришел сигнал
    if (item->tableWidget() == ui->twClientKredit) {
        // Обработка нажатия в таблице twClientKredit
        int row = item->row();
        QString idkr = ui->twClientKredit->item(row, 0)->text();
        ui->leIDKredit->setText(idkr);
    } else if (item->tableWidget() == ui->twSec) {
        int row = item->row();
        QString dateopen = ui->twSec->item(row, 2)->text();
        QString dateclose = ui->twSec->item(row, 3)->text();
        QString part = ui->twSec->item(row, 4)->text();
        ui->leDateOp->setText(dateopen);
        ui->leDateClose->setText(dateclose);
        ui->lePart->setText(part);
    }
}


void ClientKredit::del()
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

    int currow = ui->twSec->currentRow();
    if (currow < 0)
    {
        QMessageBox::critical(this, "Ошибка", "Не выбран пользователь!");
        return;
    }

    if (QMessageBox::question(this, "Удаление", "Удалить кредит?", QMessageBox::Cancel, QMessageBox::Ok) == QMessageBox::Cancel)
        return;


    QString idKredit = ui->twSec->item(currow, 1)->text();
    QString opendateText = ui->twSec->item(currow, 2)->text();
    QDate opendate = QDate::fromString(opendateText, "yyyy-MM-dd");

    QSqlQuery query(dbconn);
    QString sqlstr = "DELETE FROM KredityKlienta WHERE IDClient = ? AND IDKredit = ? AND DateOpening = ?";
    query.prepare(sqlstr);
    query.bindValue(0, clientID);
    query.bindValue(1, idKredit.toInt());
    query.bindValue(2, opendate);

    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    selectAll();
}




ClientKredit::~ClientKredit()
{
    if( dbconn.isOpen())
        dbconn.close();
    delete ui;
}

void ClientKredit::on_btnExit_clicked()
{
    close();

}

void ClientKredit::on_btnSearch_clicked()
{
    // Получение текста из поля ввода поиска
    QString searchText = ui->leSearch->text();

    // Формирование SQL-запроса для поиска по дате открытия
    QString queryStr = "SELECT * FROM Kredit WHERE Nazv LIKE '%" + searchText + "%'";

    // Выполнение запроса к базе данных
    QSqlQuery query;

    // Очистка предыдущих результатов поиска
    ui->twClientKredit->clearContents();
    ui->twClientKredit->setRowCount(0);

    int row = 0;
    while (query.next()) {
        ui->twClientKredit->insertRow(row); // Вставка новой строки
        ui->twClientKredit->setItem(row, 0, new QTableWidgetItem(query.value("IDKredit").toString()));
        ui->twClientKredit->setItem(row, 1, new QTableWidgetItem(query.value("Nazv").toString()));
        ui->twClientKredit->setItem(row, 2, new QTableWidgetItem(query.value("Country").toString()));
        ui->twClientKredit->setItem(row, 3, new QTableWidgetItem(query.value("Loan").toString()));
        ui->twClientKredit->setItem(row, 4, new QTableWidgetItem(query.value("Term").toString()));
        row++;
    }
}
