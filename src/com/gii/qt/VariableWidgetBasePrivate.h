#pragma once

#include <QLabel>
#include <gii/gen/Variable.h>
#include <gii/qt/VariableWidgetBase.h>

namespace sf
{

struct VariableWidgetBase::PrivateBase : VariableHandler
{
		qulonglong _id{0};
		Variable _variable{};
		bool _converted{false};
		bool _readOnly{false};

		void keyPressEvent(const QKeyEvent* event);

		/**
	 * @brief Gets the label which the passed widget has as buddy.
	 *
	 * Assumed is that there is only one label.
	 *
	 * @param widget Widget which the has set as buddy.
	 * @return When not found nullptr.
	 */
		static QLabel* findLabelByBuddy(const QWidget* widget);
};

}// namespace sf
