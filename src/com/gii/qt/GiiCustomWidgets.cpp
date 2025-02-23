#include "GiiCustomWidgets.h"
#include "InformationIdEditPlugin.h"
#include "VariableBarPlugin.h"
#include "VariableCheckBoxPlugin.h"
#include "VariableComboBoxPlugin.h"
#include "VariableEditPlugin.h"
#include "VariableTreeViewPlugin.h"
#include <misc/qt/ObjectExtension.h>

namespace sf
{

GiiCustomWidgets::GiiCustomWidgets(QObject* parent)
	: QObject(parent)
{
	if (ObjectExtension::inDesigner())
	{
		//Variable::initialize();
	}
	// Add the implemented widgets to the list.
	widgets.append(new VariableBarPlugin(this));
	widgets.append(new VariableEditPlugin(this));
	widgets.append(new VariableCheckBoxPlugin(this));
	widgets.append(new VariableComboBoxPlugin(this));
	widgets.append(new VariableTreeViewPlugin(this));
	widgets.append(new InformationIdEditPlugin(this));
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

}// namespace sf
