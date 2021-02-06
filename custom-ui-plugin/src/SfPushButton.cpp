#include "SfPushButton.h"
#include "misc/dbgutils.h"

namespace sf
{

PushButton::PushButton(QWidget* parent)
	:QPushButton(parent)
{
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