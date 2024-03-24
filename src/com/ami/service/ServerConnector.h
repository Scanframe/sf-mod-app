#pragma once
#include "VariableCollector.h"
#include "ServerConnection.h"
#include "misc/qt/QtThreadClosure.h"

namespace sf
{

class ServerConnector :QObject
{
	public:
		explicit ServerConnector(QObject* parent);

		~ServerConnector() override;

		VariableCollector _collector;
		ServerConnection* _connection;
		QtThreadClosure _threadClosure;
};

}




