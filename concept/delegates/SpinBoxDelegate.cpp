#include "SpinBoxDelegate.h"
#include <QSpinBox>
#include <QStandardItemModel>

SpinBoxDelegate::SpinBoxDelegate(QObject* parent)
	:QStyledItemDelegate(parent)
{
}

QWidget* SpinBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,	const QModelIndex& index) const
{
	auto editor = new QSpinBox(parent);
	auto pal = editor->palette();
	pal.setColor(editor->backgroundRole(), Qt::red);
	editor->setPalette(pal);
	// Set text alignment when a standard item model is used.
	if (auto model = dynamic_cast<const QStandardItemModel*>(index.model()))
	{
		editor->setAlignment(model->item(index.row(), index.column())->textAlignment());
	}
	editor->setAutoFillBackground(true);
	editor->setFrame(false);
	editor->setMinimum(0);
	editor->setMaximum(10);
	return editor;
}

void SpinBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	int value = index.model()->data(index, Qt::EditRole).toInt();
	auto spinBox = dynamic_cast<QSpinBox*>(editor);
	spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
	const QModelIndex& index) const
{
	auto spinBox = dynamic_cast<QSpinBox*>(editor);
	spinBox->interpretText();
	int value = spinBox->value();

	model->setData(index, value, Qt::EditRole);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
	const QModelIndex&/* index */) const
{
	editor->setGeometry(option.rect);
}
