#pragma once
#include "ClientConnection.h"
#include <misc/gen/Sync.h>
#include <misc/gen/Condition.h>
#include "misc/qt/QtSync.h"
#include <QThread>
#include <QAbstractSocket>
#include <QMutex>
#include <QWaitCondition>

namespace sf
{

class SocketClient :public QThread, protected QtSync
{
	public:
		explicit SocketClient(QObject* parent);

		~SocketClient() override;

		void connectHost(const QString& hostName, int portNumber);

	private:

		void run() override;

		void handleError(QAbstractSocket::SocketError socketError) const;

		QString _hostName;
		int _portNumber{0};
		// Timeout in milliseconds.
		int _waitTimeout{5000};
		// Mutex for the condition.
		QMutex _mutex;
		//
		QAbstractSocket* _socket{nullptr};
		//
		QWaitCondition _waitCondition;
		//
		ClientConnection* _connection;

	// LLDB Crashes and GDB freezes when this MACRO is in top of the class.
	Q_OBJECT
};

}