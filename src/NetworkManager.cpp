/*
    Copyright (C) 2020 Florian Cabot <florian.cabot@hotmail.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "NetworkManager.hpp"

NetworkManager::NetworkManager(AbstractState* networkedState)
    : networkedState(networkedState)
{
	if(networkedState == nullptr)
	{
		return;
	}
	if(server)
	{
		udpUpSocket.bind(QHostAddress::Any,
		                 QSettings().value("network/port").toUInt());
		auto socketPtr(&udpUpSocket);
		auto clientsPtr(&clients);
		auto netTimerPtr(&networkTimer);
		connect(socketPtr, &QUdpSocket::readyRead,
		        [socketPtr, clientsPtr, netTimerPtr]() {
			        while(socketPtr->hasPendingDatagrams())
			        {
				        QNetworkDatagram datagram(socketPtr->receiveDatagram());

				        QString str;
				        quint16 p;
				        qreal ft;
				        QByteArray buf(datagram.data());
				        QDataStream stream(&buf, QIODevice::ReadOnly);
				        stream >> str;
				        stream >> p;
				        stream >> ft;
				        if(str != PROJECT_NAME)
				        {
					        qDebug() << "Received bad header :";
					        qDebug() << str;
					        continue;
				        }

				        bool exists(false);
				        for(auto& c : *clientsPtr)
				        {
					        if(c.addr == datagram.senderAddress()
					           && p == c.port)
					        {
						        c.frameTiming      = ft;
						        c.lastReceivedTime = netTimerPtr->elapsed();
						        qDebug() << "Update client :";
						        qDebug() << c.addr;
						        qDebug() << c.port;
						        qDebug() << c.lastReceivedTime;
						        exists = true;
						        break;
					        }
				        }
				        if(!exists)
				        {
					        clientsPtr->append({datagram.senderAddress(), p, ft,
					                            netTimerPtr->elapsed(), 0});
					        qDebug() << "New client :";
					        qDebug() << datagram.senderAddress();
					        qDebug() << p;
				        }
			        }
		        });
	}
	// UDP client
	else
	{
		udpDownSocket.bind(QSettings().value("network/ip").toUInt());
		auto socketPtr(&udpDownSocket);
		auto netStatePtr(networkedState);
		connect(socketPtr, &QAbstractSocket::readyRead,
		        [socketPtr, netStatePtr]() {
			        QNetworkDatagram datagram(socketPtr->receiveDatagram());

			        QByteArray buf(datagram.data());
			        QDataStream stream(&buf, QIODevice::ReadOnly);
			        netStatePtr->readFromDataStream(stream);
		        });
	}
	networkTimer.start();
}

void NetworkManager::update(float frameTiming)
{
	if(networkedState == nullptr)
	{
		return;
	}
	if(server)
	{
		QByteArray buf;
		QDataStream stream(&buf, QIODevice::WriteOnly);
		networkedState->writeInDataStream(stream);

		// qDebug() << "Sending " + QString::number(buf.size()) + " bytes.";

		for(int i(clients.size() - 1); i >= 0; --i)
		{
			// if hasn't responded in 10 seconds, "disconnect"
			if(networkTimer.elapsed() - clients.at(i).lastReceivedTime > 10000)
			{
				qDebug() << "Client disconnected :";
				qDebug() << clients.at(i).addr;
				qDebug() << clients.at(i).port;
				clients.removeAt(i);
			}
			else if(networkTimer.elapsed() - clients.at(i).lastSentTime
			        > clients.at(i).frameTiming * 1500)
			{
				udpDownSocket.writeDatagram(buf, clients.at(i).addr,
				                            clients.at(i).port);
				clients[i].lastSentTime = networkTimer.elapsed();
			}
		}
	}
	else
	{
		if(networkTimer.elapsed() > 1000)
		{
			QByteArray buf;
			QDataStream stream(&buf, QIODevice::WriteOnly);
			stream << QString(PROJECT_NAME);
			stream << quint16(udpDownSocket.localPort());
			stream << qreal(frameTiming);
			udpUpSocket.writeDatagram(
			    buf, QHostAddress(QSettings().value("network/ip").toString()),
			    QSettings().value("network/port").toUInt());
			networkTimer.restart();
		}
	}
}
