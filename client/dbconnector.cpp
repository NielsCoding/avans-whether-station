#include "dbconnector.h"
#include "ui_dbconnector.h"
#include "main.h"

#include "mainwindow.h"

dbConnector::dbConnector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dbConnector)
{
    ui->setupUi(this);
    ui->lineEdit_espAdress->setText(_espHost);
    ui->lineEdit_database->setText(_dbName);
    ui->lineEdit_adress->setText(_dbHost);
}

dbConnector::~dbConnector()
{
    delete ui;
}

void dbConnector::on_pushButton_cancel_clicked()
{
    close();
}

void dbConnector::on_pushButton_login_clicked()
{
    _espHost = ui->lineEdit_espAdress->text();
    _dbHost = ui->lineEdit_adress->text();
    _dbName = ui->lineEdit_database->text();
    QString username = ui->lineEdit_username->text();
    QString password = ui->lineEdit_password->text();

    // Extract possible port (default if not provided)
    QStringList dbAdress = _dbHost.split(":");
    if (dbAdress.size() > 1){
        dbRef.setPort(dbAdress[1].toInt());
        qDebug() << dbAdress[1];
    }

    dbRef.setHostName(dbAdress[0]);
    dbRef.setUserName(username);
    dbRef.setPassword(password);
    dbRef.setDatabaseName(_dbName);

    if(dbRef.open()){
        QMessageBox::information(this, "Connection", "GREAT SUCCES!");
        close();
    } else {
        QMessageBox::warning(this, "No connection", "Failed to connect");
    }
}

QString dbConnector::getdbName(){
    return _dbName;
}

QString dbConnector::getEspHost(){
    return _espHost;
}
