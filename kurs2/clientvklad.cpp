#include "clientvklad.h"
#include "ui_clientvklad.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <string>
#include "client.h"

clientvklad::clientvklad(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::clientvklad)
{
    ui->setupUi(this);

    ui->btnConnect->setVisible(false);

    connect(ui->btnConnect,SIGNAL(clicked(bool)),this, SLOT(dbconnect()));
    connect(ui->btnSelectAll,SIGNAL(clicked(bool)),this, SLOT(selectAll()));
    connect(ui->btnAdd,SIGNAL(clicked(bool)),this, SLOT(add()));
    connect(ui->btnDel,SIGNAL(clicked(bool)),this, SLOT(del()));
    connect(ui->btnEdit, SIGNAL(clicked(bool)), this, SLOT(edit()));
    connect(ui->twClientVklad, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onTableItemClicked(QTableWidgetItem*)));
    connect(ui->twSec, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onTableItemClicked(QTableWidgetItem*)));

    // Количество столбцов
    ui->twClientVklad->setColumnCount(5);
    // Возможность прокрутки
    ui->twClientVklad->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twClientVklad->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twClientVklad->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twClientVklad->setHorizontalHeaderItem(0,new QTableWidgetItem("Вклад"));
    ui->twClientVklad->setHorizontalHeaderItem(1,new QTableWidgetItem("Название"));
    ui->twClientVklad->setHorizontalHeaderItem(2,new QTableWidgetItem("Страна"));
    ui->twClientVklad->setHorizontalHeaderItem(3,new QTableWidgetItem("Ставка"));
    ui->twClientVklad->setHorizontalHeaderItem(4,new QTableWidgetItem("Срок"));
    ui->twClientVklad->setColumnWidth(0, 10);
    ui->twClientVklad->setColumnWidth(1, 215);
    ui->twClientVklad->setColumnWidth(2, 100);
    ui->twClientVklad->setColumnWidth(3, 100);
    ui->twClientVklad->setColumnWidth(4, 50);
    // Последний столбец растягивается при изменении размера формы
    ui->twClientVklad->horizontalHeader()->setStretchLastSection(true);
    // Разрешаем сортировку пользователю
    ui->twClientVklad->setSortingEnabled(true);
    ui->twClientVklad->sortByColumn(0);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twClientVklad->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Количество столбцов
    ui->twSec->setColumnCount(5);
    // Возможность прокрутки
    ui->twSec->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twSec->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twSec->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twSec->setHorizontalHeaderItem(0,new QTableWidgetItem("ВкладКлиент"));
    ui->twSec->setHorizontalHeaderItem(1,new QTableWidgetItem("Вклад"));
    ui->twSec->setHorizontalHeaderItem(2,new QTableWidgetItem("Дата открытия"));
    ui->twSec->setHorizontalHeaderItem(3,new QTableWidgetItem("Дата закрытия"));
    ui->twSec->setHorizontalHeaderItem(4,new QTableWidgetItem("Накопительная часть"));
    ui->twSec->setColumnWidth(0, 5);
    ui->twSec->setColumnWidth(1, 5);
    ui->twSec->setColumnWidth(2, 85);
    ui->twSec->setColumnWidth(3, 85);
    ui->twSec->setColumnWidth(4, 20);    // Последний столбец растягивается при изменении размера формы
    ui->twSec->horizontalHeader()->setStretchLastSection(true);
    // Разрешаем сортировку пользователю
    ui->twSec->setSortingEnabled(true);
    ui->twSec->sortByColumn(0);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twSec->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void clientvklad::dbconnect()
{
    if (!dbconn.isOpen())
    {
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
void clientvklad::selectAll()
{
    // Очистка содержимого таблицы
    ui->twClientVklad->clearContents();
    ui->leSearch->clear();

    // Подключение к БД, если необходимо
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
    QString sqlstr = "select * from Vklad";
    if (!query.exec(sqlstr))
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    // Установка количества строк в таблице
    if (query.isActive())
        ui->twClientVklad->setRowCount(query.size());
    else
        ui->twClientVklad->setRowCount(0);

    int i = 0;
    while (query.next())
    {
        ui->twClientVklad->setItem(i, 0, new QTableWidgetItem(query.value("IDVclad").toString()));
        ui->twClientVklad->setItem(i, 1, new QTableWidgetItem(query.value("Nazv").toString()));
        ui->twClientVklad->setItem(i, 2, new QTableWidgetItem(query.value("Country").toString()));
        ui->twClientVklad->setItem(i, 3, new QTableWidgetItem(query.value("Loan").toString()));
        ui->twClientVklad->setItem(i, 4, new QTableWidgetItem(query.value("Term").toString()));
        i++;
    }

    // Загрузка списка кредитов клиента
    QSqlQuery quer(dbconn);
    QString sqlst = "SELECT * FROM VkladiKlienta WHERE IDClient = ?";
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
        ui->twSec->setItem(k, 1, new QTableWidgetItem(quer.value("IDVclad").toString()));
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



void clientvklad::add()
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
    QString sqlstr = "INSERT INTO VkladiKlienta(IDClient, idvclad, DateOpening, DateClose, RepaidPart) VALUES (?, ?, ?, ?, ?)";
    query.prepare(sqlstr);

    // Привязка параметров к запросу
       query.bindValue(0, clientID); // Используем полученный IDClient
       query.bindValue(1, ui->leID->text().toInt()); // Предполагается, что IDKredit можно получить из QLineEdit
       query.bindValue(2, QDate::fromString(ui->leDateOpen->text(), "yyyy-MM-dd")); // Преобразование строки в QDate
       query.bindValue(3, QDate::fromString(ui->leDateClos->text(), "yyyy-MM-dd")); // Преобразование строки в QDate
       query.bindValue(4, ui->lePart->text().toInt()); // Предполагается, что RepaidPart можно получить из QLineEdit


    if( !query.exec() )
    {
        QMessageBox::critical(this,"Ошибка",query.lastError().text());
        return;
    }

    selectAll();
}




void clientvklad::onTableItemClicked(QTableWidgetItem *item) {
    if (!item) {
        return; // Защита от нулевого указателя
    }

    // Проверяем, от какой таблицы пришел сигнал
    if (item->tableWidget() == ui->twClientVklad) {
        // Обработка нажатия в таблице twClientKredit
        int row = item->row();
        QString idkr = ui->twClientVklad->item(row, 0)->text();
        ui->leID->setText(idkr);
    } else if (item->tableWidget() == ui->twSec) {
        int row = item->row();
        QString dateopen = ui->twSec->item(row, 2)->text();
        QString dateclose = ui->twSec->item(row, 3)->text();
        QString part = ui->twSec->item(row, 4)->text();
        ui->leDateOpen->setText(dateopen);
        ui->leDateClos->setText(dateclose);
        ui->lePart->setText(part);
    }
}


void clientvklad::loadClientData(int idClient)
{
    clientID = idClient;
    selectAll(); // Загружаем данные кредитов данного клиента при установке ID клиента

}

void clientvklad::del()
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

    int currow = ui->twClientVklad->currentRow();
    if (currow < 0)
    {
        QMessageBox::critical(this, "Ошибка", "Не выбран пользователь!");
        return;
    }

    // Получаем подтверждение удаления у пользователя
    if (QMessageBox::question(this, "Удаление", "Удалить вклад?", QMessageBox::Cancel, QMessageBox::Ok) == QMessageBox::Cancel)
        return;

    // Получаем IDVklad из выбранной строки в twClientVklad
    QString idVklad = ui->twClientVklad->item(currow, 0)->text();
    QString opendateText = ui->twSec->item(currow, 2)->text();
    QDate opendate = QDate::fromString(opendateText, "yyyy-MM-dd");

    QSqlQuery query(dbconn);
    QString sqlstr = "DELETE FROM VkladiKlienta WHERE IDClient = ? AND idvclad = ? AND DateOpening = ?";
    query.prepare(sqlstr);
    query.bindValue(0, clientID);
    query.bindValue(1, idVklad.toInt());
    query.bindValue(2, opendate);

    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }
    selectAll();
}




