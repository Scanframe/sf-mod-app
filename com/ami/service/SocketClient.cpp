#include "SocketClient.h"
#include <QTcpSocket>
#include <QIODevice>

namespace sf
{

sf::SocketClient::SocketClient(QObject* parent)
	:QThread(parent)
{
	//connect(_socket, &QIODevice::readyRead, this, &SocketClient::readMessage);
	//connect(_socket, &QAbstractSocket::errorOccurred,	this, &SocketClient::handleError);
	// Start the thread.
	start();
}

SocketClient::~SocketClient()
{
	{
		Lock lock(this);
		_shouldQuit = true;
	}
	_waitCondition.notifyAll(_mutex);
	wait();
}

void SocketClient::connectHost(const QString& hostName, int portNumber)
{
	Lock lock(this);
	_hostName = hostName;
	_portNumber = portNumber;
	if (!isRunning())
	{
		start();
	}
	else
	{
		_waitCondition.notifyOne(_mutex);
	}
}

void SocketClient::handleError(QAbstractSocket::SocketError socketError) const
{
	switch (socketError)
	{
		case QAbstractSocket::RemoteHostClosedError:
			qInfo() << "The remote host closed the connection.";
			break;
		case QAbstractSocket::HostNotFoundError:
			qInfo() << "The host was not " << _hostName << " found. Please check the host name and port settings.";
			break;
		case QAbstractSocket::ConnectionRefusedError:
			qInfo() << "The connection was refused by the peer.";
			break;
		default:
			qWarning() << "The following error occurred:" << _socket->errorString();
	}
}

void SocketClient::readMessage()
{
	if (_socket->bytesAvailable())
	{
		auto ba = _socket->readAll();
		qDebug() << __FUNCTION__ << QString(ba);
	}
}

void SocketClient::run()
{
	// Need to create TCP socket here on since it is not allowed in the main thread.
	QScopedPointer<QAbstractSocket> socket(_socket = new QTcpSocket);
	//
	while (!_shouldQuit)
	{
		// Lock the mutex so the condition can act on it.
		Mutex::Lock wait_lock(_mutex);
		// Wait for a signal from the main thread.
		_waitCondition.wait(_mutex);
		// When the thread supposed to quit break the loop.
		if (_shouldQuit)
		{
			break;
		}
		// Disconnect abruptly if a connection still exists.
		_socket->abort();
		// Scope for Lock instance for access to data members.
		{
			Lock sync_lock(this);
			_socket->connectToHost(_hostName, _portNumber);
		}
		if (!_socket->waitForConnected(_waitTimeout))
		{
			handleError(_socket->error());
			break;
		}
		do
		{
			if (!_socket->waitForReadyRead(_waitTimeout))
			{
				handleError(_socket->error());
			}
			else
			{
				readMessage();
			}
		}
		while (!_shouldQuit && _socket->state() != QAbstractSocket::UnconnectedState);
	}
	qInfo() << "Thread stopped...";
}

}