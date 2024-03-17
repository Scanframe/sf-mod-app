#pragma once

#include <QAbstractListModel>
#include <QAbstractItemView>
#include <gii/gen/UnitConversionServerEx.h>

namespace sf
{

/**
 * @brief List model for manipulating the passed script manager.
 */
class FollowersListModel :public QAbstractListModel
{
	public:
		/**
		 * @brief Constructor.
		 */
		explicit FollowersListModel(UnitConversionServerEx* ucs, QObject* parent = nullptr);

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

		bool remove(QModelIndex index);

		/**
		 * @brief Edits a entry depending on the validity of the passed index.
		 *
		 * @param index Invalid when adding and Valid when editing.
		 */
		bool edit(QModelIndex index);

		/**
		 * @brief Adds a entry depending on the validity of the passed index.
		 */
		bool add();

		/**
		 * @brief Gets the modification flag.
		 */
		[[nodiscard]] bool isDirty() const {return _dirty;};

		/**
		 * @brief Saves changes to the unit server and resets the list's dirty flag.
		 */
		void sync();

		/**
		 * @brief Entry in to the temporary list of followers.
		 */
		struct Entry
		{
			Variable::id_type _masterId{0};
			QString _formula;
			QString _unit;
			QList<Variable::id_type> _followerIds;
		};

	private:
		/**
		 * @brief Holds the interpreter derived class.
		 */
		UnitConversionServerEx* _ucs{nullptr};

		void update();

		/**
		 * @brief Holds a copy of the profile section.
		 */
		QList<Entry> _list;

		/**
		 * Dirty flag on modification.
		 */
		bool _dirty{false};
};

}