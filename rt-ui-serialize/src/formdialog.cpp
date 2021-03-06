#include <QDebug>
#include <QScrollArea>
#include <QResizeEvent>
#include "formdialog.h"
#include "misc/gen/dbgutils.h"

FormDialog::FormDialog(QWidget* parent, Qt::WindowFlags f)
	:QDialog(parent, f)
{
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
		//qDebug() << "child->objectName():" << _RTTI_NAME(*child).c_str();
		// When the child is a QScrollArea.
		auto sa = dynamic_cast<QScrollArea*>(child);
		if (sa && sa->widget())
		{
			// qDebug() << "Class:" << _RTTI_NAME(*this) << sa->contentsMargins();
			return (sa->widget()->rect() + sa->contentsMargins()).size();
		}
	}
	return sz;
}

