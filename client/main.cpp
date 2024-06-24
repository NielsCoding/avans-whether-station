#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCore/QtMath>
#include <QtCore/QRandomGenerator>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QApplication>
#include <QMessageBox>
#include <iostream>
#include <stdio.h>

//#include "chart.h"
//#include "chartview.h"
#include "mainwindow.h"
#include "main.h"
#include "ui_mainwindow.h"

QSqlDatabase dbRef = QSqlDatabase();

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    MainWindow w;

    dbRef = QSqlDatabase::addDatabase("QMYSQL");

//    QString input =  "000e,1d,2f,28\n000d,1d,2f,28\n000c,1d,2f,29\n000b,1d,2f,28\n000a,1d,2f,2a";
//    CSV_Import import;


//    QString output = "INSERT INTO `WSdb`.`tblMain` () VALUES ";
//    QVector<QString> list = input.split("\n");
//    for (int i = 0; i < list.size(); ++i) {
//        output.append(import.csvToSql(list[i]));
//        if (i+1 < list.size()){
//            output.append(",");
//        }
//    }
//    qDebug() << output;

    w.show();
	return a.exec();
}

/* QT_CHARTS_USE_NAMESPACE

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

//![1]
	QLineSeries *series = new QLineSeries();
	for (int i = 0; i < 500; i++) {
		QPointF p((qreal) i, qSin(M_PI / 50 * i) * 100);
		p.ry() += QRandomGenerator::global()->bounded(20);
		*series << p;
	}
//![1]

	Chart *chart = new Chart();
	chart->addSeries(series);
	chart->setTitle("Zoom in/out example");
	chart->setAnimationOptions(QChart::SeriesAnimations);
	chart->legend()->hide();
	chart->createDefaultAxes();

	ChartView *chartView = new ChartView(chart);
	chartView->setRenderHint(QPainter::Antialiasing);

	QMainWindow window;
	window.setCentralWidget(chartView);
	window.resize(400, 300);
	window.grabGesture(Qt::PanGesture);
	window.grabGesture(Qt::PinchGesture);
	window.show();

	return a.exec();
} */
