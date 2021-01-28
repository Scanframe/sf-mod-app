#include <QDebug>
#include <QLabel>
#include <QMetaProperty>

#include "qformwriter.h"
#include "dbgutils.h"

namespace sf
{

QList<DomProperty*> QFormWriter::computeProperties(QObject* obj)
{
	auto list = QFormBuilder::computeProperties(obj);

	auto mo = obj->metaObject();
	auto label = dynamic_cast<QLabel*>(obj);
	auto var_pixmap = obj->property("pixmap");
	//
	if (label && var_pixmap.isValid())
	{
		qDebug() << _Q_RTTI_TYPENAME << mo->property(mo->indexOfProperty("pixmap")).typeName();
		auto prop = mo->property(mo->indexOfProperty("pixmap"));
		if (!prop.isStored())
		{
			qDebug() << _Q_RTTI_TYPENAME << "\"pixmap\" NOT isStored";
		}
		else
		{
			qDebug() << _Q_RTTI_TYPENAME << "\"pixmap\" isStored";
		}
		qDebug() << "Pixmap Variant:" << var_pixmap;
		auto pixmap = var_pixmap.value<QPixmap>();
		qDebug() << "Pixmap:" << pixmap;

		//qDebug() << "map::keys" << map.keys();
/*
		//variantToDomProperty();
		if (checkProperty(obj, "pixmap"))
		{
			auto prop = createProperty(obj, "pixmap", obj->property("pixmap"));
			if (prop)
			{
				list.append(prop);
			}
		}
*/
		//list.append(createProperty(obj, "mytest", "Just a value."));
	}
	return list;

/*
	auto map = propertyMap(list);

	for (auto& key: map.keys())
	{
		auto ks = key.toStdString();
		auto prop = obj->property(ks.c_str());

		if (prop.isNull() || !prop.isValid())
		{
			//qDebug() << _Q_RTTI_TYPENAME << key << " == isNull";
			map.erase(map.find(key));
		}

		if (!mo->property(mo->indexOfProperty(ks.c_str())).isStored())
		{
			//qDebug() << _Q_RTTI_TYPENAME << key << " not isStored";
			map.erase(map.find(key));
		}
	}
	auto var = toVariant(obj->metaObject(), dp);
	qDebug() << _Q_RTTI_NAME(*obj) << var;
	return map.values();
*/
}

}