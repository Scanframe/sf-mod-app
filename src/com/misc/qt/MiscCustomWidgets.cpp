#include "MiscCustomWidgets.h"
#include "ActionButtonPlugin.h"
#include "DrawWidgetPlugin.h"
#include "EditorPlugin.h"
#include "ObjectExtension.h"
#include "PropertyPagePlugin.h"
#include <QDebug>

namespace sf
{

MiscCustomWidgets::MiscCustomWidgets(QObject* parent)
	: QObject(parent)
{
	if (ObjectExtension::inDesigner())
	{
	}
	// Add the implemented widgets to the list.
	widgets.append(new DrawWidgetPlugin(this));
	widgets.append(new ActionButtonPlugin(this));
	widgets.append(new EditorPlugin(this));
	widgets.append(new PropertyPagePlugin(this));
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

}// namespace sf
