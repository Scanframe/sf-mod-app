#include "ActionButton.h"
#include "../gen/dbgutils.h"

namespace sf
{

ActionButton::ActionButton(QWidget* parent)
	:QPushButton(parent)
	 , ObjectExtension(this)
	 , _action(nullptr)
{
}

bool ActionButton::isRequiredProperty(const QString& name)
{
	return true;
}

void ActionButton::setAction(QAction* action)
{
	// Store the action
	_action = action;
	// TODO: disconnect the other one maybe?
	connectAction(action);
}

void ActionButton::connectAction(QAction* action)
{
	// if I've got already an action associated to the button
	// remove all connections
	if (_action && _action != action)
	{
		disconnect(_action, &QAction::changed,
			this, &ActionButton::updateButtonStatusFromAction);
		disconnect(this, &ActionButton::clicked,
			_action, &QAction::trigger);
	}
	// configure the button
	updateButtonStatusFromAction();
	// Connect the action and the button so that when the action is changed the button is changed too!
	connect(action, &QAction::changed,
		this, &ActionButton::updateButtonStatusFromAction);
	// Connect the button to the slot that forwards the signal to the action
	connect(this, &ActionButton::clicked, _action, &QAction::trigger);
}

QAction* ActionButton::getAction() const
{
	return _action;
}

void ActionButton::updateButtonStatusFromAction()
{
	QAction* a = _action;
	//
	if (!a && !_actionName.isEmpty() && parent())
	{
		// Find sibling action.
		a = parent()->findChild<QAction*>(_actionName, Qt::FindChildOption::FindDirectChildrenOnly);
	}
	// Skip when in designer.
	if (!inDesigner())
	{
		if (!a)
		{
			auto name = QString::fromStdString(SF_RTTI_NAME(*this));
			setText(name);
			setStatusTip(name);
			setToolTip(name);
			setIcon(QIcon());
			setEnabled(false);
			setCheckable(false);
			setChecked(false);
		}
		else
		{
			setText(a->text());
			setStatusTip(a->statusTip());
			setToolTip(a->toolTip());
			setIcon(a->icon());
			setEnabled(a->isEnabled());
			setCheckable(a->isCheckable());
			setChecked(a->isChecked());
		}
	}
}

QString ActionButton::getActionByName() const
{
	return _action ? _action->objectName() : _actionName;
}

void ActionButton::setActionByName(const QString& name)
{
	_actionName = name;
	updateButtonStatusFromAction();
}

}