#include "SocketServer.h"
#include <misc/qt/QThreadClosure.h>
#include <QDateTime>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QNetworkInterface>

namespace sf
{

SocketServer::SocketServer(QObject* parent)
	:QTcpServer(parent)
{
	initialize();
}

void SocketServer::initialize()
{
	// TODO: Listen address and port should be configurable.
	if (!listen(QHostAddress::Any, _portNumber))
	{
		qWarning() << QString("Unable to start the server: %1.").arg(errorString());
		return;
	}
	QString ipAddress;
	auto ipAddressesList = QNetworkInterface::allAddresses();
	for (const auto& i: ipAddressesList)
	{
		if (i != QHostAddress::LocalHost && i.toIPv4Address())
		{
			ipAddress = i.toString();
			break;
		}
	}
	// If no address was found use IPv4 localhost
	if (ipAddress.isEmpty())
	{
		ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
	}
	//
	qDebug() << QString("Running on IP (%1) port (%2)").arg(ipAddress).arg(serverPort());
}


void SocketServer::incomingConnection(qintptr socketDescriptor)
{
	auto* tc = new QThreadClosure([this, socketDescriptor](QThread& thread)
	{
		auto tcpSocket = new QTcpSocket(&thread);
		if (!tcpSocket->setSocketDescriptor(socketDescriptor))
		{
			qDebug() << "SocketError:" << tcpSocket->error() << tcpSocket->errorString();
			return;
		}
		QDateTime date = QDateTime::currentDateTime();
		QString formattedTime = date.toString("dd.MM.yyyy hh:mm:ss.zzz");
		QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();
		tcpSocket->write(formattedTimeMsg);
		tcpSocket->disconnectFromHost();
		tcpSocket->waitForDisconnected();
	}, this);
	// When thread has finished cleanup.
	connect(tc, &QThread::finished, tc, &QThread::deleteLater);
	// Start the thread.
	tc->start();
}

}
