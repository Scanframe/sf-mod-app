#include "GmiCustomWidgets.h"
#include "AxesCoordEditPlugin.h"
#include <QDebug>
#include <misc/qt/ObjectExtension.h>

namespace sf
{

GmiCustomWidgets::GmiCustomWidgets(QObject* parent)
	: QObject(parent)
{
	if (ObjectExtension::inDesigner())
	{
		//Variable::initialize();
	}
	// Add the implemented widgets to the list.
	widgets.append(new AxesCoordEditPlugin(this));
}

GmiCustomWidgets::~GmiCustomWidgets()
{
	if (ObjectExtension::inDesigner())
	{
		// Disabled deinitialize because the in the designer the _references vector is changed.
		//Variable::deinitialize();
	}
}

QList<QDesignerCustomWidgetInterface*> GmiCustomWidgets::customWidgets() const
{
	return widgets;
}

}// namespace sf
