#include "SfPushButton.h"
#include "misc/dbgutils.h"

/*
Styling a custom control is as follows the '::' is replaced with '--' for the selector.

sf--PushButton
{
	background-color: magenta;
	border: 1px solid green;
}

*/

namespace sf
{

PushButton::PushButton(QWidget* parent)
	:ObjectExtension(this), QPushButton(parent)
{
}

bool PushButton::isRequiredProperty(const QString& name)
{
	static const char* keys[] =
		{
			"geometry",
			"toolTip",
			"whatsThis",
			"text",
			"iconSize",
			"styleSheet",
			"iconSource"
		};
	// Check if passed property name is in the keys list.
	return std::any_of(&keys[0], &keys[sizeof(keys) / sizeof(keys[0])], [name](const char* prop)
	{
		return name == prop;
	});
}

void PushButton::mousePressEvent(QMouseEvent* event)
{
	std::clog << __FUNCTION__ << " IconSource: " << IconSource << std::endl;
	// Call overridden method.
	QPushButton::mousePressEvent(event);
}

void PushButton::setIconSource(const QString& text)
{
	setIcon(QIcon(IconSource = text));
}

QString PushButton::iconSource() const
{
	return IconSource;
}

} // namespace