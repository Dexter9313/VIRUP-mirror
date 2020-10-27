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

#ifndef NETWORKMANAGER_HPP
#define NETWORKMANAGER_HPP

#include <QElapsedTimer>
#include <QHostAddress>
#include <QMatrix4x4>
#include <QNetworkDatagram>
#include <QSettings>
#include <QUdpSocket>

class NetworkManager : public QObject
{
	Q_OBJECT
  public:
	NetworkManager();
	bool isServer() const { return server; };
	void update(float frameTiming, QMatrix4x4& syncedView);

  private:
	bool server = QSettings().value("network/server").toBool();
	QUdpSocket udpUpSocket;   // client to server
	QUdpSocket udpDownSocket; // server to client
	QElapsedTimer networkTimer;

	// client
	QMatrix4x4 networkedView; // variable to share
	// server
	struct Client
	{
		QHostAddress addr;
		quint16 port;
		qreal frameTiming;
		qint64 lastReceivedTime;
		qint64 lastSentTime;
	};
	QList<Client> clients;
};

#endif // NETWORKMANAGER_HPP
