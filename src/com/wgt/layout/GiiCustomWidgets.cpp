#include "GiiCustomWidgets.h"
#include "LayoutButtonPlugin.h"
#include "LayoutTabsPlugin.h"
#include <misc/qt/ObjectExtension.h>
#include <QDebug>

namespace sf
{

GiiCustomWidgets::GiiCustomWidgets(QObject* parent)
	:QObject(parent)
{
	// Add the implemented widgets to the list.
	widgets.append(new LayoutButtonPlugin(this));
	widgets.append(new LayoutTabsPlugin(this));
}

GiiCustomWidgets::~GiiCustomWidgets()
{
	(void)this;
}

QList<QDesignerCustomWidgetInterface*> GiiCustomWidgets::customWidgets() const
{
	return widgets;
}

}
