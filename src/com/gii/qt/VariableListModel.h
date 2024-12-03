#pragma once
#include <QAbstractItemView>
#include <QAbstractListModel>
#include <gii/gen/Variable.h>
#include <gii/global.h>

namespace sf
{

/**
 * @brief List model for Variables.
 */
class _GII_CLASS VariableListModel
	: public QAbstractListModel
{
		Q_OBJECT

	public:
		/**
		 * @brief Constructor.
		 */
		explicit VariableListModel(QObject* parent = nullptr);

		/**
		 * @brief ???
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
		 * @brief Overridden from base class to provide assignment of an item when edited.
		 */
		bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	Q_SIGNALS:
		/**
		 * @brief Signals a change of the object's property.
		 * @param obj Object concerned.
		 * @param propertyIndex Index of the property
		 * @param dynamic Property is a dynamic one.
		 */
		void changed(const Variable* var);
		/**
		 *
		 * @param lineEdit Line edit created for this property.
		 * @param obj Object concerned.
		 * @param propertyIndex Index of the property
		 * @param dynamic Property is a dynamic one.
		 */
		void addLineEditActions(QLineEdit* line_edit, const Variable* var) const;

	private:
		/**
		 * @brief Holds the list of variables for this instance.
		 */
		QList<std::shared_ptr<Variable>> _vars;

		int _nameLevels{2};
};

}// namespace sf
