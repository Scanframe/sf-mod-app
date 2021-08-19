#include <QSpinBox>
#include <QDateTimeEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QToolButton>
#include <QPlainTextEdit>
#include <QLayout>
#include <QDial>
#include <QTreeView>
#include <QKeySequenceEdit>
#include <QLabel>
#include <QGroupBox>
#include <gen/gen_utils.h>
#include "PropertySheetDialog.h"
#include "PropertyPage.h"
#include "Resource.h"

namespace sf
{

PropertyPage::PropertyPage(QWidget* parent)
	:QWidget(parent)
	 , _sheet(qobject_cast<PropertySheetDialog*>(parent))
{
}

QString PropertyPage::getPageName() const
{
	return QString::fromStdString(demangle(typeid(*this).name()));
}

QString PropertyPage::getPageDescription() const
{
	return {};
}

QIcon PropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Settings), QPalette::Text);
}

namespace
{

void connectIt(PropertySheetDialog* sheet, QObject* parent, QWidgetList& rv) // NOLINT(misc-no-recursion)
{
	if (!sheet)
	{
		return;
	}
/*
	// Layout
	if (auto w = qobject_cast<QWidget*>(parent))
	{
		if (auto o = w->layout())
		{
			qDebug() << "Layout: " << o->objectName();
			connectIt(sheet, o, rv);
		}
	}
*/
	// Iterate through
	for (auto widget: parent->findChildren<QWidget*>(QString(), Qt::FindChildOption::FindDirectChildrenOnly))
	{
		// Label
		if (qobject_cast<QLabel*>(widget))
		{
			continue;
		}
		// ToolButton
		if (qobject_cast<QToolButton*>(widget))
		{
			continue;
		}
		// GroupBox
		if (auto o = qobject_cast<QGroupBox*>(widget))
		{
			connectIt(sheet, o, rv);
			continue;
		}
		if (auto o = qobject_cast<QTabWidget*>(widget))
		{
			for (int i = 0; i < o->count(); i++)
			{
				connectIt(sheet, o->widget(i), rv);
			}
			continue;
		}
		// CheckBox
		if (auto o = qobject_cast<QCheckBox*>(widget))
		{
			QObject::connect(o, &QCheckBox::stateChanged, [o, sheet](int)
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// LineEdit
		if (auto o = qobject_cast<QLineEdit*>(widget))
		{
			QObject::connect(o, &QLineEdit::textChanged, [o, sheet](const QString&)
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// TextEdit
		if (auto o = qobject_cast<QTextEdit*>(widget))
		{
			QObject::connect(o, &QTextEdit::textChanged, [o, sheet]()
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// QPlainTextEdit
		if (auto o = qobject_cast<QPlainTextEdit*>(widget))
		{
			QObject::connect(o, &QPlainTextEdit::textChanged, [o, sheet]()
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
				QObject::connect(o, &QComboBox::currentTextChanged, [o, sheet](const QString&)
				{
					sheet->checkModified(o);
				});
			}
			else
			{
				QObject::connect(o, &QComboBox::currentIndexChanged, [o, sheet](int)
				{
					sheet->checkModified(o);
				});
			}
			continue;
		}
		// DateTimeEdit
		if (auto o = qobject_cast<QDateTimeEdit*>(widget))
		{
			QObject::connect(o, &QDateTimeEdit::dateTimeChanged, [o, sheet]()
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// SpinBox
		if (auto o = qobject_cast<QSpinBox*>(widget))
		{
			QObject::connect(o, &QSpinBox::valueChanged, [o, sheet]()
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// DoubleSpinBox
		if (auto o = qobject_cast<QDoubleSpinBox*>(widget))
		{
			QObject::connect(o, &QDoubleSpinBox::valueChanged, [o, sheet]()
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// Dial
		if (auto o = qobject_cast<QDial*>(widget))
		{
			QObject::connect(o, &QDial::valueChanged, [o, sheet]()
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// KeySequenceEdit
		if (auto o = qobject_cast<QKeySequenceEdit*>(widget))
		{
			QObject::connect(o, &QKeySequenceEdit::keySequenceChanged, [o, sheet]()
			{
				sheet->checkModified(o);
			});
			continue;
		}
		// QAbstractItemModel
		if (auto o = qobject_cast<QTreeView*>(widget))
		{
			// When no model is connected avoid erroneous connections.
			if (o->model())
			{
				QObject::connect(o->model(), &QAbstractItemModel::dataChanged, [sheet, widget]()
				{
					sheet->checkModified(widget);
				});
				QObject::connect(o->model(), &QAbstractItemModel::rowsInserted, [sheet, widget]()
				{
					sheet->checkModified(widget);
				});
				QObject::connect(o->model(), &QAbstractItemModel::rowsMoved, [sheet, widget]()
				{
					sheet->checkModified(widget);
				});
				QObject::connect(o->model(), &QAbstractItemModel::rowsRemoved, [sheet, widget]()
				{
					sheet->checkModified(widget);
				});
			}
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
	connectIt(_sheet, this, rv);
	return rv;
}

void PropertyPage::updatePage()
{
	// Left empty and must be implemented in a derived class.
}

bool PropertyPage::isPageModified() const
{
	// Left empty and must be implemented in a derived class.
	return false;
}

void PropertyPage::afterPageApply(bool was_modified)
{
	// Left empty and can be implemented in a derived class.
}

void PropertyPage::applyPage()
{
	// Left empty and must be implemented in a derived class.
}

void PropertyPage::stateSaveRestore(QSettings& settings, bool save)
{
	// Left empty and must be implemented in a derived class.
}

}
