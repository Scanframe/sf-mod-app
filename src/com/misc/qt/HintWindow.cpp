#include "HintWindow.h"
#include "../gen/ScriptObject.h"
#include "Globals.h"
#include <QGuiApplication>

namespace sf
{

HintWindow::HintWindow(QWidget* parent)
	: QLabel(parent)
{
	setFrameStyle(QFrame::Shape::StyledPanel);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
}

void HintWindow::setPosition(const QPoint& pt)
{
	move(pt + _offset);
}

void HintWindow::setOffset(const QPoint& pt)
{
	_offset = pt;
}

void HintWindow::setOffset(int x, int y)
{
	_offset.setX(x);
	_offset.setY(y);
}

void HintWindow::setActive(bool active)
{
	setVisible(active);
}

struct HintWindowScriptObject : HintWindow
	, ScriptObject
{
		explicit HintWindowScriptObject(const ScriptObject::Parameters& params)
			: HintWindow(getGlobalParent())
			, ScriptObject("HintWindow", params._parent)
		{}

		bool getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flag_set) override;

		[[nodiscard]] const IdInfo* getInfo(const std::string& name) const override;

		void destroyObject(bool& should_delete) override;

		static ScriptObject::IdInfo _info[];

		enum EPosition
		{
			MousePosition,
			/** Window is shown at the current mouse position. */
			WidgetPosition,
			/** Window is shown at the active widget upper left corner. */
		};
		EPosition _position{MousePosition};
};

#define SID_TEXT 1
#define SID_OFFSET 2
#define SID_ACTIVE 3
#define SID_POSITION 4

ScriptObject::IdInfo HintWindowScriptObject::_info[] = {
	{SID_TEXT, ScriptObject::idVariable, "Text", 0, nullptr},
	{SID_OFFSET, ScriptObject::idFunction, "offset", 2, nullptr},
	{SID_ACTIVE, ScriptObject::idVariable, "Active", 0, nullptr},
	{SID_POSITION, ScriptObject::idVariable, "Position", 0, nullptr},
};

bool HintWindowScriptObject::getSetValue(const ScriptObject::IdInfo* info, Value* value, Value::vector_type* params, bool flag_set)
{
	switch (info->_index)
	{
		default:
			// Signal that the request was not handled.
			return false;

		case SID_TEXT:
			if (flag_set)
			{
				setText(value->getQString());
			}
			else
			{
				value->set(text());
			}
			break;

		case SID_ACTIVE:
			if (flag_set)
			{
				setVisible(value->getInteger());
				switch (_position)
				{
					case WidgetPosition:
						if (auto wgt = qobject_cast<QWidget*>(qApp->focusObject()))// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
						{
							QPoint pt(0, 0);
							wgt->mapToGlobal(pt);
							setPosition(pt);
						}
						break;

					case MousePosition:
						setPosition(QCursor::pos());
						break;
				}
			}
			else
			{
				value->set(isVisible());
			}
			break;

		case SID_POSITION:
			if (flag_set)
			{
				_position = clip<EPosition>((EPosition) value->getInteger(), MousePosition, WidgetPosition);
			}
			else
			{
				value->set(_position);
			}
			break;

		case SID_OFFSET:
			setOffset(static_cast<int>((*params)[0].getInteger()), static_cast<int>((*params)[1].getInteger()));
			break;
	}
	// Signal that the request was handled.
	return true;
}

const ScriptObject::IdInfo* HintWindowScriptObject::getInfo(const std::string& name) const
{
	for (auto& i: _info)
	{
		if (i._name == name)
		{
			// Assign this pointer to info structure to reference owner of this member.
			i._data = const_cast<HintWindowScriptObject*>(this);
			return &i;
		}
	}
	return getInfoUnknown();
}

void HintWindowScriptObject::destroyObject(bool& should_delete)
{
	should_delete = true;
}

SF_REG_CLASS(
	ScriptObject, ScriptObject::Parameters, Interface,
	HintWindowScriptObject, "HintWindow", "Floating hint window."
)

}// namespace sf
