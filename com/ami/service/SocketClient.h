#pragma once

#include <misc/gen/Sync.h>
#include <misc/gen/Condition.h>
#include <QThread>
#include <QAbstractSocket>

namespace sf
{

class SocketClient :public QThread, protected Sync
{
	Q_OBJECT

	public:
		explicit SocketClient(QObject* parent);

		~SocketClient() override;

		QAbstractSocket* _socket{nullptr};

		void connectHost(const QString& hostName, int portNumber);

	private:

		void run() override;

		void readMessage();

		void handleError(QAbstractSocket::SocketError socketError) const;

		QString _hostName;
		int _portNumber{0};
		bool _shouldQuit{false};
		// Timeout in milliseconds.
		int _waitTimeout{5000};
		// Mutex for the condition.
		Mutex _mutex;
		//
		Condition _waitCondition;
};

}