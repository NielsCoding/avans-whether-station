#include <QSqlDatabase>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "main.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client.ClientEcho();
    _dbConenctor = new dbConnector(this);
}

MainWindow::~MainWindow()
{
    dbRef.close();
    delete _dbConenctor;
    delete ui;
}

void MainWindow::timeFunction()
{
    client.timeFunction();
}

void MainWindow::on_actionConnection_triggered()
{
//    _dbConenctor->preLoadUi();
    _dbConenctor->show();
}

void MainWindow::on_actionRefresh_triggered()
{
    drawGraph();
}

void MainWindow::on_actionDisconnenct_triggered()
{
    dbRef.close();
}


void MainWindow::drawGraph(){
    QLineSeries *seriesTemperature = new QLineSeries();
    QLineSeries *seriesHumidity = new QLineSeries();
    QLineSeries *seriesPressure = new QLineSeries();


    if(dbRef.open()){
        QSqlQuery queryGraphData;
//        queryGraphData.exec("select `tblMain`.`ID`, `temperature`, `humidity`, `pressure` FROM `tblMain` ORDER BY `tblMain`.`ID` DESC limit 16;");
        queryGraphData.exec("select * FROM `tblMain` ORDER BY `tblMain`.`ID` DESC limit 190;");

        for (int i = 0; queryGraphData.next(); ++i) {
//            int time = queryGraphData.value(4).toTime().hour()*100 + queryGraphData.value(4).toTime().minute();
            int time = i;

            seriesTemperature->append(time, queryGraphData.value(1).toFloat());
            seriesHumidity->append(time, queryGraphData.value(2).toFloat());
            seriesPressure->append(time, (queryGraphData.value(3).toFloat()*100));

        }

//        QPen pen(QRgb(0x57FF00));
//        pen.setWidth(5);
//        seriesHumidity->setPen(pen);


        QChart *chartTemperature = new QChart();
        chartTemperature->legend()->hide();
        chartTemperature->addSeries(seriesTemperature);
        chartTemperature->createDefaultAxes();
        chartTemperature->setTitle("Temperature!");
        QChartView *chartView1 = new QChartView(chartTemperature);

        QChart *chartHumidity = new QChart();
        chartHumidity->legend()->hide();
        chartHumidity->addSeries(seriesHumidity);
        chartHumidity->createDefaultAxes();
        chartHumidity->setTitle("Humidity!");
        QChartView *chartView2 = new QChartView(chartHumidity);

        QChart *chartPressure = new QChart();
        chartPressure->legend()->hide();
        chartPressure->addSeries(seriesPressure);
        chartPressure->createDefaultAxes();
        chartPressure->setTitle("Pressure!");
        QChartView *chartView3 = new QChartView(chartPressure);


        QWidget *window = new QWidget;
        QGridLayout *layout = new QGridLayout(window);
        layout->addWidget(chartView1, 0 ,0);
        layout->addWidget(chartView2, 1 ,0);
        layout->addWidget(chartView3, 2 ,0);

        layout->setVerticalSpacing(0);
        layout->setHorizontalSpacing(0);
        layout->setContentsMargins(0,0,0,0);


        MainWindow::setCentralWidget(window);

    } else {
        QMessageBox::warning(this, "No connection", "Failed to connect");
    }
}

