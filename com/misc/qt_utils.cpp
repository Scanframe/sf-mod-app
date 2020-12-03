#include "qt_utils.h"

namespace sf
{

QMetaObject::Connection QObject_connect
	(
		const QObject* sender,
		const char* signal_name,
		const QObject* receiver,
		const char* method_name,
		Qt::ConnectionType
	)
{
	std::string signal(SIGNAL(__s__()));
	std::string slot(SLOT(__m__()));
	//
	std::string::size_type pos = signal.find("__s__");
	//
	if (pos != std::string::npos)
	{
		signal.replace(pos, 5, signal_name);
	}
	pos = slot.find("__m__");
	if (pos != std::string::npos)
	{
		slot.replace(pos, 5, method_name);
	}
	//
	return QObject::connect(sender, signal.c_str(), receiver, slot.c_str());
}

}