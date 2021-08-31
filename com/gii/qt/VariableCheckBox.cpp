#include "VariableCheckBox.h"

#include <QKeyEvent>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QMenu>
#include <QTimer>
#include <QGuiApplication>
#include <misc/gen/dbgutils.h>
#include <misc/gen/ScriptEngine.h>
#include <misc/qt/LayoutWidget.h>
#include "VariableWidgetBasePrivate.h"

namespace sf
{

struct VariableCheckBox::Private :QObject, VariableWidgetBase::PrivateBase
{
	VariableCheckBox* _widget{nullptr};
	QHBoxLayout* _layout{nullptr};
	QLabel* _labelName{nullptr};
	QLabel* _labelNameAlt{nullptr};
	QCheckBox* _editValue{nullptr};
	int _nameLevel{-1};

	static VariableCheckBox::Private* cast(PrivateBase* data)
	{
		return static_cast<VariableCheckBox::Private*>(data); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	}

	struct CheckBox :QCheckBox
	{
		explicit CheckBox(const QString& text, QWidget* parent = nullptr)
			:QCheckBox(text, parent) {}

		void keyPressEvent(QKeyEvent* event) override;
	};

	explicit Private(VariableCheckBox* widget)
		:_widget(widget)
	{
		_labelName = new QLabel("&Name");
		_editValue = new CheckBox("Value");
		//
		_labelName->setObjectName("name");
		_editValue->setObjectName("value");
		// Allow name with hot key for instant focus.
		_labelName->setBuddy(_editValue);
		// Format using Horizontal layout.
		_layout = new QHBoxLayout(_widget);
		// Clear margins for now.
		_layout->setContentsMargins(0, 0, 0, 0);
		// Add the widgets in the right order of appearance.
		_layout->addWidget(_labelName);
		_layout->addWidget(_editValue);
		_layout->setStretchFactor(_editValue, 1);
		// Allows setting tab order in designer.
		_widget->setFocusPolicy(Qt::StrongFocus);
		// Then when getting focus pass it to the child line edit widget.
		_widget->setFocusProxy(_editValue);
		//
		QObject::connect(_editValue, &QCheckBox::clicked, this, &VariableCheckBox::Private::onClicked);
		// Only link when not in design mode.
		if (!ObjectExtension::inDesigner())
		{
			_variable.setHandler(this);
		}
		QTimer::singleShot(0, this, &VariableCheckBox::Private::connectLabelNameAlt);
		// The edit value gets handler for the context menu.
		_editValue->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(_editValue, &QCheckBox::customContextMenuRequested, this, &VariableCheckBox::Private::createContextMenu);
	}

	~Private() override
	{
		// Clear the handler when destructing.
		_variable.setHandler(nullptr);
		delete _labelName;
		delete _editValue;
	}

	void createContextMenu(const QPoint& pos) const
	{
		if (QGuiApplication::keyboardModifiers() ==(Qt::ControlModifier | Qt::ShiftModifier))
		{
			if (auto lw = LayoutWidget::getLayoutWidgetOf(_widget))
			{
				lw->popupContextMenu(_widget, _editValue->mapToGlobal(pos));
				return;
			}
		}
	}

