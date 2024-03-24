#pragma once

#include "../global.h"
#include <gii/gen/InformationBase.h>
#include "Namespace.h"
#include <QIcon>
#include <QAbstractItemModel>

namespace sf
{

/**
 * @brief Item model for viewing Variables in a tree view.
 */
class _GII_CLASS InformationItemModel :public QAbstractItemModel
{
	public:
		/**
		 * @brief Constructor.
		 */
		explicit InformationItemModel(Gii::SelectionMode mode, Gii::TypeId idType, QObject* parent);

		/**
		 * @brief Overridden destructor.
		 */
		~InformationItemModel() override;

		/**
		 * @brief Overridden from base class to provide column display names.
		 */
		[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

		/**
		 * @brief Overridden from base class to provide the amount of columns.
		 */
		[[nodiscard]] int columnCount(const QModelIndex& parent) const override;

		/**
		 * @brief Overridden from base class to provide the amount of rows.
		 */
		[[nodiscard]] int rowCount(const QModelIndex& parent) const override;

		/**
		 * @brief Overridden from base class to provide the data to present.
		 */
		[[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

		/**
		 * @brief Overridden from base class to provide flags on an item by index.
		 */
		[[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;

		/**
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] QModelIndex index(int row, int column, const QModelIndex& parent) const override;

		/**
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] QModelIndex parent(const QModelIndex& child) const override;

		/**
		 * @brief Toggle the selection of the item at the passed index.
		 */
		void toggleSelection(const QModelIndex& index);

		/**
		 * @brief Gets the status of a folder type entry of the passed index.
		 */
		bool isFolder(const QModelIndex& index);
		/**
		 * @brief Gets the information ID from the passed index.
		 */
		Gii::IdType getId(const QModelIndex& index);

		/**
		 * @brief Toggle the selection of the item at the passed index.
		 */
		[[nodiscard]] InformationTypes::IdVector getSelectedIds() const;

		/**
		 * @brief Updates the list data.
		 */
		void updateList();

	private:

		/**
		 * @brief Entries in the list.
		 */
		struct TreeItem
		{

			explicit TreeItem(TreeItem* parent, QString name);

			~TreeItem();

			/**
			 * @brief TYpe of entry
			 */
			enum DataType
			{
				dtVariable,
				dtResultData,
				dtFolder
			} _type{dtFolder};

			/**
			 * @brief Holds the selected status.
			 */
			bool _selected{false};
			/**
			 * @brief Hold the ID of the variable or result when the type is not dtFolder.
			 */
			Gii::IdType _id{0};
			/**
			 * @brief Holds the display name of the item.
			 */
			QString _name;
			/**
			 * @brief Holds the parent element.
			 */
			TreeItem* _parentItem{nullptr};
			/**
			 * @brief Holds the child elements.
			 */
			QList<TreeItem*> _childItems;
			/**
			 * @brief Gets the name path of the current item.
			 */
			[[nodiscard]] QStringList getNamePath() const;
		};

		/**
		 * @brief Creates the needed folder parents and
		 */
		TreeItem* createPath(const QStringList& namePath);
		/**
		 * @brief Determines the multiple or single selection mode.
		 */
		Gii::SelectionMode _mode;
		/**
		 * @brief Determines the type of id's to be selected.
		 */
		Gii::TypeId _idType;
		/**
		 * @brief Holds the root item
		 */
		TreeItem* _rootItem;

		QList<TreeItem*> _selected;

		QIcon _icons[3];
};

}
