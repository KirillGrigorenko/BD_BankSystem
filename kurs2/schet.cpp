#include "schet.h"
#include "ui_schet.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <string>

Schet::Schet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Schet)
{
    ui->setupUi(this);
    connect(ui->btnConnect,SIGNAL(clicked(bool)),this, SLOT(dbconnect()));
    connect(ui->btnSelectAll,SIGNAL(clicked(bool)),this, SLOT(selectAll()));
    connect(ui->btnAdd,SIGNAL(clicked(bool)),this, SLOT(add()));
    connect(ui->btnDel,SIGNAL(clicked(bool)),this, SLOT(del()));
    connect(ui->btnEdit, SIGNAL(clicked(bool)), this, SLOT(edit()));
    connect(ui->twSchet, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onTableItemClicked(QTableWidgetItem*)));
    connect(ui->btnSearch, &QPushButton::clicked, this, &Schet::on_btnSearch_clicked);


    ui->btnConnect->setVisible(false);
    // Количество столбцов
    ui->twSchet->setColumnCount(4);
    // Возможность прокрутки
    ui->twSchet->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twSchet->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twSchet->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twSchet->setHorizontalHeaderItem(0,new QTableWidgetItem("Счет"));
    ui->twSchet->setHorizontalHeaderItem(1,new QTableWidgetItem("Валюта"));
    ui->twSchet->setHorizontalHeaderItem(2,new QTableWidgetItem("Баланс"));
    ui->twSchet->setHorizontalHeaderItem(3,new QTableWidgetItem("Дата открытия"));
    // Последний столбец растягивается при изменении размера формы
    ui->twSchet->horizontalHeader()->setStretchLastSection(true);
    // Разрешаем сортировку пользователю
    ui->twSchet->setSortingEnabled(true);
    ui->twSchet->sortByColumn(0);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twSchet->setEditTriggers(QAbstractItemView::NoEditTriggers);


    // Количество столбцов
    ui->twSec->setColumnCount(3);
    // Возможность прокрутки
    ui->twSec->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twSec->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twSec->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twSec->setHorizontalHeaderItem(0,new QTableWidgetItem("ФИО"));
    ui->twSec->setHorizontalHeaderItem(1,new QTableWidgetItem("Номер телефона"));
    ui->twSec->setHorizontalHeaderItem(2,new QTableWidgetItem("Дата рождения"));
    ui->twSec->setColumnWidth(0, 300);
    ui->twSec->setColumnWidth(1, 160);
    ui->twSec->setColumnWidth(2, 75);
    // Последний столбец растягивается при изменении размера формы
    ui->twSec->horizontalHeader()->setStretchLastSection(true);
    // Разрешаем сортировку пользователю
    ui->twSec->setSortingEnabled(true);
    ui->twSec->sortByColumn(0);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twSec->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void Schet::loadClientData(int idClient)
{
    clientID = idClient;
    selectAll(); // Загружаем данные кредитов данного клиента при установке ID клиента

}

void Schet::dbconnect()
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
void Schet::SecondTable()
{
    // Подключение к БД
    if (!dbconn.isOpen())
    {
        dbconnect();
        if (!dbconn.isOpen())
        {
            QMessageBox::critical(this, "Ошибка", dbconn.lastError().text());
            return;
        }
    }

    // Получаем IDSchet из текущей выбранной строки в twSchet
    int schetRow = ui->twSchet->currentRow();
    if (schetRow < 0)
    {
        QMessageBox::critical(this, "Ошибка", "Не выбран счёт!");
        return;
    }
    QString idSchet = ui->twSchet->item(schetRow, 0)->text();

    // Запрос для получения IDClient по IDSchet
    QSqlQuery clientQuery(dbconn);
    QString clientSql = "SELECT IDClient FROM Schet WHERE IDSchet = ?";
    clientQuery.prepare(clientSql);
    clientQuery.bindValue(0, idSchet);

    if (!clientQuery.exec() || !clientQuery.next())
    {
        QMessageBox::critical(this, "Ошибка", "Клиент не найден.");
        return;
    }
    int idClient = clientQuery.value(0).toInt();

    // Очистка содержимого таблицы ui->twSec
    ui->twSec->clearContents();

    // Запрос для получения данных клиента по IDClient
    QSqlQuery query(dbconn);
    QString sqlstr = "SELECT * FROM Client WHERE IDClient = " + QString::number(idClient);

    // Выполнение запроса и проверка его успешности
    if (!query.exec(sqlstr))
    {
        QMessageBox::critical(this, "Ошибка", "Какашка");
        return;
    }

    // Установка числа строк в таблице ui->twSec
    if (query.isActive())
        ui->twSec->setRowCount(query.size());
    else
        ui->twSec->setRowCount(0);

    // Заполнение таблицы ui->twSec данными о клиенте
    int row = 0;
    while (query.next())
    {
        ui->twSec->setItem(row, 0, new QTableWidgetItem(query.value("FIO").toString()));
        ui->twSec->setItem(row, 1, new QTableWidgetItem(query.value("Phone").toString()));
        ui->twSec->setItem(row, 2, new QTableWidgetItem(query.value("Birth").toString()));
        row++;
    }
}


