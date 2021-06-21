#include <QJSValueIterator>
#include "ExposedObject.h"

ExposedObject::ExposedObject(QObject* parent)
	:QObject(parent)
{
	std::clog << "Creating: " << __FUNCTION__ << std::endl;
}

int ExposedObject::stdClog(const QString& str)
{
	qDebug() <<  str;
	return str.length();
}

int ExposedObject::passValue(const QJSValue& value)
{
	if (value.isString())
	{
		qDebug() << value.toString();
	}
	QJSValueIterator it(value);
	while (it.hasNext())
	{
		it.next();
		qDebug() << it.name() << ": " << it.value().toString();
	}
	return 0;
}

