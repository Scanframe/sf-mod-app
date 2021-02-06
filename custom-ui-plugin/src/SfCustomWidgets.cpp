#include "SfCustomWidgets.h"
#include "SfPushButtonPlugin.h"
#include "SfAnalogClockPlugin.h"

namespace sf
{

CustomWidgets::CustomWidgets(QObject* parent)
	:QObject(parent)
{
	// Add the implemented widgets to the list.
	widgets.append(new sf::AnalogClockPlugin(this));
	widgets.append(new sf::PushButtonPlugin(this));
}

QList<QDesignerCustomWidgetInterface*> CustomWidgets::customWidgets() const
{
	return widgets;
}

} // namespace
