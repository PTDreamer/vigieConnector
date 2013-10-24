#include "ModbusServer.h"
#include <QSqlQuery>
#include <QTcpSocket>

#define MAX_ENERGY_METERS 50
//Functions List

//01 (0x01) Read Coils
//02 (0x02) Read Discrete Inputs
//03 (0x03) Read Holding Registers
//04 (0x04) Read Input Registers
//05 (0x05) Write Single Coil
//06 (0x06) Write Single Register
//15 (0x0F) Write Multiple Coils
//16 (0x10) Write Multiple registers
//20 (0x14) Read File Record										(To Do)
//21 (0x15) Write File Record										(To Do)
//22 (0x16) Mask Write Register										(To Do)
//23 (0x17) Read/Write Multiple registers							(To test)
//24 (0x18) Read FIFO Queue											(To Do)
//43 (0x2B) Encapsulated Interface Transport						(To Do)

ModbusServer::ModbusServer(int numberOfUnit, QString vigieServerIP, quint16 vigieServerPort, int debug, QMap<int, vigie> &modbusVigieMap):vigieServerAdress(vigieServerIP),vigieServerPort(vigieServerPort),debug(debug),threadRun(true),modbusVigieMap(modbusVigieMap)
{
    this->numberOfUnits = numberOfUnit;
    energy * en;
    for(int x=0;x<MAX_ENERGY_METERS;++x)
    {
        en=new energy;
        en->lowside_updated=false;
        en->upperside_updated=false;
        en->scale_updated=false;
        energycheck.append(en);
    }

    QFile temp(":vigie/vigie_template.txt");
    if(!temp.open(QIODevice::ReadOnly))
    {
        if(debug > 0) qDebug()<<"Could not open template file";
        return;
    }
    vigieTemplate = temp.readAll();
    this->start();
}

ModbusFrame ModbusServer::giveRequest(ModbusFrame &requestFrame)
{
    if(debug > 1) qDebug()<<"Received modbus frame"<<requestFrame.toText();
    ModbusFrame responseFrame(requestFrame);
    responseFrame.copyMobdbusHeader(requestFrame);

    int unitId = requestFrame.getUnitId();
    int referenceNumber = requestFrame.getReferenceNumber();
    int wordCount = requestFrame.getWordCount();

    QVector <QVariant> vVar(wordCount);

    if (unitId > numberOfUnits)
    {
        responseFrame.setExceptionCode(ModbusFrame::SlaveDeviceFailure);
        if(debug > 0) qDebug()<<"Error receive modbus frame for a device number not available";
        return responseFrame;
    }
    switch(requestFrame.getFunctionCode())
    {
    case ModbusFrame::WriteMultipleRegisters : //16 (0x10) Write Multiple registers
            vVar = requestFrame.getData(QMetaType::UShort);
            for (int i = 0 ; i < vVar.size() ; i++)
            {
                int adjustedReference = referenceNumber + i;
                {
                    if(debug > 1) qDebug()<<"Received modbus WriteMultipleRegisters frame adress "<<referenceNumber+i;
                    if(adjustedReference<99)
                    {
                        QMutexLocker locker(&readWriteMutex);
                        if(debug>0) qDebug()<<"Received power measurement from adress:"<<adjustedReference + 1<<" with value:"<<vVar[i].toUInt();
                        vigie v = modbusVigieMap.value(adjustedReference + 1);//+1??
                        v.data = vVar[i].toUInt();
                        if(debug>0) qDebug()<<QString("Appending vigie message with id=%0 param=%1 value=%2 from modbus adr=%3").arg(v.id).arg(v.param).arg(v.data.toString()).arg(adjustedReference + 1);
                        vigieList.append(v);
                        threadSleep.wakeAll();
                    }
                    else if (adjustedReference<199)
                    {
                        int adjustedIndex = adjustedReference - 100;
                        if(energycheck[adjustedIndex]->upperside_updated)
                        {
                            if(debug>0) qDebug()<<"ENERGY LOW SIDE WAS ALREADY TRUE";
                        }
                        else
                        {
                            energycheck[adjustedIndex]->upperside_updated=true;
                            energycheck[adjustedIndex]->upperside=vVar[i].toUInt();
                            energycheck[adjustedIndex]->reference=adjustedReference+1;
                            if(debug>0) qDebug()<<"Received energy lower side measurement from adress:"<<adjustedReference+1<<" with value:"<<vVar[i].toUInt()<<" going into index:"<<adjustedIndex;
                        }

                    }
                    else if (referenceNumber<299)
                    {
                        int adjustedIndex = adjustedReference - 200;
                        if(energycheck[adjustedIndex]->lowside_updated)
                        {
                            if(debug>0) qDebug()<<"ENERGY UPPER SIDE WAS ALREADY TRUE";
                        }
                        else
                        {
                            energycheck[adjustedIndex]->lowside_updated=true;
                            energycheck[adjustedIndex]->lowside=vVar[i].toUInt();
                            if(debug>0) qDebug()<<"Received energy upper side measurement from adress:"<<adjustedReference+1<<" with value:"<<vVar[i].toUInt()<<" going into index:"<<adjustedIndex;


                        }
                    }
                    else if (referenceNumber<399)
                    {
                        int adjustedIndex = referenceNumber + i - 300;
                        if(energycheck[referenceNumber+i-300]->scale_updated)
                        {
                            if(debug>0) qDebug()<<"ENERGY SCALE WAS ALREADY TRUE";
                        }
                        else
                        {
                            energycheck[adjustedIndex]->scale_updated=true;
                            energycheck[adjustedIndex]->scale=vVar[i].toUInt();
                            if(debug>0) qDebug()<<"Received energy scale measurement from adress:"<<adjustedReference+1<<" with value:"<<vVar[i].toUInt()<<" going into index:"<<adjustedIndex;
                        }
                    }
                }
            }
            foreach(energy * en,energycheck)
            {
                if(en->lowside_updated && en->upperside_updated && en->scale_updated)
                {

                    quint32 total=(quint16)en->lowside+en->upperside*65536;
                    double exponential=pow(10,en->scale);
                    double temp= total*exponential;
                    en->lowside_updated=false;
                    en->scale_updated=false;
                    en->upperside_updated=false;
                    if(debug) qDebug()<<"temp"<<temp;
                    QMutexLocker locker(&readWriteMutex);
                    vigie v = modbusVigieMap.value(en->reference);
                    v.data = temp;
                    if(debug>0) qDebug()<<QString("(ENERGY) Appending vigie message with id=%0 param=%1 value=%2 from modbus adr=%3").arg(v.id).arg(v.param).arg(v.data.toString()).arg(en->reference);
                    vigieList.append(v);
                    threadSleep.wakeAll();
                }

            }
            responseFrame.setReferenceNumber(requestFrame.getReferenceNumber());
            responseFrame.setWordCount(requestFrame.getWordCount());
            break;
    default :
        break;
    }
    return responseFrame;
}

