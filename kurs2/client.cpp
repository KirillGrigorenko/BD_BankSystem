#include "client.h"
#include "ui_client.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <string>
#include "clientkredit.h"
#include "clientvklad.h"
#include "schet.h"

client::client(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::client)
{
    ui->setupUi(this);

    connect(ui->btnConnect,SIGNAL(clicked(bool)),this, SLOT(dbconnect()));
    connect(ui->btnSelectAll,SIGNAL(clicked(bool)),this, SLOT(selectAll()));
    connect(ui->btnAdd,SIGNAL(clicked(bool)),this, SLOT(add()));
    connect(ui->btnDel,SIGNAL(clicked(bool)),this, SLOT(del()));
    connect(ui->btnEdit, SIGNAL(clicked(bool)), this, SLOT(edit()));
    connect(ui->twClient, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onTableItemClicked(QTableWidgetItem*)));

    ui->btnConnect->setVisible(false);

    // Количество столбцов
    ui->twClient->setColumnCount(5);
    // Возможность прокрутки
    ui->twClient->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twClient->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twClient->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twClient->setHorizontalHeaderItem(0,new QTableWidgetItem("ФИО"));
    ui->twClient->setHorizontalHeaderItem(1,new QTableWidgetItem("Телефон"));
    ui->twClient->setHorizontalHeaderItem(2,new QTableWidgetItem("Дата"));
    ui->twClient->setHorizontalHeaderItem(3,new QTableWidgetItem("Логин"));
    ui->twClient->setHorizontalHeaderItem(4,new QTableWidgetItem("Пароль"));
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
    ui->twClientKredit->setColumnWidth(0, 15);
    ui->twClientKredit->setColumnWidth(1, 200);
    ui->twClientKredit->setColumnWidth(2, 75);
    ui->twClientKredit->setColumnWidth(3, 100);
    ui->twClientKredit->setColumnWidth(4, 50);

    // Последний столбец растягивается при изменении размера формы
    ui->twClientKredit->horizontalHeader()->setStretchLastSection(true);
    // Разрешаем сортировку пользователю
    ui->twClientKredit->setSortingEnabled(true);
    ui->twClientKredit->sortByColumn(0);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twClientKredit->setEditTriggers(QAbstractItemView::NoEditTriggers);

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
    ui->twClientVklad->setColumnWidth(0, 15);
    ui->twClientVklad->setColumnWidth(1, 200);
    ui->twClientVklad->setColumnWidth(2, 75);
    ui->twClientVklad->setColumnWidth(3, 100);
    ui->twClientVklad->setColumnWidth(4, 50);

    // Последний столбец растягивается при изменении размера формы
    ui->twClientVklad->horizontalHeader()->setStretchLastSection(true);
    // Разрешаем сортировку пользователю
    ui->twClientVklad->setSortingEnabled(true);
    ui->twClientVklad->sortByColumn(0);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twClientVklad->setEditTriggers(QAbstractItemView::NoEditTriggers);

    selectAll();
}

