#pragma once

#include "../gen/Variable.h"

namespace sf
{

struct VariableWidgetBasePrivate :VariableHandler
{
	QString _id{};
	Variable _variable{};
	bool _converted{false};
	bool _readOnly{false};

	void keyPressEvent(QKeyEvent* event);
};


}
