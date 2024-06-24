#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QTextStream>
#include <QTimer>
#include <QDateTime>
#include <QSqlQuery>

#include "HandleMessage.h"
#include "../shared/wifi.h"

// class client for wheather station
class Client : public QObject
{
    Q_OBJECT
public:
    Client(QObject *parent = 0);
    virtual ~Client();

public slots:
    void ClientEcho(); // function to ask data from wheather station
    void timeFunction();  // function to look every second what time currently is en handle if minute is passed

private:
    void missingRecords();

    int _missingRecords = 0;
    QTcpSocket *socket; // tcpsocket for communicating
    QTimer      *timer; // timer to read every second what time it curruntly is.

    // qint16 currentMinute; // timing for currentMinute
    HandleMessage Handlemsg; // add HandleMessage to Client.h

    int tcpPortAddress = WS_SERVER_PORT; // port of communication via tcp
    QString networkAddress = WS_ESP8266_WLAN_IP; // network address for commincation via tcp

    uint8_t totalRecords = 1;  // total records to ask wheather station
    uint8_t offsetRecords = 0; // offset from reqeusting records
};

#endif // CLIENT_H
