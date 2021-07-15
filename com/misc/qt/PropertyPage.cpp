#include <QSpinBox>
#include <QDateTimeEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPlainTextEdit>

#include <gen/gen_utils.h>
#include <QDial>
#include <QKeySequenceEdit>
#include <QLabel>
#include <QGroupBox>
#include "PropertySheetDialog.h"
#include "PropertyPage.h"
#include "Resource.h"

namespace sf
{

PropertyPage::PropertyPage(PropertySheetDialog* parent)
	:QWidget(parent)
{
}

QString PropertyPage::getPageName() const
{
	return QString::fromStdString(demangle(typeid(*this).name()));
}

QString PropertyPage::getPageDescription() const
{
	return QString();
}

QIcon PropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Settings), QPalette::Text);
}

namespace
{

void connectIt(PropertySheetDialog* sheet, QWidget* parent, QWidgetList& rv)
{
	// Iterate through
	for (auto widget: parent->findChildren<QWidget*>(QString(), Qt::FindChildOption::FindDirectChildrenOnly))
	{
		// Label
		if (auto o = qobject_cast<QLabel*>(widget))
		{
			continue;
		}
		// GroupBox
		if (auto o = qobject_cast<QGroupBox*>(widget))
		{
			connectIt(sheet, o, rv);
			continue;
		}
		// CheckBox
		if (auto o = qobject_cast<QCheckBox*>(widget))
		{
			QObject::connect(o, &QCheckBox::stateChanged, [o, sheet](int) -> void
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// LineEdit
		if (auto o = qobject_cast<QLineEdit*>(widget))
		{
			QObject::connect(o, &QLineEdit::textChanged, [o, sheet](const QString&) -> void
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// TextEdit
		if (auto o = qobject_cast<QTextEdit*>(widget))
		{
			QObject::connect(o, &QTextEdit::textChanged, [o, sheet]() -> void
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// QPlainTextEdit
		if (auto o = qobject_cast<QPlainTextEdit*>(widget))
		{
			QObject::connect(o, &QPlainTextEdit::textChanged, [o, sheet]() -> void
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// QComboBox
		if (auto o = qobject_cast<QComboBox*>(widget))
		{
			if (o->isEditable())
			{
				QObject::connect(o, &QComboBox::currentTextChanged, [o, sheet](const QString&) -> void
				{
					sheet->checkModified(o);
				});
			}
			else
			{
				QObject::connect(o, &QComboBox::currentIndexChanged, [o, sheet](int) -> void
				{
					sheet->checkModified(o);
				});
			}
			continue;
		}
		// DateTimeEdit
		if (auto o = qobject_cast<QDateTimeEdit*>(widget))
		{
			QObject::connect(o, &QDateTimeEdit::dateTimeChanged, [o, sheet]() -> void
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// SpinBox
		if (auto o = qobject_cast<QSpinBox*>(widget))
		{
			QObject::connect(o, &QSpinBox::valueChanged, [o, sheet]() -> void
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// DoubleSpinBox
		if (auto o = qobject_cast<QDoubleSpinBox*>(widget))
		{
			QObject::connect(o, &QDoubleSpinBox::valueChanged, [o, sheet]() -> void
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// Dial
		if (auto o = qobject_cast<QDial*>(widget))
		{
			QObject::connect(o, &QDial::valueChanged, [o, sheet]() -> void
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// KeySequenceEdit
		if (auto o = qobject_cast<QKeySequenceEdit*>(widget))
		{
			QObject::connect(o, &QKeySequenceEdit::keySequenceChanged, [o, sheet]() -> void
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// No standard matching control widget was so add it to the returned list.
		rv.append(widget);
	}
}

} // namespace

QWidgetList PropertyPage::connectControls()
{
	// Return value contains the widgets that were not connected.
	QWidgetList rv;
	if (auto sheet = qobject_cast<PropertySheetDialog*>(parent()))
	{
		connectIt(sheet, this, rv);
		qDebug() << objectName() << __FUNCTION__ << rv;
	}
	else
	{
		qWarning() << __FUNCTION__ << "Parent is not of type sf::PropertySheetDialog!";
	}
	return rv;
}

}
