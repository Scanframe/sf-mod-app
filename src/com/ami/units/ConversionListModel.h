#pragma once

#include <QAbstractListModel>
#include <QAbstractItemView>
#include <gii/gen/UnitConversionServerEx.h>

namespace sf
{

/**
 * @brief List model for manipulating the passed script manager.
 */
class ConversionListModel :public QAbstractListModel
{
	public:
		/**
		 * @brief Constructor.
		 */
		explicit ConversionListModel(UnitConversionServerEx* ucs, QObject* parent = nullptr);

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
		 * @brief Removes entry with the passed index row.
		 */
		bool remove(QModelIndex index);

		/**
		 * @brief Gets the modification flag.
		 */
		[[nodiscard]] bool isDirty() const {return _dirty;};

		/**
		 * @brief Saves changes to the unit server and resets the list's dirty flag.
		 */
		void sync();

	private:
		/**
		 * @brief Holds the interpreter derived class.
		 */
		UnitConversionServerEx* _ucs{nullptr};

		/**
		 * @brief Entry in to the temporary list of followers.
		 */
		struct Entry
		{
			QString _option;
			QString _from_unit;
			int _from_precision{0};
			double _multiplier{1};
			double _offset{0};
			QString _to_unit;
			int _to_precision{0};
		};

		class Adapter;

		/**
		 * @brief Updates the temporary list.
		 */
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