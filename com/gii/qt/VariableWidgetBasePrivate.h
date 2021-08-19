#pragma once

#include <QLabel>
#include "../gen/Variable.h"
#include "VariableWidgetBase.h"

namespace sf
{

struct VariableWidgetBase::PrivateBase :VariableHandler
{
	QString _id{};
	Variable _variable{};
	bool _converted{false};
	bool _readOnly{false};

	void keyPressEvent(QKeyEvent* event);

	/**
	 * @brief Gets the label which the passed widget has as buddy.
	 *
	 * Assumed is that there is only one label.
	 *
	 * @param widget Widget which the has set as buddy.
	 * @return When not found nullptr.
	 */
	QLabel* findLabelByBuddy(QWidget* widget);

};


}
