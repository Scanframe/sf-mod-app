#include "Persistent.h"
#include <QObject>
#include <QMap>
#include <QMetaProperty>
#include <QMetaType>

namespace sf
{

Persistent::Persistent(QObject* obj)
	:_object(obj)
{
}


QMap<int, const char*> getObjectProperties(QObject* obj)
{
	QMap<int, const char*> rv;
	auto mo = obj->metaObject();
	do
	{
		for (int i = mo->propertyOffset(); i < mo->propertyCount(); ++i)
		{
			const auto& prop = mo->property(i);
			if (prop.isWritable() && prop.isDesignable() && !prop.isAlias())
			{
				// Ignore QObject which has only the unused name property.
				if (mo->metaType() != QMetaType::fromType<QObject>())
				{
					rv[i] = prop.name();
				};
			}
		}
	}
	while ((mo = mo->superClass()));
	return rv;
}


QStringList Persistent::getProperties() const
{
	QStringList rv;
	for (auto& name: getObjectProperties(_object))
	{
		rv.append(QString("%1=%2").arg(name).arg(_object->property(name).toString()));
	}
	return rv;
}

void Persistent::setProperties(const QStringList& values) const
{
	for (auto& vn: values)
	{
		if (auto idx = vn.indexOf('='))
		{
			if (idx >= 0)
			{
				auto pn = vn.left(idx).toLatin1();
				// Only set it when the property exists.
				if (_object->property(pn).isValid())
				{
					_object->setProperty(pn, vn.mid(idx + 1));
				}
			}
		}

	}
}

}