void client::dbconnect()
{
    if(!dbconn.isOpen())
    {
        dbconn=QSqlDatabase::addDatabase("QPSQL");
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
void client::selectAll()
{
    ui->twClient->clearContents();
    ui->leSearch->clear();

    if( !dbconn.isOpen() )
    {
        dbconnect();
        if( !dbconn.isOpen() )
        {
            QMessageBox::critical(this,"БД не открыта",dbconn.lastError().text());
            return;
        }
    }
    QSqlQuery query(dbconn);
    QString sqlstr = "select * from Client";
    if( !query.exec(sqlstr) )
    {
        QMessageBox::critical(this,"Ошибка", query.lastError().text());
        return;
    }
    if( query.isActive())
        ui->twClient->setRowCount( query.size());
    else
        ui->twClient->setRowCount( 0);

    int i=0;
    while(query.next())
{
    ui->twClient->setItem(i,0,new
QTableWidgetItem(query.value("FIO").toString()));
    ui->twClient->setItem(i,1,new
QTableWidgetItem(query.value("Phone").toString()));
    ui->twClient->setItem(i,2,new
QTableWidgetItem(query.value("Birth").toString()));
    ui->twClient->setItem(i,3,new
QTableWidgetItem(query.value("Login").toString()));
    ui->twClient->setItem(i,4,new
QTableWidgetItem(query.value("Pass").toString()));
        i++;
    }
    for (int row = 0; row < ui->twClient->rowCount(); ++row){
        ui->twClient->setRowHidden(row, false);
    }

}

void client::getClientIDByFIO(const QString &fio)
{
    QSqlQuery query(dbconn);
    query.prepare("SELECT IDClient FROM Client WHERE FIO = ?");
    query.bindValue(0, fio);
    if (!query.exec() || !query.first()) {
        QMessageBox::critical(this, "Ошибка", "Пользователь не найден.");
        return; // Возвращаем -1 в случае ошибки
    }

    clientID = query.value(0).toInt();
}

void client::add()
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
    QSqlQuery query(dbconn);

    QString sqlstr = "insert into Client(FIO, Phone, Birth, Login, Pass) values (?,?,?,?,?)";
    query.prepare(sqlstr);

    query.bindValue(0,ui->leFIO->text());

    QString datestring = ui->leData->text();
    QDate date = QDate::fromString(datestring, "yyyy-MM-dd");

    query.bindValue(2, date);
    query.bindValue(1,ui->lePhone->text());
    query.bindValue(3,ui->leLogin->text());
    query.bindValue(4,ui->lePassword->text());

    if( !query.exec() )
    {
        QMessageBox::critical(this,"Ошибка",query.lastError().text());
        return;
    }
    selectAll();
}

void client::onTableItemClicked(QTableWidgetItem *item) {
    if (item == nullptr) {
        return; // Защита от нулевого указателя
    }
    // Получаем индекс строки, которая была выбрана
    int row = item->row();

    // Получаем данные из выбранной строки
    QString fio = ui->twClient->item(row, 0)->text();
    getClientIDByFIO(fio);

    ui->leFIO->setText(fio);
    ui->leData->setText(ui->twClient->item(row, 2)->text());
    ui->lePhone->setText(ui->twClient->item(row, 1)->text());
    ui->leLogin->setText(ui->twClient->item(row, 3)->text());
    ui->lePassword->setText(ui->twClient->item(row, 4)->text());

    // Загружаем данные по кредитам клиента
    loadClientKreditData();
    loadClientVkladData();
}

void client::loadClientKreditData()
{
    ui->twClientKredit->clearContents();
    ui->twClientKredit->setRowCount(0);

    QSqlQuery query(dbconn);
    QString sqlstr = "SELECT K.IDKredit, K.Nazv, K.Country, K.Loan, K.Term  "
                     "FROM KredityKlienta KK "
                     "JOIN Kredit K ON KK.IDKredit = K.IDKredit "
                     "WHERE KK.IDClient = ?";
    query.prepare(sqlstr);
    query.bindValue(0, clientID);

    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    ui->twClientKredit->setHorizontalHeaderItem(0, new QTableWidgetItem("Кредит"));
    ui->twClientKredit->setHorizontalHeaderItem(1, new QTableWidgetItem("Название"));
    ui->twClientKredit->setHorizontalHeaderItem(2, new QTableWidgetItem("Страна"));
    ui->twClientKredit->setHorizontalHeaderItem(3, new QTableWidgetItem("Ставка"));
    ui->twClientKredit->setHorizontalHeaderItem(4, new QTableWidgetItem("Срок"));


    int row = 0;
    while (query.next())
    {
        if (row >= ui->twClientKredit->rowCount())
               ui->twClientKredit->insertRow(row);

        QTableWidgetItem *itemId = new QTableWidgetItem(query.value("IDKredit").toString());
        QTableWidgetItem *itemNazv = new QTableWidgetItem(query.value("Nazv").toString());
        QTableWidgetItem *itemCountry = new QTableWidgetItem(query.value("Country").toString());
        QTableWidgetItem *itemLoan = new QTableWidgetItem(query.value("Loan").toString());
        QTableWidgetItem *itemTerm = new QTableWidgetItem(query.value("Term").toString());

        ui->twClientKredit->setItem(row, 0, itemId);
        ui->twClientKredit->setItem(row, 1, itemNazv);
        ui->twClientKredit->setItem(row, 2, itemCountry);
        ui->twClientKredit->setItem(row, 3, itemLoan);
        ui->twClientKredit->setItem(row, 4, itemTerm);

        row++;
    }
}



void client::loadClientVkladData()
{
    ui->twClientVklad->clearContents();
    ui->twClientVklad->setRowCount(0);

    QSqlQuery query(dbconn);
    QString sqlstr = "SELECT VK.idvclad, V.Nazv, V.Country, V.Loan, V.Term "
                     "FROM VkladiKlienta VK "
                     "JOIN Vklad V ON VK.idvclad = V.idvclad "
                     "WHERE VK.IDClient = ?";
    query.prepare(sqlstr);
    query.bindValue(0, clientID);

    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    int row = 0;
    while (query.next())
    {
        ui->twClientVklad->insertRow(row); // Вставляем новую строку в таблицу

        QTableWidgetItem *itemId = new QTableWidgetItem(query.value("idvclad").toString());
        QTableWidgetItem *itemNazv = new QTableWidgetItem(query.value("Nazv").toString());
        QTableWidgetItem *itemCountry = new QTableWidgetItem(query.value("Country").toString());
        QTableWidgetItem *itemLoan = new QTableWidgetItem(query.value("Loan").toString());
        QTableWidgetItem *itemTerm = new QTableWidgetItem(query.value("Term").toString());

        ui->twClientVklad->setItem(row, 0, itemId);
        ui->twClientVklad->setItem(row, 1, itemNazv);
        ui->twClientVklad->setItem(row, 2, itemCountry);
        ui->twClientVklad->setItem(row, 3, itemLoan);
        ui->twClientVklad->setItem(row, 4, itemTerm);

        row++;
    }
}


void client::del()
{
    // Подключение к БД
    if( !dbconn.isOpen() )
    {
        dbconnect();
        if( !dbconn.isOpen() )
        {
            QMessageBox::critical(this,"Ошибка",dbconn.lastError().text());
            return;
        }
    }
    int currow = ui->twClient->currentRow();
    if( currow < 0 )
    {
        QMessageBox::critical(this,"Ошибка","Не выбран пользователь!");
        return;
    }

    if( QMessageBox::question(this,"Удалить","Удалить выбранного клиента?",
    QMessageBox::Cancel,QMessageBox::Ok)==QMessageBox::Cancel)
    return;
    // Создать объект запроса
    QSqlQuery query(dbconn);
    QString sqlstr = "delete from client where FIO = '" + ui->twClient->item(currow,0)->text() + "'";



    if( !query.exec(sqlstr) )
    {
        QMessageBox::critical(this,"Ошибка",query.lastError().text());
        return;
    }
    selectAll();
}


void client::edit()
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

    int currow = ui->twClient->currentRow();
        // Если он меньше 0 (строка не выбрана), то
        // сообщение об ошибке и выход из функции
    if( currow < 0 )
    {
        QMessageBox::critical(this,"Ошибка","Не выбран пользователь!");
        return;
    }
    QSqlQuery query(dbconn);
    QString sqlstr = "UPDATE Client SET FIO = ?, Phone = ?, Birth = ?, Login = ?, Pass = ? WHERE IDClient = ?";

    query.prepare(sqlstr);
        // Передать параметры из полей ввода в запрос
    query.bindValue(0,ui->leFIO->text());
    query.bindValue(1,ui->lePhone->text());
    query.bindValue(2,ui->leData->text());
    query.bindValue(3,ui->leLogin->text());
    query.bindValue(4,ui->lePassword->text());
    query.bindValue(5,clientID);
        // Выполнить запрос
    if( !query.exec() )
    {
        QMessageBox::critical(this,"Ошибка",query.lastError().text());
        return;
    }
        // и обновить записи в компоненте таблицы
    selectAll();
}

