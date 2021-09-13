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

}