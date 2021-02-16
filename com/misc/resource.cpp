#include <QFile>
#include "resource.h"

namespace sf
{

const char* Resource::_iconNames[] =
	{
		":action/clear", ":action/reload", ":action/submit"
	};

const char* Resource::getIconName(Resource::Icon icon)
{
	return _iconNames[static_cast<int>(icon)];
}

QIcon Resource::getIcon(Resource::Icon icon)
{
	return QIcon(_iconNames[static_cast<int>(icon)]);
}

QString Resource::getString(const QString& resource_location)
{
	QFile file(resource_location);
	if (file.open(QIODevice::ReadOnly))
	{
		return file.readAll();
	}
	return nullptr;
}

}
