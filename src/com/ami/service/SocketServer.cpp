#include "SocketServer.h"
#include "ServerConnection.h"
#include <misc/qt/QtThreadClosure.h>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkInterface>
#include <QTimer>

namespace sf
{

SocketServer::SocketServer(QObject* parent)
	:QTcpServer(parent)
{
	initialize();
	auto* timer = new QTimer(this);
	timer->setInterval(1000);
	connect(timer, &QTimer::timeout, this, &SocketServer::relayThreads);
	timer->start();
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
	for (const auto& entry: ipAddressesList)
	{
		if (entry != QHostAddress::LocalHost && entry.toIPv4Address() != 0)
		{
			ipAddress = entry.toString();
			break;
		}
	}
	// If no address was found use IPv4 localhost
	if (ipAddress.isEmpty())
	{
		ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
	}
	//
	qInfo() << QString("Listening on IP (%1) port (%2)").arg(ipAddress).arg(serverPort());
}

void SocketServer::relayThreads()
{
	QMutexLocker lock(&_mutexList);
	for (auto* connector: _connectors)
	{
		connector->_connection->relayThread();
	}
	// Remove disconnected.
	for (auto* connector: _cleanup)
	{
		delete connector;
	}
	_cleanup.clear();
}

void SocketServer::incomingConnection(qintptr socketDescriptor)
{
	// Lock for the connectors.
	QMutexLocker lock(&_mutexList);
	auto* connector = new ServerConnector(this);
	_connectors.append(connector);
	// When thread has finished cleanup.
	connect(&connector->_threadClosure, &QThread::finished, [this, connector]()
	{
		QMutexLocker lock(&_mutexList);
		_connectors.removeOne(connector);
		_cleanup.append(connector);
	});
	// Assign thread function to handle the connection.
	auto fn = [this, connector, socketDescriptor](QThread& thread, QObject* parent)
	{
		// Create socket from this thread.
		auto* socket = new QTcpSocket(parent);
		connect(socket, &QAbstractSocket::disconnected, [this, socket]()
		{
			(void)this;
			qInfo() << SF_RTTI_NAME(this).c_str() << "Disconnected...";
			socket->close();
		});
		// Initialize the socket using a descriptor.
		if (!socket->setSocketDescriptor(socketDescriptor))
		{
			qDebug() << "SocketError:" << socket->error() << socket->errorString();
			return;
		}
		// Report PMTU, send buffer and receive buffer sizes.
		qDebug() << "PMTU, Send, Receive"
			<< socket->socketOption(QAbstractSocket::PathMtuSocketOption)
			<< socket->socketOption(QAbstractSocket::SendBufferSizeSocketOption)
			<< socket->socketOption(QAbstractSocket::ReceiveBufferSizeSocketOption);
		// Create the server connection.
		connector->_connection = new ServerConnection(socket, parent);
		// Set the socket to be part of the connection.
		socket->setParent(connector->_connection);
		// Do as long as the thread is not requested to terminate.
		while (!thread.isInterruptionRequested())
		{
			// Non-blocking method processing in coming and out going data.
			if (!socket->isOpen() || !connector->_connection->process())
			{
				// Break the loop as requested.
				qInfo() << SF_RTTI_NAME(this).c_str() << "Loop stopped...";
				break;
			}
		}
		// Clean up.
		socket->disconnectFromHost();
		if (socket->state() != QAbstractSocket::UnconnectedState)
		{
			socket->waitForDisconnected();
		}
		qInfo() << SF_RTTI_NAME(this).c_str() << "Thread stopped...";
	};
	// Assign the thread function to the closure.
	connector->_threadClosure.assign(fn);
	// Start the thread.
	connector->_threadClosure.start();
}

}
