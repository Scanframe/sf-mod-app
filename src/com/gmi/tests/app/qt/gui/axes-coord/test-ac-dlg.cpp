#include "test-ac-dlg.h"
#include <QDebug>
#include <QFormBuilder>
#include <QLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>

namespace sf
{

AxesCoordDialog::AxesCoordDialog(QWidget* parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setModal(true);
	setSizeGripEnabled(true);
	setMinimumSize(100, 50);
	_scrollArea = new QScrollArea(this);
	_scrollArea->setFrameShape(QFrame::Shape::Box);
	_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	//scrollArea->setSizeAdjustPolicy(QScrollArea::SizeAdjustPolicy::AdjustToContentsOnFirstShow);
	// Connect the resizing signal from the AxesCoordDialog to the scroll area.
	connect(this, &AxesCoordDialog::resizing, [&](QResizeEvent* event) -> void {
		_scrollArea->setGeometry(QRect(QPoint(), event->size()));
	});
}

void AxesCoordDialog::resizeEvent(QResizeEvent* event)
{
	QDialog::resizeEvent(event);
	Q_EMIT resizing(event);
}

void AxesCoordDialog::connectChildren()
{
	//connectByName(_widget, "btnClose", "released", this, "close");
	auto child = findChild<QPushButton*>("btnClose");
	if (child)
	{
		connect(child, &QPushButton::released, this, &AxesCoordDialog::close);
	}
}

}// namespace sf
