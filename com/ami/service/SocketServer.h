#pragma once

#include <QtNetwork/QTcpServer>

namespace sf
{

class SocketServer :public QTcpServer
{
	public:
		explicit SocketServer(QObject* parent);

	protected:
		void incomingConnection(qintptr socketDescriptor) override;

	private:
		void initialize();
		quint16 _portNumber{8001};
};

}