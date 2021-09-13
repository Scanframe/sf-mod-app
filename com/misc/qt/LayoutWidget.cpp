#include "LayoutWidget.h"

namespace sf
{

LayoutWidget::LayoutWidget(QWidget* parent, Qt::WindowFlags f)
:QWidget(parent, f)
{
}

LayoutWidget* LayoutWidget::getLayoutWidgetOf(QObject* obj)
{
	QObject* o = obj;
	while(o)
	{
		if (auto lw = dynamic_cast<LayoutWidget*>(o))
		{
			return lw;
		}
		o = o->parent();
	}
	return nullptr;
}

void LayoutWidget::openPropertyEditor(QObject* target)
{
	// Deliberately empty.
}

void LayoutWidget::popupContextMenu(QObject* target, const QPoint& pos)
{
	// Deliberately empty.
}

void LayoutWidget::setReadOnly(bool ro)
{
	_readOnly = ro;
}

bool LayoutWidget::getReadOnly() const
{
	return _readOnly;
}

}
