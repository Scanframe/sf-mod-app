#include <QToolButton>
#include <QStyle>
#include <QGuiApplication>
#include <misc/qt/Resource.h>
#include "LayoutWidget.h"
#include "InformationSelectDialog.h"
#include "InformationIdEdit.h"

namespace sf
{

InformationIdEdit::InformationIdEdit(QWidget* parent)
	:QLineEdit(parent)
	 , ObjectExtension(this)
	 , _typeId(Gii::Variable)
{
	// Create a clear button with icon
	btnOpenDialog = new QToolButton(this);
	btnOpenDialog->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Search), QPalette::ColorRole::ButtonText));
	btnOpenDialog->setCursor(Qt::ArrowCursor);
	btnOpenDialog->setToolTip(tr("Open select dialog."));
	//
	auto margin = contentsMargins();
	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	margin.setRight(margin.right() + btnOpenDialog->sizeHint().width() + frameWidth + 2);
	setContentsMargins(margin);
	setId(0);
	// signals, clear lineEdit if btn pressed; change btn visibility on input
	connect(btnOpenDialog, &QToolButton::clicked, [&](){selectDialog();});
	// Context menu in case it needs to be modified.
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QLineEdit::customContextMenuRequested, [&](const QPoint& pos)
	{
		if (QGuiApplication::keyboardModifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
		{
			if (auto lw = LayoutWidget::getLayoutWidgetOf(this))
			{
				lw->popupContextMenu(this, mapToGlobal(pos));
				return;
			}
		}
		auto menu = createStandardContextMenu();
		menu->exec(mapToGlobal(pos));
		delete menu;
	});
	// Limit the amount of characters by default. (enough for 64 bit hex value)
	setMaxLength(24);
}

void InformationIdEdit::resizeEvent(QResizeEvent*)
{
	QSize sz = btnOpenDialog->sizeHint();
	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	btnOpenDialog->move(rect().right() - frameWidth - sz.width(), (rect().bottom() + 1 - sz.height()) / 2);
}

bool InformationIdEdit::selectDialog(QWidget* parent)
{
	auto ids = InformationSelectDialog(parent ? parent : btnOpenDialog).execute(Gii::Single, _typeId);
	// Only when not empty set the ID.
	if (!ids.empty())
	{
		setId(ids.at(0));
		return true;
	}
	return false;
}

void InformationIdEdit::setId(Gii::IdType id)
{
	setText(QString("0x%1").arg(id, 0, 16));
}

Gii::IdType InformationIdEdit::getId()
{
	return text().toULongLong(nullptr, 0);
}

void InformationIdEdit::focusOutEvent(QFocusEvent* event)
{
	QLineEdit::focusOutEvent(event);
	// Makes the text format to a hex value.
	setId(getId());
}

Gii::TypeId InformationIdEdit::getTypeId() const
{
	return _typeId;}

void InformationIdEdit::setTypeId(Gii::TypeId typeId)
{
	if (_typeId != typeId)
	{
		_typeId = typeId;
	}
}

}
