#include <iostream>
#include <QMessageBox>
#include <QMetaMethod>
#include "SfPushButton.h"
#include <misc/qt/Macros.h>

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

SF_IMPL_PROP_GSN(qreal, PushButton, FloatSome, this->_floatSome, changed)

PushButton::PushButton(QWidget* parent)
	:ObjectExtension(this), QPushButton(parent)
{
}

bool PushButton::isRequiredProperty(const QString& name)
{
/*
	static const char* keys[] =
		{
			"geometry",
			"toolTip",
			"whatsThis",
			"text",
			"iconSize",
			"styleSheet",
			"sizePolicy",
			"iconSource"
		};
	// Check if passed property name is in the keys list.
	return std::any_of(&keys[0], &keys[sizeof(keys) / sizeof(keys[0])], [name](const char* prop)
	{
		return name == prop;
	});
*/
	return true;
}

void PushButton::mousePressEvent(QMouseEvent* event)
{
	qDebug() << __FUNCTION__ << " IconSource: " << _iconSource;
	// Call overridden method.
	QPushButton::mousePressEvent(event);
}

void PushButton::setIconSource(const QString& text)
{
	setIcon(QIcon(_iconSource = text));
}

QString PushButton::iconSource() const
{
	return _iconSource;
}

void PushButton::changed()
{
//	QMessageBox::information(this, "Changed", "Property Changed");
	qDebug() << __FUNCTION__;
}

void PushButton::connectNotify(const QMetaMethod& signal)
{
	QObject::connectNotify(signal);
	qDebug() << __FUNCTION__ << signal.name() << signal.methodSignature();
}

void PushButton::changed(void* p)
{
	qDebug() << __FUNCTION__ << p;
}

}