void clientvklad::edit()
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

    int currow = ui->twSec->currentRow();

    if( currow < 0 )
    {
        QMessageBox::critical(this,"Ошибка","Не выбран вклад!");
        return;
    }

       QSqlQuery query(dbconn);
       QString sqlstr = "UPDATE VkladiKlienta SET DateOpening = ?, DateClose = ?, RepaidPart = ? WHERE IDClient = ? AND IDVclad = ?";
       query.prepare(sqlstr);
       // Привязка параметров к запросу

       query.bindValue(0, QDate::fromString(ui->leDateOpen->text(), "yyyy-MM-dd"));
       query.bindValue(1, QDate::fromString(ui->leDateClos->text(), "yyyy-MM-dd"));
       query.bindValue(2, ui->lePart->text().toInt());
       query.bindValue(3, clientID);
       query.bindValue(4, ui->leID->text().toInt());

       // Выполнить запрос
       if (!query.exec())
       {
           QMessageBox::critical(this, "Ошибка", query.lastError().text());
           return;
       }

       // Обновить записи в компоненте таблицы
       selectAll();
   }

clientvklad::~clientvklad()
{
    if( dbconn.isOpen())
        dbconn.close();
    delete ui;
}

void clientvklad::on_btnExit_clicked()
{
    close();
}

void clientvklad::on_btnSearch_clicked()
{
    // Получение текста из поля ввода поиска
    QString searchText = ui->leSearch->text();

    // Формирование SQL-запроса для поиска по названию вклада в таблице Vklad
    QString queryStr = "SELECT * FROM Vklad WHERE Nazv LIKE '%" + searchText + "%'";

    // Выполнение запроса к базе данных
    QSqlQuery query;
    if (!query.exec(queryStr)) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return;
    }

    // Очистка предыдущих результатов поиска
    ui->twClientVklad->clearContents();
    ui->twClientVklad->setRowCount(0);

    int row = 0;
    while (query.next()) {
        ui->twClientVklad->insertRow(row); // Вставка новой строки
        ui->twClientVklad->setItem(row, 0, new QTableWidgetItem(query.value("IDVclad").toString()));
        ui->twClientVklad->setItem(row, 1, new QTableWidgetItem(query.value("Nazv").toString()));
        ui->twClientVklad->setItem(row, 2, new QTableWidgetItem(query.value("Country").toString()));
        ui->twClientVklad->setItem(row, 3, new QTableWidgetItem(query.value("Loan").toString()));
        ui->twClientVklad->setItem(row, 4, new QTableWidgetItem(query.value("Term").toString()));
        row++;
    }
}

