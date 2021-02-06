#include "SfPushButton.h"
#include "misc/dbgutils.h"

SfPushButton::SfPushButton(QWidget* parent)
	:QPushButton(parent)
{
}

void SfPushButton::mousePressEvent(QMouseEvent* event)
{
	std::clog << __FUNCTION__ << " IconSource: " << IconSource << std::endl;
	// Call overridden method.
	QPushButton::mousePressEvent(event);
}

void SfPushButton::setIconSource(const QString& text)
{
	setIcon(QIcon(IconSource = text));
}

QString SfPushButton::iconSource() const
{
	return IconSource;
}
