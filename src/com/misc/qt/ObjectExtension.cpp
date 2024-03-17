#include <QCoreApplication>
#include "ObjectExtension.h"

namespace sf
{

ObjectExtension::ObjectExtension(QObject* object)
	:_object(object)
{
}

bool ObjectExtension::inDesigner()
{
	static int flag{-1};
	if (flag < 0)
	{
		flag = QFileInfo(QCoreApplication::applicationFilePath()).fileName().contains("designer");
	}
	return flag;
}

bool ObjectExtension::getSaveChildren() const
{
	return false;
}

void ObjectExtension::addPropertyPages(PropertySheetDialog* sheet)
{
	// Deliberate left empty.
}

QObject* ObjectExtension::getExtensionParent(QObject* obj, bool first)
{
	QObject* rv = nullptr;
	while (obj)
	{
		if (dynamic_cast<ObjectExtension*>(obj))
		{
			if (first)
			{
				return obj;
			}
			rv = obj;
		}
		obj = obj->parent();
	}
	return first ? nullptr : rv;
}

}