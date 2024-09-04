#include "kredit.h"
#include "ui_kredit.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <string>

Kredit::Kredit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Kredit)
{
    ui->setupUi(this);

    connect(ui->btnConnect,SIGNAL(clicked(bool)),this, SLOT(dbconnect()));
    connect(ui->btnSelectAll,SIGNAL(clicked(bool)),this, SLOT(selectAll()));
    connect(ui->btnAdd,SIGNAL(clicked(bool)),this, SLOT(add()));
    connect(ui->btnDel,SIGNAL(clicked(bool)),this, SLOT(del()));
    connect(ui->btnEdit, SIGNAL(clicked(bool)), this, SLOT(edit()));
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


void Kredit::dbconnect()
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


void Kredit::selectAll()
{
    ui->twKredit->clearContents();
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
    QSqlQuery query(dbconn);
    QString sqlstr = "select * from Kredit";
    if( !query.exec(sqlstr) )
    {
        QMessageBox::critical(this,"Ошибка", query.lastError().text());
        return;
    }
    if( query.isActive())
        ui->twKredit->setRowCount( query.size());
    else
        ui->twKredit->setRowCount( 0);
    int i=0;
    while(query.next())
{

    ui->twKredit->setItem(i,0,new
QTableWidgetItem(query.value("IDKredit").toString()));
    ui->twKredit->setItem(i,1,new
QTableWidgetItem(query.value("Nazv").toString()));
    ui->twKredit->setItem(i,2,new
QTableWidgetItem(query.value("Country").toString()));
    ui->twKredit->setItem(i,3,new
QTableWidgetItem(query.value("Loan").toString()));
    ui->twKredit->setItem(i,4,new
QTableWidgetItem(query.value("Term").toString()));

        i++;
    }
    for (int row = 0; row < ui->twKredit->rowCount(); ++row){
        ui->twKredit->setRowHidden(row, false);
    }
}


void Kredit::add()
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
    QString sqlstr = "insert into Kredit(Nazv, COuntry, Loan, Term) values (?,?,?,?)";
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


void Kredit::onTableItemClicked(QTableWidgetItem *item) {
    if (item == nullptr) {
        return; // Защита от нулевого указателя
    }

    // Получаем индекс строки, которая была выбрана
    int row = item->row();

    // Получаем данные из выбранной строки
    QString krid = ui->twKredit->item(row, 0)->text();
    QString nazv = ui->twKredit->item(row, 1)->text();
    QString country = ui->twKredit->item(row, 2)->text();
    QString loan = ui->twKredit->item(row, 3)->text();
    QString term = ui->twKredit->item(row, 4)->text();


    ui->leID->setText(krid);
    ui->leNazv->setText(nazv);
    ui->leCountry->setText(country);
    ui->leLoan->setText(loan);
    ui->leTerm->setText(term);

}


void Kredit::del()
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
    int currow = ui->twKredit->currentRow();
    if( currow < 0 )
    {
        QMessageBox::critical(this,"Ошибка","Не выбран пользователь!");
        return;
    }
    if( QMessageBox::question(this,"Удаление","Удалить кредит?",
    QMessageBox::Cancel,QMessageBox::Ok)==QMessageBox::Cancel)
    return;
    // Создать объект запроса
    QSqlQuery query(dbconn);

    QString sqlstr = "delete from Kredit WHERE IDKredit = '"+ ui->twKredit->item(currow,0)->text() + "'";
    // Выполнить строку запроса и проверить его успешность
    if( !query.exec(sqlstr) )
    {
        QMessageBox::critical(this,"Ошибка",query.lastError().text());
        return;
    }
    // Обновить содержимое компонента таблицы
    selectAll();
}


void Kredit::edit()
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

    int currow = ui->twKredit->currentRow();
        // Если он меньше 0 (строка не выбрана), то
        // сообщение об ошибке и выход из функции
    if( currow < 0 )
    {
        QMessageBox::critical(this,"Ошибка","Не выбран пользователь!");
        return;
    }
        // Спросить у пользователя подтверждение удаления записи
        // Используется статический метод QMessageBox::question
        // для задания вопроса, который возвращает код нажатой кнопки

    QSqlQuery query(dbconn);

    QString sqlstr = "UPDATE Kredit SET Nazv = ?, Country = ?, Loan = ?, Term = ? WHERE IDKredit = '"+ ui->twKredit->item(currow,0)->text() + "'";
        // Подготовить запрос
    query.prepare(sqlstr);
        // Передать параметры из полей ввода в запрос
    query.bindValue(0,ui->leNazv->text());
    query.bindValue(1,ui->leCountry->text());
    query.bindValue(2,ui->leLoan->text());
    query.bindValue(3,ui->leTerm->text());

        // Выполнить запрос
    if( !query.exec() )
    {
        QMessageBox::critical(this,"Ошибка",query.lastError().text());
        return;
    }
        // и обновить записи в компоненте таблицы
    selectAll();
}


Kredit::~Kredit()
{
    if( dbconn.isOpen())
        dbconn.close();
    delete ui;
}

void Kredit::on_btnExit_clicked()
{

  close();
admin *adminDialog = new admin;
adminDialog->setModal(true);
adminDialog->show();
}

void Kredit::on_btnSearch_clicked()
{
    // Получение текста из поля ввода поиска
       QString searchText = ui->leSearch->text();

       // Формирование SQL-запроса для поиска
       QString queryStr = "SELECT * FROM Kredit WHERE Country LIKE '%" + searchText + "%'";

       // Выполнение запроса к базе данных
       QSqlQuery query;
       if (!query.exec(queryStr)) {
           qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
           return;
       }

       // Очистка предыдущих результатов поиска
       ui->twKredit->clearContents();

       // Отображение результатов поиска на странице "Кредит"
       int row = 0;
       while (query.next()) {
           if (ui->twKredit->rowCount() <= row) {
                      ui->twKredit->insertRow(row);
                  }
           ui->twKredit->setItem(row, 0, new QTableWidgetItem(query.value("Nazv").toString()));
           ui->twKredit->setItem(row, 1, new QTableWidgetItem(query.value("Country").toString()));
           ui->twKredit->setItem(row, 2, new QTableWidgetItem(query.value("Loan").toString()));
           ui->twKredit->setItem(row, 3, new QTableWidgetItem(query.value("Term").toString()));
           row++;
       }
       if (ui->twKredit->rowCount() > row) {
              ui->twKredit->setRowCount(row);
          }
}
