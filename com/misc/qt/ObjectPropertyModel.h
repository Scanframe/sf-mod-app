#pragma once

#include <QAbstractListModel>
#include "../global.h"

namespace sf
{

/**
 * @brief List model for QObject properties.
 */
class _MISC_CLASS ObjectPropertyModel :public QAbstractListModel
{
	Q_OBJECT
	public:
		/**
		 * @brief Constructor.
		 */
		explicit ObjectPropertyModel(QObject* parent = nullptr);

		/**
		 * @brief ???
		 */
		void setTarget(QObject* obj);

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
		void changed(QObject* obj, int propertyIndex, bool dynamic);
		/**
		 *
		 * @param lineEdit Line edit created for this property.
		 * @param obj Object concerned.
		 * @param propertyIndex Index of the property
		 * @param dynamic Property is a dynamic one.
		 */
		void addLineEditActions(QLineEdit* lineEdit, QObject* obj, int propertyIndex, bool dynamic) const;

	private:
		/**
		 * @brief Holds the target object.
		 */
		QObject* _target{nullptr};

		struct Entry
		{
			inline
			Entry(QObject* obj, int index, bool dynamic)
				:_obj(obj), _index(index), _dynamic(dynamic) {};

			QObject* _obj;
			int _index;
			bool _dynamic;
		};

		/**
		 * @brief Holds the indices of the properties.
		 */
		QList<Entry> _indices;
};

}