void Schet::selectAll()
{
    ui->twSchet->clearContents();
    ui->leSearch->clear();

    if( !dbconn.isOpen() )
    {
        dbconnect();
        if( !dbconn.isOpen() )
        {
            QMessageBox::critical(this,"Ошибка",dbconn.lastError().text());
            return;
        }
    }
        // Создать объект запроса с привязкой к установленному соединению
    QSqlQuery query(dbconn);
    QString sqlstr = "select * from Schet";
        // Выполнить запрос и поверить его успешность
    if( !query.exec(sqlstr) )
    {
        QMessageBox::critical(this,"Error", query.lastError().text());
        return;
    }
    if( query.isActive())
        ui->twSchet->setRowCount( query.size());
    else
        ui->twSchet->setRowCount( 0);

    int i=0;
    while(query.next())
{
    ui->twSchet->setItem(i,0,new
QTableWidgetItem(query.value("IDSchet").toString()));
    ui->twSchet->setItem(i,1,new
QTableWidgetItem(query.value("Currency").toString()));
    ui->twSchet->setItem(i,2,new
QTableWidgetItem(query.value("Balance").toString()));
    ui->twSchet->setItem(i,3,new
QTableWidgetItem(query.value("OpeningDate").toString()));

        i++;
    }

    QSqlQuery quer(dbconn);
    QString sqlst = "SELECT * FROM Schet WHERE IDClient = ?";
    quer.prepare(sqlst);
    quer.bindValue(0, clientID);

    if (!quer.exec())
    {
        QMessageBox::critical(this, "Ошибка", quer.lastError().text());
        return;
    }

    if (quer.isActive())
        ui->twSchet->setRowCount(quer.size());
    else
        ui->twSchet->setRowCount(0);

    int k = 0;
    while (quer.next())
    {
        ui->twSchet->setItem(k, 0, new QTableWidgetItem(quer.value("IDSchet").toString()));
        ui->twSchet->setItem(k, 1, new QTableWidgetItem(quer.value("Currency").toString()));
        ui->twSchet->setItem(k, 2, new QTableWidgetItem(quer.value("Balance").toString()));
        ui->twSchet->setItem(k, 3, new QTableWidgetItem(quer.value("OpeningDate").toString()));
        k++;
    }

    for (int ro = 0; ro < ui->twSec->rowCount(); ++ro)
    {
        ui->twSchet->setRowHidden(ro, false);
    }
}

void Schet::add()
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
    QString sqlstr = "insert into Schet(IDClient, Currency, Balance, OpeningDate) values (?,?,?,?)";
    // Подготовить запрос
    query.prepare(sqlstr);

    // Передать параметры из полей ввода в запрос
    query.bindValue(0,clientID);
    query.bindValue(1,ui->leCur->text());
    query.bindValue(2,ui->leBalance->text().toDouble());

    QString datestring = ui->leData->text();
    QDate date = QDate::fromString(datestring, "yyyy-MM-dd");
    query.bindValue(3,date);

    if( !query.exec() )
    {
        QMessageBox::critical(this,"Ошибка",query.lastError().text());
        return;
    }

    selectAll();
}


void Schet::onTableItemClicked(QTableWidgetItem *item) {
    if (item == nullptr) {
        return; // Защита от нулевого указателя
    }

    // Получаем индекс строки, которая была выбрана
    int row = item->row();

    // Получаем данные из выбранной строки
    QString ids = ui->twSchet->item(row, 0)->text();
    QString cur = ui->twSchet->item(row, 1)->text();
    QString bal = ui->twSchet->item(row, 2)->text();
    QString dat = ui->twSchet->item(row, 3)->text();

    // Отображаем данные в соответствующих Line Edit
    ui->leIDSchet->setText(ids);
    ui->leCur->setText(cur);
    ui->leBalance->setText(bal);
    ui->leData->setText(dat);

    SecondTable();
}

