#pragma once
#include <QAbstractItemView>
#include <QAbstractListModel>
#include <gii/gen/Variable.h>
#include <gii/global.h>

namespace sf
{

/**
 * @brief List model for a #sf::Variable list.
 */
class _GII_CLASS VariableListModel final
	: public QAbstractListModel
	, protected VariableHandler
{
		Q_OBJECT

	public:
		/**
		 * @brief Constructor.
		 */
		explicit VariableListModel(QObject* parent = nullptr);

		/**
		 * @brief Gets the levels of names displayed of the variables in the list.
		 */
		[[nodiscard]] int nameLevel() const;

		/**
		 * @brief Sets the levels of names displayed of the variables in the list.
		 */
		void setNameLevel(int level);

		/**
		 * @brief Gets if the row selection is enabled.
		 * By default, the row selection is enabled.
		 */
		[[nodiscard]] bool rowCheckBox() const;

		/**
		 * @brief Enables the selection of rows.
		 * The #sf::Variable::getData() returns true when selected.
		 */
		void setRowCheckBox(bool enabled);

		/**
		 * @brief Displayable columns.
		 */
		enum EField : int
		{
			cId,
			cName,
			cValue,
			cUnit,
			cFlags
		};

		/**
		 * @brief Creates a #sf::CommonItemDelegate instance for the passed view.
		 */
		void setDelegates(QAbstractItemView* view);

		/**
		 * @brief Refresh the attached viewer.
		 */
		void refresh();

		/**
		 * @brief Update all values in the given column.
		 */
		void updateColumn(int column);

		/**
		 * @brief Update all values in the column for given field.
		 */
		void updateField(EField field);

		/**
		 * @brief Update all values in all columns.
		 */
		void updateAll();

		/**
		 * @brief Overridden from base class to provide column display names.
		 */
		[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

		/**
		 * @brief Overridden from base class to provide the amount of rows.
		 */
		[[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;

		/**
		 * @brief Overridden from base class to provide the data to present.
		 */
		[[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

		/**
		 * @brief Overridden from base class to provide the amount of columns.
		 */
		[[nodiscard]] int columnCount(const QModelIndex& parent) const override;

		/**
		 * @brief Overridden from base class to provide flags on an item by index.
		 */
		[[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;

		/**
		 * @brief Adds local or global variable to this instance by pointer.
		 * Internally a client copy of the sf::Variable is added.
		 * @param var Variable to add.
		 */
		void addVariable(const Variable* var);

		/**
		 * @brief Adds local or global variables to this instance.
		 * @param list List of Variables.
		 */
		void addVariables(const InformationTypes::Vector& list);

		/**
		 * @brief Adds global variable to this instance by id.
		 * @param id Variable id to add.
		 * @param desired Sets the desired flag on the variable.
		 */
		void addVariable(id_type id, bool desired = true);
		/**
		 * @brief Adds local or global variables to this instance.
		 * @param ids List of Variable ids.
		 * @param desired Sets the desired flag on the variable.
		 */
		void addVariables(const IdVector& ids, bool desired = true);

		/**
		 * @brief Gets the underlying variable pointer.
		 * @param index List index to get the variable from.
		 * @return nullptr when the index is not valid.
		 */
		[[nodiscard]] Variable* getVariable(const QModelIndex& index) const;

		/**
		 * @brief Overridden from base class to provide assignment of an item when edited.
		 */
		bool setData(const QModelIndex& index, const QVariant& value, int role) override;

		/**
		 * @brief Signals a change of the Variable's property.
		 * @param var Variable concerned.
		 */
		// ReSharper disable once CppFunctionIsNotImplemented
		Q_SIGNAL void changed(const Variable* var);

	private:
		/**
		 * @brief Overridden from #sf::VariableHandler.
		 */
		void variableEventHandler(EEvent event, const Variable& caller, Variable& link, bool same_inst) override;

		/**
	 * @brief Finds the row beloning to the given variable.
	 * @param link Linked variable reference.
	 * @return Row number and -1 when not found.
	 */
		int getRow(Variable& link) const;

		/**
			 * @brief Gets the field displayed in the column.
			 * @param column Position or number of the column
			 * @return Field displayed.
			 */
		EField getField(int column) const;

		/**
		 * @brief Gets the column position or number where the field is displayed.
		 * @param field
		 * @return -1 when field is not displayed.
		 */
		int getColumn(EField field) const;

		/**
		 * @brief Holds the list of variables for this instance.
		 */
		QList<std::shared_ptr<Variable>> _varList;
		/**
		 * @brief The amount of name levels presented in the list for the name column.
		 */
		int _nameLevel;
		/**
		 * @brief Holds the flag weather the row selection is enabled or not.
		 */
		bool _rowCheckBox;
		/**
		 * @brief List of columns being shown.
		 */
		QList<EField> _columns;
};

}// namespace sf
