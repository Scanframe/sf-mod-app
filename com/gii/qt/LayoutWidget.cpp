#include "LayoutWidget.h"

namespace sf
{

struct LayoutWidget::Private
{

	LayoutWidget*_w;

	Gii::IdType _idOffset{};

	bool _readOnly{true};

	explicit Private(LayoutWidget* widget)
	:_w(widget)
	{
		_w->_p = this;
	}
};

LayoutWidget::LayoutWidget(QWidget* parent, Qt::WindowFlags f)
:QWidget(parent, f)
{
	new Private(this);
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
	_p->_readOnly = ro;
}

bool LayoutWidget::getReadOnly() const
{
	return _p->_readOnly;
}

QDir LayoutWidget::getDirectory() const
{
	return {};
}

QString LayoutWidget::getSuffix()
{
	return "ui";
}

Gii::IdType LayoutWidget::getIdOffset() const
{
	return _p->_idOffset;
}

void LayoutWidget::setIdOffset(Gii::IdType idOfs) const
{
	_p->_idOffset = idOfs;
}

}