client::~client()
{
    if(dbconn.isOpen())
        dbconn.close();
    delete ui;
}

void client::on_btnEXit_clicked()
{
    close();
    admin *adminDialog = new admin;
    adminDialog->setModal(true);
    adminDialog->show();
}

void client::on_btnSearch_clicked()
{
    QString search = ui->leSearch->text();

    int rowCount = ui->twClient->rowCount();
    int colCount = ui->twClient->columnCount();

    for(int row = 0; row < rowCount; ++row){
        bool match = false;
        for(int col = 0; col < colCount; ++col){
            QTableWidgetItem *item = ui->twClient->item(row, col);
            if (item && item->text().contains(search, Qt::CaseInsensitive)){
                match = true;
                break;
            }
        }
        ui->twClient->setRowHidden(row, !match);
    }
}


void client::on_btnCLientKredit_clicked()
{
    // Получаем текущую выбранную строку в таблице
       int currow = ui->twClient->currentRow();
       if (currow < 0)
       {
           QMessageBox::critical(this, "Ошибка", "Не выбран пользователь!");
           return;
       }

       ClientKredit *clientKredit = new ClientKredit(this);
       clientKredit->loadClientData(clientID);
       clientKredit->setModal(true);
       clientKredit->show();


}

void client::on_btnClientVklad_clicked()
{
    // Получаем текущую выбранную строку в таблице
       int currow = ui->twClient->currentRow();
       if (currow < 0)
       {
           QMessageBox::critical(this, "Ошибка", "Не выбран пользователь!");
           return;
       }

       clientvklad *clvk = new clientvklad(this);
       clvk->loadClientData(clientID);
       clvk->setModal(true);
       clvk->show();
}

void client::on_btnSchet_clicked()
{
       int currow = ui->twClient->currentRow();
       if (currow < 0)
       {
           QMessageBox::critical(this, "Ошибка", "Не выбран пользователь!");
           return;
       }

       Schet *scht = new Schet(this);
       scht->loadClientData(clientID);
       scht->setModal(true);
       scht->show();
}
