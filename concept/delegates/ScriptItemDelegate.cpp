#include "ComboBoxDelegate.h"
#include <QComboBox>
#include <QLineEdit>
#include <QStandardItemModel>

ComboBoxDelegate::ComboBoxDelegate(QObject* parent)
	:QStyledItemDelegate(parent)
{
}

QWidget* ComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	auto editor = new QComboBox(parent);
	auto pal = editor->palette();
	pal.setColor(editor->backgroundRole(), Qt::red);
	editor->setPalette(pal);

	// Set text alignment when a standard item model is used.
	if (auto model = dynamic_cast<const QStandardItemModel*>(index.model()))
	{
		// Not perfect but allows drawing
//		editor->setEditable(true);
//		editor->lineEdit()->setReadOnly(true);
//		editor->lineEdit()->setAlignment(Qt::AlignCenter);
		//editor->setAlignment(model->item(index.row(), index.column())->textAlignment());
	}

	for (int i = 0; i < 100; i++)
	{
		editor->addItem(QString("Value (%1)").arg(i), i);
	}
	editor->setFrame(false);
	return editor;
}

void ComboBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	int value = index.model()->data(index, Qt::EditRole).toInt();
	auto comboBox = dynamic_cast<QComboBox*>(editor);
	comboBox->setCurrentIndex(value);
}

void ComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
	const QModelIndex& index) const
{
	auto comboBox = dynamic_cast<QComboBox*>(editor);
	int value = comboBox->currentIndex();
	model->setData(index, value, Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
	const QModelIndex&/* index */) const
{
	editor->setGeometry(option.rect);
}
