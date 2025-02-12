#pragma once
#include <ami/service/ServerConnection.h>
#include <ami/service/VariableCollector.h>
#include <misc/qt/QtThreadClosure.h>

namespace sf
{

class ServerConnector : public QObject
{
	public:
		explicit ServerConnector(QObject* parent);

		~ServerConnector() override;

		VariableCollector _collector;
		ServerConnection* _connection;
		QtThreadClosure _threadClosure;
};

}// namespace sf
