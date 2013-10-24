/**
 ******************************************************************************
 * @file       mytcpserver.cpp
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2013
 * @addtogroup [Group]
 * @{
 * @addtogroup MyTcpServer
 * @{
 * @brief [Brief]
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "mytcpserver.h"

MyTcpServer::MyTcpServer(QObject *parent) :
    QObject(parent)
{
    server = new QTcpServer(this);

    // whenever a user connects, it will emit signal
    connect(server, SIGNAL(newConnection()),
            this, SLOT(newConnection()));
    if(!server->listen(QHostAddress::Any, 4444))
    {
        qDebug() << "Server could not start";
    }
    else
    {
        qDebug() << "Server started!";
    }
}

void MyTcpServer::newConnection()
{
    qDebug()<<"new connection";
    // need to grab the socket
    QTcpSocket *socket = server->nextPendingConnection();
    //connect(socket,SIGNAL(readyRead()),this,SLOT(outputSocket()));
   // while(true)
    {
        if(socket->waitForReadyRead(1000))
            qDebug()<<socket->readAll();
        if(!socket->isOpen())
        {
            qDebug()<<"closed";
            return;
}
        }
}
