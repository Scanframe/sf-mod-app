#include "VariableEdit.h"
#include "LayoutData.h"
#include "VariableWidgetBasePrivate.h"
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QTimer>
#include <misc/gen/ScriptEngine.h>
#include <misc/gen/dbgutils.h>

namespace sf
{

struct VariableEdit::Private : QObject
	, VariableWidgetBase::PrivateBase
{
		VariableEdit* _widget{nullptr};
		QHBoxLayout* _layout{nullptr};
		QLabel* _labelName{nullptr};
		QLabel* _labelNameAlt{nullptr};
		QLineEdit* _editValue{nullptr};
		QLabel* _labelUnit{nullptr};
		int _nameLevel{-1};

		static VariableEdit::Private* cast(PrivateBase* data)
		{
			return static_cast<VariableEdit::Private*>(data);// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
		}

		struct LineEdit : QLineEdit
		{
				explicit LineEdit(const QString& text, QWidget* parent = nullptr)
					: QLineEdit(text, parent)
				{}

				void keyPressEvent(QKeyEvent* event) override;
		};

		explicit Private(VariableEdit* widget)
			: _widget(widget)
		{
			_labelName = new QLabel("&Name");
			_editValue = new LineEdit("Value");
			_labelUnit = new QLabel("unit");
			//
			_labelName->setObjectName("name");
			_editValue->setObjectName("value");
			_labelUnit->setObjectName("unit");

			// Set the correct parent to edit in the layout editor.
			for (auto o: QObjectList{_labelName, _editValue, _labelUnit})
			{
				o->setProperty(VariableWidgetBase::propertyNameEditorObject(), QVariant::fromValue(_widget));
			}
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
			QObject::connect(_editValue, &QLineEdit::editingFinished, this, &VariableEdit::Private::onEditingFinished);
			// Only link when not in design mode.
			if (!ObjectExtension::inDesigner())
			{
				_variable.setHandler(this);
			}
			// Execute after the layout is loaded, so the label can be found for sure.
			QTimer::singleShot(0, this, &VariableEdit::Private::connectLabelNameAlt);
			// Get the lout data.
			auto ld = LayoutData::from(_widget);
			// When found and the layout is not readonly.
			if (ld && !ld->getReadOnly())
			{
				// The edit value gets handler for the context menu.
				_editValue->setContextMenuPolicy(Qt::CustomContextMenu);
				// Connect the context menu signal.
				connect(_editValue, &QLineEdit::customContextMenuRequested, this, &VariableEdit::Private::createContextMenu);
			}
		}

		// This method is used to create a on-standard menu.
		void createContextMenu(const QPoint& pos) const
		{
			// When modifiers are met for the context menu.
			if (LayoutData::hasMenuModifiers())
			{
				if (auto ld = LayoutData::from(_widget))
				{
					ld->popupContextMenu(_widget, _editValue->mapToGlobal(pos));
					return;
				}
			}
			auto menu = _editValue->createStandardContextMenu();
			menu->exec(_editValue->mapToGlobal(pos));
			delete menu;
		}

		~Private() override
		{
			// Clear the handler when destructing.
			_variable.setHandler(nullptr);
			delete _labelName;
			delete _editValue;
			delete _labelUnit;
		}

		void variableEventHandler(
			EEvent event,
			const Variable& call_var,
			Variable& link_var,
			bool same_inst
		) override
		{
			SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, Variable::getEventName(event));
			switch (event)
			{
				case veLinked:
				case veIdChanged:
					_widget->applyReadOnly(_readOnly || link_var.isReadOnly());
					_labelUnit->setVisible(!!link_var.getUnit().length());
					_labelUnit->setText(QString::fromStdString(link_var.getUnit()));
					_editValue->setAlignment(Qt::AlignVCenter | (link_var.isNumber() ? Qt::AlignRight : Qt::AlignLeft));
					_editValue->setText(QString::fromStdString(link_var.getCurString()));
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
					_editValue->setText(QString::fromStdString(link_var.getCurString()));
					break;

				case veFlagsChange:
					_widget->applyReadOnly(_readOnly || link_var.isReadOnly());
					break;

				case veConverted:
					_editValue->setText(QString::fromStdString(link_var.getCurString()));
					_labelUnit->setVisible(!!link_var.getUnit().length());
					_labelUnit->setText(QString::fromStdString(link_var.getUnit()));
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
				variableEventHandler(Variable::veValueChange, _variable, _variable, true);
			}
		}

		void onEditingFinished()
		{
			if (!_editValue->isReadOnly() && _editValue->isModified())
			{
				updateValue(false);
			}
		}

		void onDestroyed(QObject* obj = nullptr)// NOLINT(readability-make-member-function-const)
		{
			if (_labelNameAlt && _labelNameAlt == obj)
			{
				_labelName->setVisible(true);
				disconnect(_labelNameAlt, &QLabel::destroyed, this, &VariableEdit::Private::onDestroyed);
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
				connect(_labelNameAlt, &QLabel::destroyed, this, &VariableEdit::Private::onDestroyed);
				// Trigger event to fill in the label text and tool tip.
				_variable.emitEvent(Variable::veUserPrivate);
			}
		}
};

void VariableEdit::Private::LineEdit::keyPressEvent(QKeyEvent* event)
{
	QLineEdit::keyPressEvent(event);
	cast(dynamic_cast<VariableEdit*>(parent())->_p)->keyPressEvent(event);
}

VariableEdit::VariableEdit(QWidget* parent)
	: VariableWidgetBase(parent, this)
{
	_p = new VariableEdit::Private(this);
}

void VariableEdit::applyReadOnly(bool yn)
{
	setFocusPolicy(yn ? Qt::NoFocus : Qt::StrongFocus);
	if (_p)
	{
		auto e = VariableEdit::Private::cast(_p)->_editValue;
		e->setReadOnly(yn);
		// Make the style for readonly have effect on the widget.
		e->style()->polish(e);
	}
}

bool VariableEdit::isRequiredProperty(const QString& name)
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

void VariableEdit::setFocusFrame(bool yn)
{
	VariableEdit::Private::cast(_p)->_editValue->setFrame(yn);
}

bool VariableEdit::hasFocusFrame() const
{
	return VariableEdit::Private::cast(_p)->_editValue->hasFrame();
}

int VariableEdit::nameLevel() const
{
	return VariableEdit::Private::cast(_p)->_nameLevel;
}

void VariableEdit::setNameLevel(int level)
{
	auto p = VariableEdit::Private::cast(_p);
	if (p->_nameLevel != level)
	{
		p->_nameLevel = level;
		p->_variable.emitEvent(Variable::veUserPrivate);
	}
}

}// namespace sf
