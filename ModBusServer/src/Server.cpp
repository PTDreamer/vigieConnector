#include "Server.h"
#include <QTimer>
#include <QDateTime>
#include <QDomDocument>
#define POPULATE_SETTINGS FALSE

Server::Server(int debug, QString settings, bool &result):debug(debug)
{
    result = false;
    QDomDocument doc("virtual_modbus");
    if(settings.isEmpty())
        settings=qApp->applicationDirPath()+QDir::separator()+"settings.xml";

    QFile file(settings);
#if(POPULATE_SETTINGS)
    dummyPopulateSettings(&file);
#endif

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"ERROR could not open settings file";
        qDebug()<<"Please define settings file using the CLI -settings= option or place a settings.xml file on the same directory as this binary";
        return;
    }

    QString error;
    int line;
    int col;
    if(!doc.setContent(&file,&error,&line,&col))
    {
        qDebug()<<"ERROR could not parse settings file:"<<error<<line<<col;
    }

    if(debug>0) qDebug()<<"Adding new devices from settings file";

    QDomElement docElem = doc.documentElement();
    QDomNode n = docElem.namedItem("devices").firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        int modbus_adr = e.namedItem("modbus_adr").toElement().text().toInt();
        ModbusServer::vigie vig;
        vig.id =  e.namedItem("sensor_id").toElement().text().toInt();
        vig.param =  e.namedItem("parameter_id").toElement().text().toInt();
        modbusVigieMap.insert(modbus_adr,vig);
        if(debug>0) qDebug()<<QString("Added modbus device with adress %0 mapped to vigie id %1 parameter %2").arg(modbus_adr).arg(vig.id).arg(vig.param);
        n = n.nextSibling();
    }
    n = docElem.namedItem("vigie_settings");
    QDomElement e = n.toElement();
    QString vigie_ip = e.namedItem("ip").toElement().text();
    int vigie_port = e.namedItem("port").toElement().text().toInt();
    n = docElem.namedItem("modbus_settings");
    e = n.toElement();
    int modbus_port = e.namedItem("modbus_port").toElement().text().toInt();

    file.close();

    if(debug>0) qDebug()<<QString("Starting modbus client on port %0, sending to vigie server on ip %1 port %2 ").arg(modbus_port).arg(vigie_ip).arg(vigie_port);

    serverMB = new ModbusServer(1,vigie_ip,vigie_port,debug,modbusVigieMap);

    server = new QTcpServer(this);

    if (server->listen(QHostAddress::Any, modbus_port))
    {
        if(debug>0) qDebug() << "The server started on port:" + QString::number(server->serverPort());
        connect(server, SIGNAL(newConnection()), this, SLOT(newConection()));
        result = true;
    }
    else if(debug>0) qDebug() << "The server was unable to start. Reason:" + server->errorString();
}

void Server::newConection()
{
    QTcpSocket * newClient = server->nextPendingConnection();
    newClient->socketOption(QAbstractSocket::LowDelayOption);
    clients << newClient;

    if(debug>0) qDebug() << "There are " + QString::number(clients.count()) + " clients connected.";

    connect(newClient, SIGNAL(readyRead()), this, SLOT(messageReceived()));
    connect(newClient, SIGNAL(disconnected()), this, SLOT(clientDisconnect()));
}

void Server::messageReceived()
{
    QTcpSocket * socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0) return;
    QList <ModbusFrame> requests = ModbusFrame::tcpMbFrameParser(socket->readAll());
    foreach (ModbusFrame request, requests)
    {
        ModbusFrame frameMB(request);
        ModbusFrame response(serverMB->giveRequest(frameMB));
        socket->write(response.toQByteArray());
    }
}

void Server::clientDisconnect()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0) return;
    clients.removeOne(socket);
    socket->deleteLater();
}

void Server::dummyPopulateSettings(QFile *file)
{
    QDomDocument doc("virtual_modbus");
    file->remove();
    if (!file->open(QIODevice::ReadWrite))
        return;
    QDomElement root = doc.createElement("settings");
    QDomElement vigieRoot = doc.createElement("vigie_settings");
    QDomElement modBusRoot = doc.createElement("modbus_settings");

    doc.appendChild(root);
    root.appendChild(vigieRoot);
    root.appendChild(modBusRoot);
    QDomElement tag_modbus_port = doc.createElement("modbus_port");
    modBusRoot.appendChild(tag_modbus_port);
    QDomText text = doc.createTextNode("502");
    tag_modbus_port.appendChild(text);
    QDomElement tag = doc.createElement("ip");
    vigieRoot.appendChild(tag);

    QDomText t = doc.createTextNode("127.0.0.1");
    tag.appendChild(t);

    tag = doc.createElement("port");
    vigieRoot.appendChild(tag);

    t = doc.createTextNode("4444");
    tag.appendChild(t);

    QDomElement xx=doc.createElement("devices");
    root.appendChild(xx);

    for(int x=0;x<46;++x)
    {
        QDomElement ele1 = doc.createElement("device");
        xx.appendChild(ele1);
        QDomElement mod = doc.createElement("modbus_adr");
        QDomElement adr = doc.createElement("sensor_id");
        QDomElement param = doc.createElement("parameter_id");
        QDomText modt = doc.createTextNode(QString::number(x));
        mod.appendChild(modt);
        QDomText adrt = doc.createTextNode(QString::number(x));
        adr.appendChild(adrt);
        QDomText paramt = doc.createTextNode("31");
        param.appendChild(paramt);
        ele1.appendChild(mod);
        ele1.appendChild(adr);
        ele1.appendChild(param);
    }

    file->write(doc.toByteArray());
    file->close();
}
