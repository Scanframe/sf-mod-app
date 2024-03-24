#include <bitset>
#include <QComboBox>
#include <QKeySequenceEdit>
#include <QStandardItemModel>
#include <QSpinBox>
#include <QListView>
#include <QAction>
#include <QLineEdit>
#include <QColorDialog>
#include <QPlainTextEdit>
#include "CommonItemDelegate.h"
#include "qt_utils.h"
#include "Resource.h"
#include "Editor.h"

namespace sf
{

CommonItemDelegate::CommonItemDelegate(QObject* parent)
	:QStyledItemDelegate(parent)
{
}

CommonItemDelegate::EEditorType CommonItemDelegate::getEditorType(const QModelIndex& index) const
{
	return index.model()->data(index, TypeRole).value<EEditorType>();
}

QWidget* CommonItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// The user role is assigned to the type of delegate editor.
	switch (getEditorType(index))
	{
		case etDropDown:
		case etDropDownIndex:
			return new QComboBox(parent);

		case etDropDownFlags:
			return new QListView(parent);

		case etShortcut:
			return new QKeySequenceEdit(parent);

		case etSpinBox:
			return new QSpinBox(parent);

		case etStringList:
		{
			auto pte = new Editor(parent);
/*
			auto sp = pte->sizePolicy();
			sp.setVerticalPolicy(QSizePolicy::Maximum);
			pte->setSizePolicy(sp);
			pte->setMinimumHeight(pte->fontMetrics().xHeight() * 5);
			pte->setContentsMargins(0, 0, 0, 0);
			pte->resize(-1, );
*/
			pte->setMinimumSize(0, pte->fontMetrics().height() * 7);
			return pte;
		}

		case etULongLong:
		{
			auto lineEdit = new QLineEdit(parent);
			// Remove the focus border so the looks the same as the default editor.
			lineEdit->setStyleSheet(QString("%1 {border: 0;}").arg(lineEdit->metaObject()->className()));
			// Allow setting some actions for this edit.
			Q_EMIT addLineEditActions(lineEdit, index);
			return lineEdit;
		}

		case etColorEdit:
		{
			auto lineEdit = new QLineEdit(parent);
			// Remove the focus border so the looks the same as the default editor.
			lineEdit->setStyleSheet(QString("%1 {border: 0;}").arg(lineEdit->metaObject()->className()));
			//
			auto action = lineEdit->addAction(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Edit), lineEdit->palette(), QPalette::Text),
				QLineEdit::TrailingPosition);
			connect(action, &QAction::triggered, [action]()
			{
				if (auto le = qobject_cast<QLineEdit*>(action->parent()))
				{
					QColorDialog dlg(QColor(le->text()), le);
					dlg.setOption(QColorDialog::ShowAlphaChannel);
					if (dlg.exec() == QDialog::Accepted)
					{
						le->setText(QVariant(dlg.currentColor()).toString());
					}
				}
			});
			//
			return lineEdit;
		}

		default:
			// Call the initial method.
			return QStyledItemDelegate::createEditor(parent, option, index);
	}
}

void CommonItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	// The user role is assigned to the type of delegate editor.
	switch (getEditorType(index))
	{
		case etDropDown:
		{
			auto cb = dynamic_cast<QComboBox*>(editor);
			Q_ASSERT(cb);
			auto sl = index.model()->data(index, OptionsRole).toStringList();
			auto v = index.model()->data(index, Qt::EditRole);
			cb->addItems(sl);
			cb->setCurrentIndex((int) sl.indexOf(v.toString()));
			return;
		}

		case etDropDownIndex:
		{
			auto cb = dynamic_cast<QComboBox*>(editor);
			Q_ASSERT(cb);
			auto d = index.model()->data(index, OptionsRole);
			if (d.canConvert<OptionsType>())
			{
				auto options = d.value<OptionsType>();
				for (auto& e: options)
				{
					cb->addItem(e.second, e.first);
				}
				auto v = index.model()->data(index, Qt::EditRole);
				auto idx = indexFromComboBox(cb, v);
				cb->setCurrentIndex(idx);
			}
			else
			{
				auto sl = d.toStringList();
				auto v = index.model()->data(index, Qt::EditRole);
				cb->addItems(sl);
				cb->setCurrentIndex((int) sl.indexOf(v.toString()));
			}
			return;
		}

		case etDropDownFlags:
		{
			auto lv = dynamic_cast<QListView*>(editor);
			Q_ASSERT(lv);
			auto d = index.model()->data(index, OptionsRole);
			if (d.canConvert<OptionsType>())
			{
				auto options = d.value<OptionsType>();
				auto model = new QStandardItemModel(0, 1);
				auto flags = index.model()->data(index, Qt::EditRole).toInt();
				for (auto& option: options)
				{
					auto item = new QStandardItem(option.second);
					item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
					item->setData(flags & option.first.toInt() ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
					item->setData(option.first);
					model->appendRow(item);
				}
				lv->setModel(model);
				const int maxItems = 6;
				int nToShow = maxItems < model->rowCount() ? maxItems : model->rowCount();
				lv->setMinimumSize(lv->width(), nToShow * lv->sizeHintForRow(0) + lv->lineWidth() * 2);
			}
			break;
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

		case etStringList:
		{
			auto pte = dynamic_cast<QPlainTextEdit*>(editor);
			Q_ASSERT(pte);
			pte->setPlainText(index.model()->data(index, Qt::EditRole).toStringList().join('\n'));
			return;
		}

		case etULongLong:
		{
			auto le = dynamic_cast<QLineEdit*>(editor);
			Q_ASSERT(le);
			le->setText(QString("0x%1").arg(index.model()->data(index, Qt::EditRole).value<qulonglong>(), 0, 16));
			return;
		}

		default:
			// Call the initial method.
			QStyledItemDelegate::setEditorData(editor, index);
	}
}

void CommonItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	// The user role is assigned to the type of delegate editor.
	switch (getEditorType(index))
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

		case etDropDownFlags:
		{
			auto lv = dynamic_cast<QListView*>(editor);
			Q_ASSERT(lv);
			int flags = 0;
			if (auto m = dynamic_cast<QStandardItemModel*>(lv->model()))
			{
				for (int row = 0; row < m->rowCount(); row++)
				{
					auto item = m->item(row);
					if (item->data(Qt::CheckStateRole) == Qt::Checked)
					{
						flags |= item->data().toInt();
					}
				}
			}
			model->setData(index, flags, Qt::EditRole);
			break;
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

		case etStringList:
		{
			auto pte = dynamic_cast<QPlainTextEdit*>(editor);
			Q_ASSERT(pte);
			model->setData(index, pte->toPlainText().split('\n'), Qt::EditRole);
			return;
		}

		case etULongLong:
		{
			auto le = dynamic_cast<QLineEdit*>(editor);
			Q_ASSERT(le);
			model->setData(index, le->text().toULongLong(nullptr, 0), Qt::EditRole);
			return;
		}

		default:
			// Call the initial method.
			return QStyledItemDelegate::setModelData(editor, model, index);
	}
}

void CommonItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}

}