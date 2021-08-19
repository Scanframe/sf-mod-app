#include <QComboBox>
#include <QLineEdit>
#include <QKeySequenceEdit>
#include <QStandardItemModel>
#include <QSpinBox>
#include "CommonItemDelegate.h"

namespace sf
{

CommonItemDelegate::CommonItemDelegate(QObject* parent)
	:QStyledItemDelegate(parent)
{
}

QWidget* CommonItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// The user role is assigned to the type of delegate editor.
	switch (index.model()->headerData(index.column(), Qt::Horizontal, Qt::UserRole).toInt())
	{
		case etDropDown:
		case etDropDownIndex:
		{
			return new QComboBox(parent);
		}

		case etShortcut:
		{
			return new QKeySequenceEdit(parent);
		}

		case etSpinBox:
		{
			return new QSpinBox(parent);
		}
	}
	// Call the initial method.
	return QStyledItemDelegate::createEditor(parent, option, index);
}

void CommonItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	// The user role is assigned to the type of delegate editor.
	switch (index.model()->headerData(index.column(), Qt::Horizontal, Qt::UserRole).toInt())
	{
		case etDropDown:
		{
			auto cb = dynamic_cast<QComboBox*>(editor);
			Q_ASSERT(cb);
			auto sl = index.model()->data(index, Qt::UserRole).toStringList();
			auto v = index.model()->data(index, Qt::EditRole);

			cb->addItems(sl);
			cb->setCurrentIndex((int)sl.indexOf(v.toString()));
			return;
		}

		case etDropDownIndex:
		{
			auto cb = dynamic_cast<QComboBox*>(editor);
			Q_ASSERT(cb);
			cb->addItems(index.model()->data(index, Qt::UserRole).toStringList());
			cb->setCurrentIndex(index.model()->data(index, Qt::EditRole).toInt());
			return;
		}

		case etShortcut:
		{
			auto kse = dynamic_cast<QKeySequenceEdit*>(editor);
			Q_ASSERT(kse);
			kse->setKeySequence(index.model()->data(index, Qt::EditRole).value<QKeySequence>());
			return;
		}

		case etSpinBox:
		{
			auto sb = dynamic_cast<QSpinBox*>(editor);
			Q_ASSERT(sb);
			sb->setValue(index.model()->data(index, Qt::EditRole).toInt());
			return;
		}
	}
	// Call the initial method.
	return QStyledItemDelegate::setEditorData(editor, index);

}

void CommonItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	// The user role is assigned to the type of delegate editor.
	switch ( model->headerData(index.column(), Qt::Horizontal, Qt::UserRole).toInt())
	{
		case etDropDown:
		{
			auto cb = dynamic_cast<QComboBox*>(editor);
			Q_ASSERT(cb);
			model->setData(index, cb->currentText(), Qt::EditRole);
			return;
		}

		case etDropDownIndex:
		{
			auto cb = dynamic_cast<QComboBox*>(editor);
			Q_ASSERT(cb);
			model->setData(index, cb->currentIndex(), Qt::EditRole);
			return;
		}

		case etShortcut:
		{
			auto kse = dynamic_cast<QKeySequenceEdit*>(editor);
			Q_ASSERT(kse);
			model->setData(index, kse->keySequence(), Qt::EditRole);
			return;
		}

		case etSpinBox:
		{
			auto sb = dynamic_cast<QSpinBox*>(editor);
			Q_ASSERT(sb);
			model->setData(index, sb->value(), Qt::EditRole);
			return;
		}

	}
	// Call the initial method.
	return QStyledItemDelegate::setModelData(editor, model, index);
}

void CommonItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}

}