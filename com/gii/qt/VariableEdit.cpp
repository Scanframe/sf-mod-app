#include "VariableEdit.h"

#include <QKeyEvent>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <misc/gen/dbgutils.h>
#include <misc/gen/ScriptEngine.h>
#include "VariableWidgetBasePrivate.h"

namespace sf
{

struct VariableEditPrivate :QObject, VariableWidgetBasePrivate
{
	VariableEdit* _widget{nullptr};
	QHBoxLayout* _layout{nullptr};
	QLabel* _labelName{nullptr};
	QLineEdit* _editValue{nullptr};
	QLabel* _labelUnit{nullptr};
	int _nameLevel{-1};

	static VariableEditPrivate* cast(VariableWidgetBasePrivate* data)
	{
		return static_cast<VariableEditPrivate*>(data); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	}

	struct LineEdit :QLineEdit
	{
		explicit LineEdit(const QString& text, QWidget* parent = nullptr)
			:QLineEdit(text, parent) {}

		void keyPressEvent(QKeyEvent* event) override;
	};

	explicit VariableEditPrivate(VariableEdit* widget)
		:_widget(widget)
	{
		_labelName = new QLabel("&Name", _widget);
		_editValue = new LineEdit("Value", _widget);
		_labelUnit = new QLabel("Unit", _widget);
		//
		_labelName->setObjectName("name");
		_editValue->setObjectName("value");
		_labelUnit->setObjectName("unit");
		// Allow name with hot key for instant focus.
		_labelName->setBuddy(_editValue);
		// Format using Horizontal layout.
		_layout = new QHBoxLayout(_widget);
		// Clear margins for now.
		_layout->setContentsMargins(0, 0, 0, 0);
		// Add the widgets in the right order of appearance.
		_layout->addWidget(_labelName);
		_layout->addWidget(_editValue);
		_layout->addWidget(_labelUnit);
		// Allows setting tab order in designer.
		_widget->setFocusPolicy(Qt::StrongFocus);
		// Then when getting focus pass it to the child line edit widget.
		_widget->setFocusProxy(_editValue);
		//
		QObject::connect(_editValue, &QLineEdit::editingFinished, this, &VariableEditPrivate::onEditingFinished);
		// Only link when not in design mode.
		if (!ObjectExtension::inDesigner())
		{
			_variable.setHandler(this);
		}
	}

	~VariableEditPrivate() override
	{
		// Clear the handler when destructing.
		_variable.setHandler(nullptr);
	}

	void VariableEventHandler
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
				_editValue->setReadOnly(link_var.isReadOnly() || _readOnly);
				_labelName->setText(QString::fromStdString(link_var.getName(_nameLevel)));
				_labelUnit->setVisible(!!link_var.getUnit().length());
				_labelUnit->setText(QString::fromStdString(link_var.getUnit()));
				_editValue->setAlignment(Qt::AlignVCenter | (link_var.isNumber() ? Qt::AlignRight : Qt::AlignLeft));
				_editValue->setText(QString::fromStdString(link_var.getCurString()));
				_widget->setToolTip(QString::fromStdString(link_var.getDescription()));
				break;

			case veValueChange:
				_editValue->setText(QString::fromStdString(link_var.getCurString()));
				break;

			case veFlagsChange:
				_editValue->setReadOnly(link_var.isReadOnly() || _readOnly);
				break;

			case veConverted:
			case veUserPrivate:
				_labelName->setText(QString::fromStdString(link_var.getName(_nameLevel)));
				_labelUnit->setVisible(!!link_var.getUnit().length());
				_labelUnit->setText(QString::fromStdString(link_var.getUnit()));
				_editValue->setText(QString::fromStdString(link_var.getCurString()));
				break;

			default:
				break;

		}
	}

	void updateValue(bool skip)
	{
		if (_variable.getStringType() == Variable::stMulti)
		{
			_variable.setCur(Value(_editValue->text().toStdString()), skip);
		}
		else
		{
			if (_variable.getType() == Value::vitFloat)
			{
				_variable.setCur(Value(calculator(_editValue->text().toStdString(), _variable.getCur().getFloat())), skip);
			}
			else
			{
				// Just a normal assignment here.
				_variable.setCur(Value(_editValue->text().toStdString()), skip);
			}
		}
		// Update the control with the recently set value.
		if (!skip)
		{
			VariableEventHandler(Variable::veValueChange, _variable, _variable, true);
		}
	}

	void onEditingFinished()
	{
		if (!_editValue->isReadOnly() && _editValue->isModified())
		{
			updateValue(false);
		}
	}

};

void VariableEditPrivate::LineEdit::keyPressEvent(QKeyEvent* event)
{
	QLineEdit::keyPressEvent(event);
	cast(dynamic_cast<VariableEdit*>(parent())->_private)->keyPressEvent(event);
}

void VariableEdit::applyReadOnly(bool yn)
{
	setFocusPolicy(yn ? Qt::NoFocus : Qt::StrongFocus);
	if (!inDesigner())
	{
		VariableEditPrivate::cast(_private)->_editValue->setReadOnly(yn);
	}
}

VariableEdit::VariableEdit(QWidget* parent)
	:VariableWidgetBase(parent, this)
{
	_private = new VariableEditPrivate(this);
}

bool VariableEdit::isRequiredProperty(const QString& name)
{
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
}

void VariableEdit::setFocusFrame(bool yn)
{
	VariableEditPrivate::cast(_private)->_editValue->setFrame(yn);
}

bool VariableEdit::hasFocusFrame() const
{
	return VariableEditPrivate::cast(_private)->_editValue->hasFrame();
}

int VariableEdit::nameLevel() const
{
	return VariableEditPrivate::cast(_private)->_nameLevel;
}

void VariableEdit::setNameLevel(int level)
{
	auto p = VariableEditPrivate::cast(_private);
	if (	p->_nameLevel != level)
	{
		p->_nameLevel = level;
		p->_variable.emitEvent(Variable::veUserPrivate);
	}
}

}
