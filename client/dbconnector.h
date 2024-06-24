#pragma once

#include <QDialog>

#include <QMessageBox>
//#include <QtSql>
//#include <QSqlDatabase>
#include "consts.h"

namespace Ui {
class dbConnector;
}

class dbConnector : public QDialog
{
    Q_OBJECT

public:
    explicit dbConnector(QWidget *parent = nullptr);
    ~dbConnector();

    QString getdbName();
    QString getEspHost();

private slots:

    void on_pushButton_cancel_clicked();

    void on_pushButton_login_clicked();

private:
    Ui::dbConnector *ui;

    QString _dbHost = "localhost";
    QString _dbName = "WSdb";
    QString _espHost = WS_ESP8266_WLAN_IP;
};
