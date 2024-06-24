#include "HandleMessage.h"
#include "../shared/util.h"

HandleMessage::HandleMessage(QObject *parent) : QObject(parent) { }

void HandleMessage::ParseToSQL(QString input) {
	QSqlQuery queryInsertData;
	QString output = "INSERT INTO `tblMain` (`temperature`, `humidity`, `pressure`) VALUES ";
	QStringList data;
	QStringList list = input.split("\n",Qt::SkipEmptyParts);
	for (int i = 0; i < list.size(); ++i) {
		data=list[i].split(",");
		bool valid;

		output.append("(");
		output.append(QString::number(ws_sensor_tmp_to_f(data[1].toInt(&valid, 16))));
		output.append(",");
		output.append(QString::number(ws_sensor_hum_to_f(data[2].toInt(&valid, 16))));
		output.append(",");
		output.append(QString::number(ws_sensor_atm_to_f(data[3].toInt(&valid, 16))));
		output.append(")");

		if (i+1 < list.size()) output.append(",");
	}
	queryInsertData.exec(output);
}

