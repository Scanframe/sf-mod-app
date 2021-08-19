#pragma once

#include <QAbstractListModel>
#include <QAbstractItemView>
#include "ScriptManager.h"

namespace sf
{

/**
 * @brief List model for manipulating the passed script manager.
 */
class ScriptManagerListModel :public QAbstractListModel
{
	public:
		/**
		 * @brief Constructor.
		 */
		explicit ScriptManagerListModel(ScriptManager* manager, QObject* parent = nullptr);

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
		 * @brief Overridden from base class to provide assignment of an item when edited.
		 */
		bool setData(const QModelIndex& index, const QVariant& value, int role) override;

		/**
		 * @brief Adds a new entry to the end or after specified index of the list.
		 */
		void add(QModelIndex index);

		/**
		 * @brief Removes an entry at specified index.
		 *
		 * @param index Position in the list.
		 */
		void remove(QModelIndex index);

		/**
		 * @brief Compiles and initializes all scripts.
		 */
		void startAll();

		/**
		 * @brief Compiles and initializes the script at the passed index row.
		 */
		void start(QModelIndex index);

		/**
		 * @brief Stops the script from running in the background at the passed index row.
		 */
		void stop(QModelIndex index);

	private:
		/**
		 * @brief Holds the interpreter derived class.
		 */
		ScriptManager* _manager{nullptr};
};

}