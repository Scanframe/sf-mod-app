#include <QDebug>
#include <misc/qt/ObjectExtension.h>
#include "../gen/Variable.h"
#include "GiiCustomWidgets.h"
#include "VariableEditPlugin.h"
#include "VariableBarPlugin.h"
#include "AscanGraphPlugin.h"

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
	widgets.append(new VariableEditPlugin(this));
	widgets.append(new VariableBarPlugin(this));
	widgets.append(new AscanGraphPlugin(this));
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
