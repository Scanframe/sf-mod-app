#include "AxesCoordEdit.h"
#include <QHBoxLayout>
#include <QLabel>
#include <misc/gen/ScriptEngine.h>

namespace sf
{

struct AxesCoordEdit::Private : QObject
{
	public:
		AxesCoordEdit* _widget{nullptr};
		QHBoxLayout* _layout{nullptr};

		explicit Private(AxesCoordEdit* widget);

		void createContextMenu(const QPoint& pos) const;

		QLabel* _labelName;
};

AxesCoordEdit::Private::Private(AxesCoordEdit* widget)
	: _widget(widget)
{
	_widget->_p = this;
	_labelName = new QLabel("&Name", _widget);
}

// This method is used to create a non-standard menu.
void AxesCoordEdit::Private::createContextMenu(const QPoint& pos) const
{
	/*
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
*/
}

AxesCoordEdit::AxesCoordEdit(QWidget* parent)
	: QWidget(parent)
	, ObjectExtension(parent)
{
	// Is assigning itself to '_p' member.
	new AxesCoordEdit::Private(this);
}

bool AxesCoordEdit::isRequiredProperty(const QString& name)
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

}// namespace sf
