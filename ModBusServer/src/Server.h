/*--------------------------------------------------------------------------------------*/
/* 
 * File :		Server.h
 * Author :		JB
 * Date :		12/07/10
 * Comment :	Modbus server
 */
/*--------------------------------------------------------------------------------------*/

#ifndef HEADER_Serveur_H
#define HEADER_Serveur_H
#include <QSqlDatabase>

#include <QCoreApplication>
#include <QtNetwork>
#include "ModbusFrame.h"
#include "ModbusServer.h"
#include <QSqlQuery>
#include <QSettings>

class Server : QObject
{
    Q_OBJECT

public:
    Server(int debug,QString settings);

private slots:
    void newConection();
    void messageReceived();
    void clientDisconnect();

private:
    QTcpServer * server;
    QList<QTcpSocket *> clients;
    QSqlDatabase db;
    ModbusServer * serverMB;
    QSettings *settings;
    QMap<int,ModbusServer::vigie> modbusVigieMap;
    int debug;
    void dummyPopulateSettings(QFile *file);
};

#endif

