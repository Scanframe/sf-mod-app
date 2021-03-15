#include <QDebug>
#include "FormWriter.h"
#include "ObjectExtension.h"
#include "gen/dbgutils.h"

namespace sf
{

bool isWidgetRequiredProperty(const QString& name)
{
	static const char* keys[] = {"geometry", "styleSheet"};
	// Check if passed property name is in the keys list.
	return std::any_of(&keys[0], &keys[sizeof(keys) / sizeof(keys[0])], [name](const char* prop)
	{
		return name == prop;
	});
}

QList<DomProperty*> FormWriter::computeProperties(QObject* obj)
{
	auto list = QFormBuilder::computeProperties(obj);
	// Associate the properties with their names.
	auto map = propertyMap(list);
	// When a QWidget type is passed it is presumed to be the root of the form.
	if (! strcmp(obj->metaObject()->className(), "QWidget"))
	{
		for (auto& key: map.keys())
		{
			// For now only the geometry is the only property needed.
			if (!isWidgetRequiredProperty(key))
			{
				map.erase(map.find(key));
			}
		}
	}
	// When it exists modify the property list.
	else if (auto oe = dynamic_cast<sf::ObjectExtension*>(obj))
	{
		for (auto& key: map.keys())
		{
			// When the object property is not required remove it from the list/map.
			if (!oe->isRequiredProperty(key))
			{
				map.erase(map.find(key));
			}
			/*
			auto prop = obj->property(key.toStdString().c_str());
			if (prop.isNull() || !prop.isValid())
			{
				map.erase(map.find(key));
			}
			//qDebug() << _Q_RTTI_NAME(*obj) << toVariant(obj->metaObject(), map[key]);
			*/
		}
	}
	return map.values();
}

}