#include "admin.h"
#include "ui_admin.h"
#include "client.h"
#include "vklad.h"
#include "schet.h"
#include "kredit.h"
#include "clientkredit.h"
#include "clientvklad.h"

admin::admin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::admin)
{
    ui->setupUi(this);
    mainWindow = dynamic_cast<MainWindow*>(parent);
    ui->btnSchet->setVisible(false);

}

admin::~admin()
{
    delete ui;
}


void admin::on_btnExit_clicked()
{
    if(mainWindow){

        mainWindow->show();
    }
    close();
}


void admin::on_btnClient_clicked()
{
    close();
    client clnt;
    clnt.setModal(true);
    clnt.exec();
}

void admin::on_btnVklad_clicked()
{
    close();
    Vklad vkld;
    vkld.setModal(true);
    vkld.exec();
}

void admin::on_btnSchet_clicked()
{

    close();
    Schet scht;
    scht.setModal(true);
    scht.exec();
}

void admin::on_btnKredit_clicked()
{
    close();
    Kredit krdt;
    krdt.setModal(true);
    krdt.exec();
}


