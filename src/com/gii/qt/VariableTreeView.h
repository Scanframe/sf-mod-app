#pragma once
#include <QTreeView>
#include <gii/global.h>
#include <gii/qt/VariableListModel.h>

namespace sf
{

/**
 * @brief Implements a tree view (table) for a variable list.
 */
class _GII_CLASS VariableTreeView final : public QTreeView
{
	public:
		/**
		 * @brief Constructor.
		 * @param parent Parent and owner of the instance.
		 */
		explicit VariableTreeView(QWidget* parent = nullptr);

		/**
	 * @brief Gets the #sf::VariableListModel attached to this instance.
	 * @return Model pointer.
	 */
		VariableListModel* variableListModel() const;

	protected:
		typedef QTreeView base_type;

		/**
		 * @brief Gets the first editable column number.
		 * @return -1 when no editable column was found.
		 */
		int editableColumn() const;

		/**
		 * @brief Edits the selected row when editable.
		 * @return True when editable otherwise false.
		 */
		bool editRow() const;

		/**
		 * @brief Toggles the row check box in the view for the current selected row.
		 * @return True when actually toggled.
		 */
		bool toggleRow();

		/**
		 * @brief Overridden from base class.
		*/
		void keyPressEvent(QKeyEvent* event) override;

		/**
		 * @brief Initialize selection of the row.
		*/
		void initSelection() const;
};

}// namespace sf
