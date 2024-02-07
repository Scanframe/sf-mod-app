#include "SocketClient.h"
#include <QTcpSocket>
#include <QIODevice>

namespace sf
{

sf::SocketClient::SocketClient(QObject* parent)
	:QThread(parent)
	 , _socket(nullptr)
	 , _connection(nullptr)
{
	_socket = new QTcpSocket();
	_socket->moveToThread(this);
	_connection = new ClientConnection(_socket);
	// Start the thread.
	start();
}

SocketClient::~SocketClient()
{
	requestInterruption();
	_waitCondition.wakeAll();
	quit();
	wait();
	delete _socket;
	delete _connection;
}

void SocketClient::connectHost(const QString& hostName, int portNumber)
{
	Lock lock(this);
	_hostName = hostName;
	_portNumber = portNumber;
	if (isRunning())
	{
		_waitCondition.wakeOne();
	}
	else
	{
		start();
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

void SocketClient::run()
{
	while (!isInterruptionRequested())
	{
		// Lock the mutex so the condition can act on it.
		QMutexLocker wait_lock(&_mutex);
		// Wait for a signal from the main thread.
		_waitCondition.wait(&_mutex);
		// When the thread supposed to quit break the loop.
		if (isInterruptionRequested())
		{
			break;
		}
		// Disconnect abruptly if a connection still exists.
		if (_socket->isOpen())
		{
			_socket->abort();
			_socket->close();
		}
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
		//
		while (!isInterruptionRequested() && _socket->state() != QAbstractSocket::UnconnectedState && _socket->isOpen())
		{
			// Non-blocking method processing in coming and out going data.
			if (!_connection->process())
			{
				qInfo() << rtti_name(*this).c_str() << "Disconnecting...";
				_socket->disconnectFromHost();
				if (_socket->state() != QAbstractSocket::UnconnectedState)
				{
					_socket->waitForDisconnected();
				}
				break;
			}
		}
	}
	qInfo() << SF_RTTI_NAME(*this).c_str() << "Thread stopped...";
}

}