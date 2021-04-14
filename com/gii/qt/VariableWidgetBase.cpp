#include <QKeyEvent>
#include <gen/Variable.h>
#include <gen/VariableHandler.h>
#include "VariableWidgetBase.h"
#include "VariableWidgetBasePrivate.h"

namespace sf
{

void VariableWidgetBasePrivate::keyPressEvent(QKeyEvent* event)
{
	if (!_readOnly)
	{
		int steps = 1;
		if (event->modifiers() & Qt::ShiftModifier)
		{
			steps *= 10;
		}
		if (event->modifiers() & Qt::ControlModifier)
		{
			steps *= 100;
		}
		switch (event->key())
		{
			case Qt::Key_Up:
				_variable.increase(steps);
				break;

			case Qt::Key_Down:
				_variable.increase(-steps);
				break;
		}
	}
}

VariableWidgetBase::VariableWidgetBase(QWidget* parent, QObject* self)
	:base_type(parent), ObjectExtension(self)
{
}

VariableWidgetBase::~VariableWidgetBase()
{
	delete_null(_private);
}

void VariableWidgetBase::setVariableId(const QString& id)
{
	if (inDesigner())
	{
		_private->_id = id;
	}
	_private->_variable.setup(id.toULongLong(nullptr, 0), true);
}

QString VariableWidgetBase::getVariableId() const
{
	if (inDesigner())
	{
		return _private->_id;
	}
	return QString("0x%1").arg(_private->_variable.getDesiredId(), 0, 0);
}

void VariableWidgetBase::setConverted(bool yn)
{
	if (inDesigner())
	{
		_private->_converted = yn;
	}
	_private->_variable.setConvert(yn);
}

bool VariableWidgetBase::getConverted() const
{
	return _private->_converted;
}

void VariableWidgetBase::setReadOnly(bool yn)
{
	_private->_readOnly = yn;
	applyReadOnly(_private->_readOnly || _private->_variable.isReadOnly());
}

bool VariableWidgetBase::getReadOnly() const
{
	return _private->_readOnly;
}

bool VariableWidgetBase::isRequiredProperty(const QString& name)
{
	static const char* keys[] =
		{
			"converted",
			"readOnly",
			"variableId",
		};
	// Check if passed property name is in the keys list.
	return std::any_of(&keys[0], &keys[sizeof(keys) / sizeof(keys[0])], [name](const char* prop)
	{
		return name == prop;
	});
}

}
