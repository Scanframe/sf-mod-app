#include <QKeyEvent>
#include <misc/qt/qt_utils.h>
#include <gen/Variable.h>
#include <gen/VariableHandler.h>
#include "VariableWidgetBase.h"
#include "VariableWidgetBasePrivate.h"

namespace sf
{

QLabel* VariableWidgetBase::PrivateBase::findLabelByBuddy(QWidget* widget)
{
	if (auto layout = getWidgetLayout(widget))
	{
		for (int i = 0; i < layout->count(); i++)
		{
			if (auto label = qobject_cast<QLabel*>(layout->itemAt(i)->widget()))
			{
				// When a label id found having this widget as its buddy.
				if (label->buddy() == widget)
				{
					return label;
				}
			}
		}
	}
	return nullptr;
}


void VariableWidgetBase::PrivateBase::keyPressEvent(QKeyEvent* event)
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
	delete_null(_p);
}

void VariableWidgetBase::setVariableId(const QString& id)
{
	if (inDesigner())
	{
		_p->_id = id;
	}
	_p->_variable.setup(id.toULongLong(nullptr, 0), true);
}

QString VariableWidgetBase::getVariableId() const
{
	if (inDesigner())
	{
		return _p->_id;
	}
	return QString("0x%1").arg(_p->_variable.getDesiredId(), 0, 16);
}

void VariableWidgetBase::setConverted(bool yn)
{
	_p->_converted = yn;
	if (!inDesigner())
	{
		_p->_variable.setConvert(yn);
	}
}

bool VariableWidgetBase::getConverted() const
{
	return _p->_converted;
}

void VariableWidgetBase::setReadOnly(bool yn)
{
	_p->_readOnly = yn;
	applyReadOnly(_p->_readOnly || _p->_variable.isReadOnly());
}

bool VariableWidgetBase::getReadOnly() const
{
	return _p->_readOnly;
}

bool VariableWidgetBase::isRequiredProperty(const QString& name)
{
/*
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
*/
	return true;
}

Variable& VariableWidgetBase::getVariable()
{
	return _p->_variable;
}

const char* VariableWidgetBase::propertyNameEditorObject()
{
	return "editorObject";
}

}
