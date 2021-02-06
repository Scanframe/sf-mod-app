#include "SfCustomWidgets.h"
#include "SfPushButtonPlugin.h"
#include "SfAnalogClockPlugin.h"


SfCustomWidgets::SfCustomWidgets(QObject *parent)
	: QObject(parent)
{
	// Add the implemented widgets to the list.
	widgets.append(new SfPushButtonPlugin(this));
	widgets.append(new AnalogClockPlugin(this));
}

QList<QDesignerCustomWidgetInterface*> SfCustomWidgets::customWidgets() const
{
	return widgets;
}
