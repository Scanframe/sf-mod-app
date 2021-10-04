#pragma once

#include <QStyledItemDelegate>
#include "../global.h"

namespace sf
{

/**
 * @brief Allows a list model to determine the editor for a field.
 */
class _MISC_CLASS CommonItemDelegate :public QStyledItemDelegate
{
	Q_OBJECT

	public:

		/**
		 * @brief Type for the named options available in case of a dropdown.
		 */
		typedef QList<QPair<QVariant, QString>> OptionsType;
		//
		/**
		 * @brief extension on the Qt::ItemDataRole enumerate for retrieving the type of editor from the model.
		 */
		static constexpr int TypeRole = Qt::ItemDataRole::UserRole;
		/**
		 * @brief extension on the Qt::ItemDataRole enumerate for retrieving the options list.
		 */
		static constexpr int OptionsRole = Qt::ItemDataRole::UserRole + 1;

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
			etDropDownFlags,
			etShortcut,
			etSpinBox,
			etULongLong,
			etColorEdit,
		};

		/**
		 * @brief Signal allowing adding actions to a line edit.
		 * @param lineEdit Line edit in question.
		 * @param index Index of the edited item.
		 */
		Q_SIGNAL void addLineEditActions(QLineEdit* lineEdit, const QModelIndex& index) const;

		/**
		 * @brief Constructor.
		 */
		explicit CommonItemDelegate(QObject* parent = nullptr);

		/**
		 * @brief Gets the editor type by calling the modal data() method with #TypeRole.
		 *
		 * @param index Model-index of the field to get the editor type for.
		 * @return Type of the editor.
		 */
		[[nodiscard]] EEditorType getEditorType(const QModelIndex& index) const;

		/**
		 * @brief Overridden from base class.
		 */
		QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

		/**
		 * @brief Overridden from base class.
		 */
		void setEditorData(QWidget* editor, const QModelIndex& index) const override;

		/**
		 * @brief Overridden from base class.
		 */
		void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

		/**
		 * @brief Overridden from base class.
		 */
		void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

}
