#pragma once

#include <QStyledItemDelegate>

class ComboBoxDelegate :public QStyledItemDelegate
	{
		public:
			explicit ComboBoxDelegate(QObject* parent = nullptr);

			QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

			void setEditorData(QWidget* editor, const QModelIndex& index) const override;

			void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

			void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	};