ModbusServer::~ModbusServer()
{
    threadRun=false;
}

void ModbusServer::run()
{
    QTcpSocket * socket = new QTcpSocket();
    while(threadRun)
    {
        if(vigieList.length()==0)
        {
            threadSleepMutex.lock();
            threadSleep.wait(&threadSleepMutex,1000);
            threadSleepMutex.unlock();
        }
        else
        {
            QMutexLocker locker(&readWriteMutex);
            foreach(vigie vig, vigieList)
            {
                QByteArray array = parseVigie(vig);
                socket->connectToHost(vigieServerAdress,vigieServerPort);
                if(!socket->waitForConnected(5000))
                {
                    if(debug > 0 ) qDebug()<<"Could not opensocket:"<<socket->state()<<socket->error();
                }
                else
                {
                    socket->write(array);
                    socket->write("Hi");
                    socket->waitForBytesWritten(5000);
                    socket->close();

                }
            }
            vigieList.clear();
        }
    }
    socket->deleteLater();
}

QByteArray ModbusServer::parseVigie(vigie value)
{
    QByteArray valueToHash = QString("xpto}%0").arg(value.data.toString()).toLatin1();
    QString temp(vigieTemplate);
    temp.replace("%ROOM_ID","1");
    temp.replace("%SENSOR_ID",QString::number(value.id));
    temp.replace("%PARAMETER_ID",QString::number(value.param));
    temp.replace("%PARAMETER_VALUE",value.data.toString());
    temp.replace("%MD5_VALUE",QString(QCryptographicHash::hash(valueToHash,QCryptographicHash::Md5).toHex()));
    temp.replace("%VOLTAGE","1");
    temp.replace("%SIGNAL_STRENGHT","1");
    temp.replace("%MEASURE_TIMESTAMP",QString::number(QDateTime::currentMSecsSinceEpoch()/1000));
    return temp.toLatin1();
}
