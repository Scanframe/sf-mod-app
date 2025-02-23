#pragma once
#include "misc/qt/ObjectExtension.h"
#include <QTreeView>
#include <gii/global.h>

namespace sf
{

// Forward definition.
class VariableListModel;

/**
 * @brief Implements a tree view (table) for a variable list.
 */
class _GII_CLASS VariableTreeView final
	: public QTreeView
	, public ObjectExtension

{
		Q_OBJECT
		Q_PROPERTY(int nameLevel READ nameLevel WRITE setNameLevel)
		Q_PROPERTY(int rowCheckBox READ rowCheckBox WRITE setRowCheckBox)

	public:
		/**
		 * @brief Constructor.
		 * @param parent Parent and owner of the instance.
		 */
		explicit VariableTreeView(QWidget* parent = nullptr);

		/**
		 * @brief Destructor.
		 */
		~VariableTreeView() override;

		/**
		 * @brief Overridden from base class.
		 */
		bool isRequiredProperty(const QString& name) override;
		/**
	 * @brief Gets the #sf::VariableListModel attached to this instance.
	 * @return Model pointer.
	 */
		VariableListModel* variableListModel() const;
		/**
		 * @brief Gets the levels of names displayed of the variables in the list.
		 */
		[[nodiscard]] int nameLevel() const;
		/**
		 * @brief Sets the levels of names displayed of the variables in the list.
		 */
		void setNameLevel(int level = -1) const;

		/**
		 * @brief Enables the selection of rows.
		 * The #sf::Variable::getData() returns true when selected.
		 */
		void setRowCheckBox(bool enabled) const;

		/**
		 * @brief Gets if the row selection is enabled.
		 * By default, the row selection is enabled.
		 */
		bool rowCheckBox() const;

	protected:
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

	private:
		struct Private;
		Private* _p;

		Q_DISABLE_COPY(VariableTreeView)
};

}// namespace sf
