#ifndef MODBUSSERVER_H
#define MODBUSSERVER_H

#include <QtCore>
#include <QtAlgorithms>
#include "ModbusFrame.h"
#include <QSqlDatabase>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QTcpSocket>

class ModbusServer : public QThread
{
    struct energy
    {
        bool lowside_updated;
        bool upperside_updated;
        bool scale_updated;
        quint16 lowside;
        quint16 upperside;
        qint16 scale;
        int reference;

    };

public :
    struct vigie
    {
        int id;
        int param;
        QVariant data;
    };
ModbusServer(int numberOfUnit,QString vigieServerIP, quint16 vigieServerPort, int debug,QMap<int,vigie> &modbusVigieMap);
ModbusFrame giveRequest(ModbusFrame &modbusFrame);
~ModbusServer();

QByteArray parseVigie(vigie value);
private:
QString vigieServerAdress;
quint16 vigieServerPort;
int numberOfUnits;
bool debug;
bool threadRun;
QMutex threadSleepMutex;
QMutex readWriteMutex;
QWaitCondition threadSleep;

QMap<int,vigie> modbusVigieMap;
QVector <energy *> energycheck;
QList <vigie> vigieList;
QString vigieTemplate;
protected:
void run();
};



#endif // MODBUSSERVER_H
