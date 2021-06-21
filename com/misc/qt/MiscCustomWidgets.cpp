#include <QDebug>
#include "ObjectExtension.h"
#include "MiscCustomWidgets.h"
#include "DrawWidgetPlugin.h"
#include "ActionButtonPlugin.h"

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
	widgets.append(new ActionButtonPlugin(this));
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
