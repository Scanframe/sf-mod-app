#pragma once

#include <QStyledItemDelegate>

namespace sf
{

class CommonItemDelegate :public QStyledItemDelegate
{
	public:

		/**
		 * @brief Possible inline editors for a column in the list model.
		 */
		enum EEditorType
		{
			etDefault,
			etEdit,
			etTextEdit,
			etDropDown,
			etDropDownIndex,
			etDropDownEdit,
			etShortcut,
		};

		explicit CommonItemDelegate(QObject* parent = nullptr);

		QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

		void setEditorData(QWidget* editor, const QModelIndex& index) const override;

		void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

		void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

}
