#include "VariableComboBox.h"
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QTimer>
#include <QGuiApplication>
#include <misc/gen/dbgutils.h>
#include <misc/gen/ScriptEngine.h>
#include <misc/qt/qt_utils.h>
#include "VariableWidgetBasePrivate.h"

namespace sf
{

struct VariableComboBox::Private :QObject, VariableWidgetBase::PrivateBase
{
	VariableComboBox* _widget{nullptr};
	QHBoxLayout* _layout{nullptr};
	QLabel* _labelName{nullptr};
	QLabel* _labelNameAlt{nullptr};
	QComboBox* _comboBoxValue{nullptr};
	int _nameLevel{-1};

	static VariableComboBox::Private* cast(PrivateBase* data)
	{
		return static_cast<VariableComboBox::Private*>(data); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	}

	explicit Private(VariableComboBox* widget)
		:_widget(widget)
	{
		_labelName = new QLabel("Name");
		_comboBoxValue = new QComboBox();
		_labelName->setObjectName("name");
		_comboBoxValue->setObjectName("value");
		// Set the correct parent to edit in the layout editor.
		_comboBoxValue->setProperty(VariableWidgetBase::propertyNameEditorObject(), QVariant::fromValue(_widget));
		_labelName->setProperty(VariableWidgetBase::propertyNameEditorObject(), QVariant::fromValue(_widget));
		// Allow name with hot key for instant focus.
		_labelName->setBuddy(_comboBoxValue);
		// Format using Horizontal layout.
		_layout = new QHBoxLayout(_widget);
		// Clear margins for now.
		_layout->setContentsMargins(0, 0, 0, 0);
		// Add the widgets in the right order of appearance.
		_layout->addWidget(_labelName);
		_layout->addWidget(_comboBoxValue);
		// Allows setting tab order in designer.
		_widget->setFocusPolicy(Qt::StrongFocus);
		// Then when getting focus pass it to the child line edit widget.
		_widget->setFocusProxy(_comboBoxValue);
		//
		QObject::connect(_comboBoxValue, &QComboBox::currentIndexChanged, this, &VariableComboBox::Private::onIndexChange);
		// Only link when not in design mode.
		if (!ObjectExtension::inDesigner())
		{
			_variable.setHandler(this);
		}
		// Execute after the layout is loaded, so the label can be found for sure.
		QTimer::singleShot(0, this, &VariableComboBox::Private::connectLabelNameAlt);
		// The edit value gets handler for the context menu.
		_comboBoxValue->setContextMenuPolicy(Qt::CustomContextMenu);
	}

	~Private() override
	{
		// Clear the handler when destructing.
		_variable.setHandler(nullptr);
		delete _labelName;
		delete _comboBoxValue;
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
				_widget->applyReadOnly(_readOnly || link_var.isReadOnly());
				// Remove all items.
				_comboBoxValue->clear();
				for (int i = 0; i < link_var.getStateCount(); i++)
				{
					_comboBoxValue->addItem(QString::fromStdString(link_var.getStateName(i)), i);
				}
				_widget->setToolTip(QString::fromStdString(link_var.getDescription()));
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

				_comboBoxValue->setCurrentIndex(indexFromComboBox(_comboBoxValue, static_cast<int>(link_var.getState(link_var.getCur()))));
				break;

			case veFlagsChange:
				_widget->applyReadOnly(_readOnly || link_var.isReadOnly());
				break;

			default:
				break;
		}
	}

	void updateValue(bool skip)
	{
		if (!_readOnly && _variable.getStateCount())
		{
			_variable.setCur(_variable.getStateValue(_comboBoxValue->currentData().toInt()), skip);
			// Update the control with the recently set value.
			if (!skip)
			{
				variableEventHandler(Variable::veValueChange, _variable, _variable, true);
			}
		}
	}

	void onIndexChange(int index)
	{
		(void) index;
		updateValue(false);
	}

	void onDestroyed(QObject* obj = nullptr) // NOLINT(readability-make-member-function-const)
	{
		if (_labelNameAlt && _labelNameAlt == obj)
		{
			_labelName->setVisible(true);
			disconnect(_labelNameAlt, &QLabel::destroyed, this, &VariableComboBox::Private::onDestroyed);
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
			connect(_labelNameAlt, &QLabel::destroyed, this, &VariableComboBox::Private::onDestroyed);
			// Trigger event to fill in the label text and tool tip.
			_variable.emitEvent(Variable::veUserPrivate);
		}
	}
};

VariableComboBox::VariableComboBox(QWidget* parent)
	:VariableWidgetBase(parent, this)
{
	_p = new VariableComboBox::Private(this);
}

void VariableComboBox::applyReadOnly(bool yn)
{
	setFocusPolicy(yn ? Qt::NoFocus : Qt::StrongFocus);
	if (_p)
	VariableComboBox::Private::cast(_p)->_comboBoxValue->setDisabled(yn);
}

bool VariableComboBox::isRequiredProperty(const QString& name)
{
	return true;
}

void VariableComboBox::setFocusFrame(bool yn)
{
	VariableComboBox::Private::cast(_p)->_comboBoxValue->setFrame(yn);
}

bool VariableComboBox::hasFocusFrame() const
{
	return VariableComboBox::Private::cast(_p)->_comboBoxValue->hasFrame();
}

int VariableComboBox::nameLevel() const
{
	return VariableComboBox::Private::cast(_p)->_nameLevel;
}

void VariableComboBox::setNameLevel(int level)
{
	auto p = VariableComboBox::Private::cast(_p);
	if (p->_nameLevel != level)
	{
		p->_nameLevel = level;
		p->_variable.emitEvent(Variable::veUserPrivate);
	}
}

}
