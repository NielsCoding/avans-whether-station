#pragma once

#include <QMainWindow>

//#include "database.h"
#include "dbconnector.h"

#include <QMessageBox>
#include <QtSql>
#include <QSqlDatabase>
#include <QtCharts>
#include <QWidget>
#include <QWidgetSet>

#include "main.h"
#include "Client.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    virtual void drawGraph();

private slots:
//    void on_actionAbout_triggered();

//    void on_pushButton_clicked();
    void timeFunction();
    void on_actionConnection_triggered();

    void on_actionRefresh_triggered();

    void on_actionDisconnenct_triggered();

private:
    Ui::MainWindow *ui;
    Client client;
    dbConnector *_dbConenctor;

    QChart *_pChart;
//    QLineSeries *_pSeries;
    QChartView *_pChartView;

};
