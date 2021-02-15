#include <QJSValueIterator>
#include "ExposedObject.h"

ExposedObject::ExposedObject(QObject* parent)
	:QObject(parent)
{
	std::clog << __FUNCTION__ << std::endl;
}

int ExposedObject::stdClog(const QString& str)
{
	std::clog << str << std::endl;
	return str.length();
}

int ExposedObject::passValue(const QJSValue& value)
{
	if (value.isString())
	{
		std::clog << value.toString() << std::endl;
	}
	QJSValueIterator it(value);
	while (it.hasNext())
	{
		it.next();
		std::clog << it.name() << ": " << it.value().toString() << std::endl;
	}
	return 0;
}

