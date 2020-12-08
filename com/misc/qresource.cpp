#include "qresource.h"

namespace sf
{

const char* QResource::_iconNames[] =
	{
		":action/clear", ":action/reload", ":action/submit"
	};

const char* QResource::getIconName(QResource::Icon icon)
{
	return _iconNames[static_cast<int>(icon)];
}

QIcon QResource::getIcon(QResource::Icon icon)
{
	return QIcon(_iconNames[static_cast<int>(icon)]);
}

}