	void setReadOnly(bool yn) // NOLINT(readability-make-member-function-const)
	{
		_editValue->setAttribute(Qt::WA_TransparentForMouseEvents, yn);
	}
	void variableEventHandler
		(
			EEvent event,
			const Variable& call_var,
			Variable& link_var,
			bool same_inst
		) override
	{
		SF_COND_RTTI_NOTIFY(IsDebug(), DO_DEFAULT, Variable::getEventName(event));
		switch (event)
		{
			case veLinked:
			case veIdChanged:
			{
				_widget->applyReadOnly(_readOnly || link_var.isReadOnly());
				_widget->setToolTip(QString::fromStdString(link_var.getDescription()));
				auto state = _variable.getState(link_var.getCur());
				_editValue->setChecked(!!state);
				_editValue->setText(QString::fromStdString(_variable.getStateName(state)));
			}
			// Run into next.

			case veUserPrivate:
				_labelName->setText(QString::fromStdString(link_var.getName(_nameLevel)));
				if (_labelNameAlt)
				{
					_labelNameAlt->setText(_labelName->text());
					_labelNameAlt->setToolTip(_widget->toolTip());
				}
				break;

			case veValueChange:
			{
				auto state = _variable.getState(link_var.getCur());
				_editValue->setChecked(!!state);
				_editValue->setText(QString::fromStdString(_variable.getStateName(state)));
				break;
			}

			case veFlagsChange:
				_widget->applyReadOnly(_readOnly || link_var.isReadOnly());
				break;

			default:
				break;

		}
	}

	void updateValue(bool skip)
	{
		// Can have only to states.
		if (_variable.getStateCount() == 2)
		{
			Variable::size_type state = _editValue->isChecked();
			_variable.setCur(_variable.getStates().at(state)._value);
		}
		// Update the control with the recently set value.
		if (!skip)
		{
			variableEventHandler(Variable::veValueChange, _variable, _variable, true);
		}
	}

	void onClicked(bool/* checked*/)
	{
		updateValue(false);
	}

	void onDestroyed(QObject* obj = nullptr) // NOLINT(readability-make-member-function-const)
	{
		if (_labelNameAlt && _labelNameAlt == obj)
		{
			_labelName->setVisible(true);
			disconnect(_labelNameAlt, &QLabel::destroyed, this, &VariableCheckBox::Private::onDestroyed);
			_labelNameAlt = nullptr;
		}
	}

	void connectLabelNameAlt()
	{
		// Try finding label where this widget is its buddy.
		if (auto label = findLabelByBuddy(_widget))
		{
			// Hide original label.
			_labelName->setVisible(false);
			// Assign the pointer to alternate name label.
			_labelNameAlt = label;
			// Connect handler for when the label is destroyed to null the alternate label.
			connect(_labelNameAlt, &QLabel::destroyed, this, &VariableCheckBox::Private::onDestroyed);
			// Trigger event to fill in the label text and tool tip.
			_variable.emitEvent(Variable::veUserPrivate);
		}
	}

};

void VariableCheckBox::Private::CheckBox::keyPressEvent(QKeyEvent* event)
{
	QCheckBox::keyPressEvent(event);
	cast(dynamic_cast<VariableCheckBox*>(parent())->_p)->keyPressEvent(event);
}

VariableCheckBox::VariableCheckBox(QWidget* parent)
	:VariableWidgetBase(parent, this)
{
	_p = new VariableCheckBox::Private(this);
}

void VariableCheckBox::applyReadOnly(bool yn)
{
	setFocusPolicy(yn ? Qt::NoFocus : Qt::StrongFocus);
	if (_p)
	{
		VariableCheckBox::Private::cast(_p)->setReadOnly(yn);
	}
}

bool VariableCheckBox::isRequiredProperty(const QString& name)
{
/*
	if (VariableWidgetBase::isRequiredProperty(name))
	{
		return true;
	}
	static const char* keys[] =
		{
			"geometry",
			"whatsThis",
			"styleSheet",
		};
	// Check if passed property name is in the keys list.
	return std::any_of(&keys[0], &keys[sizeof(keys) / sizeof(keys[0])], [name](const char* prop)
	{
		return name == prop;
	});
*/
	return true;
}

int VariableCheckBox::nameLevel() const
{
	return VariableCheckBox::Private::cast(_p)->_nameLevel;
}

void VariableCheckBox::setNameLevel(int level)
{
	auto p = VariableCheckBox::Private::cast(_p);
	if (p->_nameLevel != level)
	{
		p->_nameLevel = level;
		p->_variable.emitEvent(Variable::veUserPrivate);
	}
}

}
