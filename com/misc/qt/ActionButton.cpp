#include "ActionButton.h"
#include "../gen/dbgutils.h"

namespace sf
{

ActionButton::ActionButton(QWidget* parent)
	:QPushButton(parent)
	 , _action(nullptr)
{
}

void ActionButton::setAction(QAction* action)
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
	// Store the action
	_action = action;
	// configure the button
	updateButtonStatusFromAction();
	// Connect the action and the button so that when the action is changed the button is changed too!
	connect(action, &QAction::changed,
		this, &ActionButton::updateButtonStatusFromAction);
	// Connect the button to the slot that forwards the signal to the action
	connect(this, &ActionButton::clicked, _action, &QAction::trigger);
}

QAction* ActionButton::getAction()
{
	return _action;
}

void ActionButton::updateButtonStatusFromAction()
{
	if (!_action)
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
		setText(_action->text());
		setStatusTip(_action->statusTip());
		setToolTip(_action->toolTip());
		setIcon(_action->icon());
		setEnabled(_action->isEnabled());
		setCheckable(_action->isCheckable());
		setChecked(_action->isChecked());
	}
}

}