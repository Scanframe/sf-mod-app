#include <QKeyEvent>
#include <misc/qt/qt_utils.h>
#include <misc/qt/PropertySheetDialog.h>
#include "../gen/Variable.h"
#include "../gen/VariableHandler.h"
#include "VariableWidgetBase.h"
#include "VariableWidgetBasePrivate.h"
#include "VariableIdPropertyPage.h"
#include "LayoutWidget.h"

namespace sf
{

QLabel* VariableWidgetBase::PrivateBase::findLabelByBuddy(QWidget* widget)
{
	if (auto layout = getWidgetLayout(widget))
	{
		for (int i = 0; i < layout->count(); i++)
		{
			if (auto label = dynamic_cast<QLabel*>(layout->itemAt(i)->widget()))
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

void VariableWidgetBase::setId(qulonglong id)
{
	// Assign the value without the offset.
	_p->_id = id;
	// When not in the designer set the variable id.
	if (!inDesigner())
	{
		if (auto lw = LayoutWidget::getLayoutWidgetOf(this))
		{
			id += lw->getIdOffset();
		}
		_p->_variable.setup(id, true);
	}
}

qulonglong VariableWidgetBase::getId() const
{
	return _p->_id;
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

void VariableWidgetBase::addPropertyPages(sf::PropertySheetDialog* sheet)
{
	// Even when empty now it could be filled later.
	ObjectExtension::addPropertyPages(sheet);
	sheet->addPage(new VariableIdPropertyPage(this, sheet));
}

}
