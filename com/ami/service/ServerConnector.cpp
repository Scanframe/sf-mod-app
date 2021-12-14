#include "ServerConnector.h"

namespace sf
{

ServerConnector::ServerConnector(QObject* parent)
	:QObject(parent)
	 , _connection(nullptr)
	 , _threadClosure(this)
{
	qDebug() << SF_Q_RTTI_TYPENAME << Thread::getCurrentId();
}

ServerConnector::~ServerConnector()
{
	qDebug() << SF_Q_RTTI_TYPENAME << Thread::getCurrentId();
}

}
