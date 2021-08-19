#include <QAbstractListModel>
#include "../gen/ScriptInterpreter.h"

#pragma once

namespace sf
{

/**
 * @brief Provides a model list for instructions and variables of a script.
 */
class _MISC_CLASS ScriptModelList :public QAbstractListModel
{
	public:
		/**
		 * @brief Constructor.
		 */
		explicit ScriptModelList(QObject* parent = nullptr);

		/**
		 * @brief Modes available for this model.
		 */
		enum EMode
		{
			mInstructions,
			mVariables
		};

		/**
		 * @brief Set the interpreter and mode for the model.
		 */
		void setInterpreter(ScriptInterpreter* interpreter, EMode mode);

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
		 * @brief Overridden from base class to provide assignment of an item when edited.
		 */
		bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	private:
		/**
		 * @brief Holds the interpreter derived class.
		 */
		ScriptInterpreter* _interpreter{nullptr};
		/**
		 * @brief Holds the mode of the model on what to display.
		 */
		EMode _mode{mInstructions};
};

}
