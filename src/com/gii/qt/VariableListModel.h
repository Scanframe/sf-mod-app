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
		 * @brief Creates a #sf::CommonItemDelegate instance for the passed view.
		 */
		void setDelegates(QAbstractItemView* view);

		/**
		 * @brief Called to refresh the attached viewer.
		 */
		void refresh();

		/**
		 * @brief Overridden from base class to provide column display names.
		 */
		[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

		/**
		 * @brief Overridden from base class to provide the amount of rows.
		 */
		[[nodiscard]] int rowCount(const QModelIndex& parent) const override;

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
		 * @brief Adds local or global variable to this instance.
		 * @param var Variable to add.
		 */
		void addVariable(const Variable* var);

		/**
		 * @brief Adds local or global variables to this instance.
		 * @param list List of Variables to.
		 */
		void addVariables(const InformationTypes::Vector& list);

		/**
		 * @brief Gets the underlying variable pointer.
		 * @param index List index to get the variable from.
		 * @return nullptr when the index is not valid.
		 */
		[[nodiscard]] Variable* getByIndex(const QModelIndex& index) const;

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
	 * @brief Finds the row beloning to the passed linked variable.
	 * @param link
	 * @return Row number and -1 when not found.
	 */
		int getRow(Variable& link) const;
		/**
		 * @brief Holds the list of variables for this instance.
		 */
		QList<std::shared_ptr<Variable>> _varList;
		/**
		 * @brief The amount of name levels presented in the list for the name column.
		 */
		int _nameLevels{2};
};

}// namespace sf
