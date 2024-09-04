#include "vklad.h"
#include "ui_vklad.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <string>

Vklad::Vklad(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Vklad)
{
    ui->setupUi(this);


    connect(ui->btnConnect,SIGNAL(clicked(bool)),this, SLOT(dbconnect()));
    connect(ui->btnSelectAll,SIGNAL(clicked(bool)),this, SLOT(selectAll()));
    connect(ui->btnAdd,SIGNAL(clicked(bool)),this, SLOT(add()));
    connect(ui->btnDel,SIGNAL(clicked(bool)),this, SLOT(del()));
    connect(ui->btnEdit, SIGNAL(clicked(bool)), this, SLOT(edit()));
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


void Vklad::dbconnect()
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

void Vklad::selectAll()
{
    ui->twVklad->clearContents();
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
    QString sqlstr = "select * from Vklad";
    if( !query.exec(sqlstr) )
    {
        QMessageBox::critical(this,"Ошибка", query.lastError().text());
        return;
    }
    if( query.isActive())
        ui->twVklad->setRowCount( query.size());
    else
        ui->twVklad->setRowCount( 0);
    int i=0;
    while(query.next())
{
        ui->twVklad->setItem(i,0,new
QTableWidgetItem(query.value("IDVclad").toString()));
        ui->twVklad->setItem(i,1,new
QTableWidgetItem(query.value("Nazv").toString()));
    ui->twVklad->setItem(i,2,new
QTableWidgetItem(query.value("Country").toString()));
    ui->twVklad->setItem(i,3,new
QTableWidgetItem(query.value("Loan").toString()));
    ui->twVklad->setItem(i,4,new
QTableWidgetItem(query.value("Term").toString()));

        i++;
    }
    for (int row = 0; row < ui->twVklad->rowCount(); ++row){
        ui->twVklad->setRowHidden(row, false);
    }
}


void Vklad::add()
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
    // Создать строку запроса
    QString sqlstr = "insert into Vklad(Nazv, Country, Loan, Term) values (?,?,?,?)";
    // Подготовить запрос
    query.prepare(sqlstr);

    // Передать параметры из полей ввода в запрос
    query.bindValue(0,ui->leNazv->text());
    query.bindValue(1,ui->leCountry->text());
    query.bindValue(2,ui->leLoan->text().toInt());
    query.bindValue(3,ui->leTerm->text().toInt());

    if( !query.exec() )
    {
        QMessageBox::critical(this,"Ошибка",query.lastError().text());
        return;
    }

    selectAll();
}

void Vklad::onTableItemClicked(QTableWidgetItem *item) {
    if (item == nullptr) {
        return; // Защита от нулевого указателя
    }

    // Получаем индекс строки, которая была выбрана
    int row = item->row();

    // Получаем данные из выбранной строки
    QString idvk = ui->twVklad->item(row, 0)->text();
    QString nazv = ui->twVklad->item(row, 1)->text();
    QString country = ui->twVklad->item(row, 2)->text();
    QString loan = ui->twVklad->item(row, 3)->text();
    QString term = ui->twVklad->item(row, 4)->text();

    // Отображаем данные в соответствующих Line Edit
    ui->leIDVclad->setText(idvk);
    ui->leNazv->setText(nazv);
    ui->leCountry->setText(country);
    ui->leLoan->setText(loan);
    ui->leTerm->setText(term);

}

void Vklad::del()
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

    int currow = ui->twVklad->currentRow();
    if( currow < 0 )
    {
        QMessageBox::critical(this,"Ошибка","Не выбран вклад!");
        return;
    }

    if( QMessageBox::question(this,"Удалить","Удалить выбранный вклад?",
    QMessageBox::Cancel,QMessageBox::Ok)==QMessageBox::Cancel)
    {
        return;
    }

    QString idvk = ui->twVklad->item(currow, 0)->text();

    QSqlQuery query(dbconn);
    QString sqlstr = "delete from Vklad WHERE IDVclad = ?";
    query.prepare(sqlstr);
    query.bindValue(0, idvk.toInt());

    if( !query.exec() )
    {
        QMessageBox::critical(this,"Error",query.lastError().text());
        return;
    }
    selectAll();
}



void Vklad::edit()
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

    int currow = ui->twVklad->currentRow();
    if( currow < 0 )
    {
        QMessageBox::critical(this,"Ошибка","Не выбран вклад!");
        return;
    }

    QSqlQuery query(dbconn);

    QString sqlstr = "UPDATE Vklad SET Nazv = ?, Country = ?, Loan = ?, Term = ? WHERE IDVclad = ?";
    query.prepare(sqlstr);
        // Передать параметры из полей ввода в запрос
    query.bindValue(0,ui->leNazv->text());
    query.bindValue(1,ui->leCountry->text());
    query.bindValue(2,ui->leLoan->text());
    query.bindValue(3,ui->leTerm->text());
    query.bindValue(4,ui->leIDVclad->text());

        // Выполнить запрос
    if( !query.exec() )
    {
        QMessageBox::critical(this,"Ошибка",query.lastError().text());
        return;
    }
        // и обновить записи в компоненте таблицы
    selectAll();
}

Vklad::~Vklad()
{
    if( dbconn.isOpen())
        dbconn.close();
    delete ui;
}

void Vklad::on_btnExit_clicked()
{
    close();
    admin *adminDialog = new admin;
    adminDialog->setModal(true);
    adminDialog->show();
}

void Vklad::on_btnSearch_clicked()
{
       QString searchText = ui->leSearch->text();
       QString queryStr = "SELECT * FROM Vklad WHERE Country LIKE '%" + searchText + "%'";
       QSqlQuery query;
       if (!query.exec(queryStr)) {
           qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
           return;
       }
       ui->twVklad->clearContents();

       int row = 0;
       while (query.next()) {
           if (ui->twVklad->rowCount() <= row) {
                      ui->twVklad->insertRow(row);
                  }
           ui->twVklad->setItem(row, 0, new QTableWidgetItem(query.value("IDVclad").toString()));
           ui->twVklad->setItem(row, 1, new QTableWidgetItem(query.value("Nazv").toString()));
           ui->twVklad->setItem(row, 2, new QTableWidgetItem(query.value("Country").toString()));
           ui->twVklad->setItem(row, 3, new QTableWidgetItem(query.value("Loan").toString()));
           ui->twVklad->setItem(row, 4, new QTableWidgetItem(query.value("Term").toString()));
           row++;
       }
       if (ui->twVklad->rowCount() > row) {
              ui->twVklad->setRowCount(row);
          }
}
