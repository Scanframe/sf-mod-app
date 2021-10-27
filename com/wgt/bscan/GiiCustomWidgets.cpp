#include <QDebug>
#include <misc/qt/ObjectExtension.h>
#include "GiiCustomWidgets.h"
#include "BscanGraphPlugin.h"

namespace sf
{

GiiCustomWidgets::GiiCustomWidgets(QObject* parent)
	:QObject(parent)
{
	if (ObjectExtension::inDesigner())
	{
		//Variable::initialize();
	}
	// Add the implemented widgets to the list.
	widgets.append(new BscanGraphPlugin(this));
}

GiiCustomWidgets::~GiiCustomWidgets()
{
	if (ObjectExtension::inDesigner())
	{
		// Disabled deinitialize because the in the designer the _references vector is changed.
		//Variable::deinitialize();
	}
}

QList<QDesignerCustomWidgetInterface*> GiiCustomWidgets::customWidgets() const
{
	return widgets;
}

}
