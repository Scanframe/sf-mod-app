#pragma once

#include "ServerConnection.h"
#include "ServerConnector.h"
#include <QtNetwork/QTcpServer>
#include <QMutex>

namespace sf
{

class SocketServer :public QTcpServer
{
	public:
		/**
		 * @brief Standard Qt constructor passing the owning parent.
		 */
		explicit SocketServer(QObject* parent);

	protected:
		/**
		 * @brief Overridden method to start a thread handling the passed socket descriptor.
		 * @param socketDescriptor
		 */
		void incomingConnection(qintptr socketDescriptor) override;

	private:
		/**
		 * @brief Initializes the instance and starts listening.
		 */
		void initialize();

		/**
		 * @brief Checks the connection for relaying threads.
		 */
		void relayThreads();

		/**
		 * @brief Holds the port number that is listened on.
		 */
		quint16 _portNumber{8001};

		/**
		 * @brief Holds the server connectors.
		 */
		QList<ServerConnector*> _connectors, _cleanup;

		QMutex _mutexList;
};

}