void Schet::del()
{
    // Подключение к БД
    if (!dbconn.isOpen())
    {
        dbconnect();
        if (!dbconn.isOpen())
        {
            QMessageBox::critical(this, "Ошибка", dbconn.lastError().text());
            return;
        }
    }

    int currow = ui->twSchet->currentRow();
    if (currow < 0)
    {
        QMessageBox::critical(this, "Ошибка", "Не выбран счёт!");
        return;
    }

    if (QMessageBox::question(this, "Удаление", "Удалить счёт?",
                              QMessageBox::Cancel, QMessageBox::Ok) == QMessageBox::Cancel)
    {
        return;
    }

    QSqlQuery query(dbconn);

    QString sqlstr = "delete from Schet where IDSchet = ? AND Currency = ? AND Balance = ?";
    query.prepare(sqlstr);
    query.bindValue(0, ui->twSchet->item(currow, 0)->text().toInt()); // Assuming IDSchet is in the first column
    query.bindValue(1, ui->twSchet->item(currow, 1)->text()); // Assuming Currency is in the second column
    query.bindValue(2, ui->twSchet->item(currow, 2)->text().toInt()); // Assuming Balance is in the third column

    if (!query.exec())
    {
        QMessageBox::critical(this, "Error", query.lastError().text());
        return;
    }

    // Обновить содержимое компонента таблицы
    selectAll();
}



void Schet::edit()
{
        // Подключиться к БД
    if( !dbconn.isOpen() )
    {
        dbconnect();
        if( !dbconn.isOpen() )
        {
            QMessageBox::critical(this,"Error",dbconn.lastError().text());
            return;
        }
    }

    int currow = ui->twSchet->currentRow();
        // Если он меньше 0 (строка не выбрана), то
        // сообщение об ошибке и выход из функции
    if( currow < 0 )
    {
        QMessageBox::critical(this,"Ошибка","Не выбран счёт!");
        return;
    }
        // Спросить у пользователя подтверждение удаления записи
        // Используется статический метод QMessageBox::question
        // для задания вопроса, который возвращает код нажатой кнопки


    QString fio = ui->twSec->item(0, 0)->text();
      qDebug() << "Полученное ФИО: " << fio;

       // Подготовка запроса на получение IDClient по ФИО
       QSqlQuery clientQuery(dbconn);
       QString clientSql = "SELECT IDClient FROM Client WHERE FIO = ?";
       clientQuery.prepare(clientSql);
       clientQuery.bindValue(0, fio);

    if (!clientQuery.exec() || !clientQuery.next())
    {
        QMessageBox::critical(this, clientSql, "Клиент не найден");
        return;
    }
   int idClient = clientQuery.value(0).toInt();


    QSqlQuery query(dbconn);

    QString sqlstr = "UPDATE Schet SET IDClient = ?, Currency = ?, Balance = ?, OpeningDate = ? WHERE IDSchet = '"+ ui->twSchet->item(currow,0)->text() + "'";

        // Подготовить запрос
    query.prepare(sqlstr);
        // Передать параметры из полей ввода в запрос
    query.bindValue(0,idClient);
    query.bindValue(1,ui->leCur->text());
    query.bindValue(2,ui->leBalance->text());
    query.bindValue(3,ui->leData->text());

        // Выполнить запрос
    if( !query.exec() )
    {
        QMessageBox::critical(this,"Ошибка",query.lastError().text());
        return;
    }
        // и обновить записи в компоненте таблицы
    selectAll();
}


Schet::~Schet()
{
    if( dbconn.isOpen())
        dbconn.close();
    delete ui;
}

void Schet::on_btnExit_clicked()
{
  close();
}

void Schet::on_btnSearch_clicked()
{
    if( !dbconn.isOpen() )
     {
         dbconnect();
         if( !dbconn.isOpen() )
         {
             QMessageBox::critical(this,"Ошибка",dbconn.lastError().text());
             return;
         }
     }

     QString searchValue = ui->leSearch->text();

     QSqlQuery query(dbconn);
     QString sqlstr = "SELECT * FROM Schet WHERE IDClient::text LIKE '%' || ? || '%' OR Currency LIKE '%' || ? || '%'";

     query.prepare(sqlstr);
     query.bindValue(0, searchValue);
     query.bindValue(1, searchValue);

     if( !query.exec() )
     {
         QMessageBox::critical(this,"Error",query.lastError().text());
         return;
     }

     ui->twSchet->clearContents();

     int row = 0;
     while(query.next())
     {
         ui->twSchet->setRowCount(row + 1);
         ui->twSchet->setItem(row, 0, new QTableWidgetItem(query.value("IDSchet").toString()));
         ui->twSchet->setItem(row, 1, new QTableWidgetItem(query.value("IDClient").toString()));
         ui->twSchet->setItem(row, 2, new QTableWidgetItem(query.value("Currency").toString()));
         ui->twSchet->setItem(row, 3, new QTableWidgetItem(query.value("Balance").toString()));
         ui->twSchet->setItem(row, 4, new QTableWidgetItem(query.value("OpeningDate").toString()));
         row++;
     }
}
