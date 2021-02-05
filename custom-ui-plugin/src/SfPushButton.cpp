#include "SfPushButton.h"
#include "misc/dbgutils.h"

SfPushButton::SfPushButton(QWidget* parent)
	:QPushButton(parent)
{
}

void SfPushButton::mousePressEvent(QMouseEvent* event)
{
	// ToDo something
	qDebug() << __FUNCTION__;
	std::clog << __FUNCTION__ << std::endl;
	// Call overridden method.
	QPushButton::mousePressEvent(event);
}