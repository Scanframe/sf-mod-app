#include <QDebug>
#include <QLayout>
#include <QScrollArea>
#include <QResizeEvent>
#include <QCoreApplication>
#include <QFormBuilder>
#include <QPushButton>
#include "FormDialog.h"
#include "../gen/gen_utils.h"
#include "../qt/qt_utils.h"

namespace sf
{

FormDialog::FormDialog(QWidget* parent, Qt::WindowFlags f)
	:QDialog(parent, f)
{
	setModal(true);
	setSizeGripEnabled(true);
	setMinimumSize(100, 50);
	_scrollArea = new QScrollArea(this);
	_scrollArea->setFrameShape(QFrame::Shape::Box);
	_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	//scrollArea->setSizeAdjustPolicy(QScrollArea::SizeAdjustPolicy::AdjustToContentsOnFirstShow);
	// Connect the resizing signal from the FormDialog to the scroll area.
	connect(this, &FormDialog::resizing, [&](QResizeEvent* event)->void
	{
		_scrollArea->setGeometry(QRect(QPoint(), event->size()));
	});
}

void FormDialog::resizeEvent(QResizeEvent* event)
{
	QDialog::resizeEvent(event);
	emit resizing(event);
}

QSize FormDialog::sizeHint() const
{
	// Call the base class method.
	auto sz = QDialog::sizeHint();
	// Iterate through all children of the onCreateFormDialog.
	for (auto* child: children())
	{
		// When the child is a QScrollArea.
		auto sa = dynamic_cast<QScrollArea*>(child);
		if (sa && sa->widget())
		{
			return (sa->widget()->rect() + sa->contentsMargins()).size();
		}
	}
	if (_widget)
	{
		return _widget->size();
	}
	return sz;
}

void FormDialog::Load(QFile file)
{
	// Open the file readonly.
	if (file.open(QFile::ReadOnly))
	{
		QFormBuilder builder;
		// Add the application directory as the plugin directory to find custom plugins.
		builder.addPluginPath(QCoreApplication::applicationDirPath());
		// Create widget from the loaded ui-file.
		delete_null(_widget);
		_widget = builder.load(&file, this);
		// When loading is successful.
		if (_widget)
		{
			// Copy the widgets to this dialog.
			setWindowIcon(_widget->windowIcon());
			setMinimumSize(_widget->size());
			//_minimumSize = _widget->size();
			// Copy the title from the widget.
			setWindowTitle(_widget->windowTitle());
			//
			// Adjust size to the default content size.
			adjustSize();
			// Somehow this is lost.
			_widget->layout()->setContentsMargins(6, 6, 6, 6);
			//
			connect(this, &FormDialog::resizing, [&](QResizeEvent* event)->void
			{
				if (_widget)
				{
					_widget->setGeometry(QRect(QPoint(), geometry().size()));
				}
			});
			//
			connectChildren();
		}
	}
}

void FormDialog::Load2(QFile file)
{
	// Open the file readonly.
	if (file.open(QFile::ReadOnly))
	{
		QFormBuilder builder;
		// Add the application directory as the plugin directory to find custom plugins.
		builder.addPluginPath(QCoreApplication::applicationDirPath());
		//
		delete_null(_widget);
		// Create widget from the loaded ui-file.
		_widget = builder.load(&file, _scrollArea);
		// When loading is successful.
		if (_widget)
		{
			// Copy the widgets to this dialog.
			setWindowIcon(_widget->windowIcon());
			// The widget min size is also the actual size.
			_widget->setMinimumSize(_widget->size());
			// Copy the title from the widget.
			setWindowTitle(_widget->windowTitle());
/*
			// Set the widget onto the layout.
			_scrollArea->setWidget(_widget);
*/
			//
			connectChildren();
		}
	}
	adjustSize();
}

void FormDialog::connectChildren()
{
	//connectByName(_widget, "btnClose", "released", this, "close");
	auto child = findChild<QPushButton*>("btnClose");
	if (child)
	{
		connect(child, &QPushButton::released, this, &FormDialog::close);
	}
}

}
