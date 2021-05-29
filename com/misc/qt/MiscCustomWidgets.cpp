#include <QDebug>
#include "ObjectExtension.h"
#include "MiscCustomWidgets.h"
#include "DrawWidgetPlugin.h"

namespace sf
{

MiscCustomWidgets::MiscCustomWidgets(QObject* parent)
	:QObject(parent)
{
	if (ObjectExtension::inDesigner())
	{
	}
	// Add the implemented widgets to the list.
	widgets.append(new DrawWidgetPlugin(this));
}

MiscCustomWidgets::~MiscCustomWidgets()
{
	if (ObjectExtension::inDesigner())
	{
	}
}

QList<QDesignerCustomWidgetInterface*> MiscCustomWidgets::customWidgets() const
{
	return widgets;
}

}
