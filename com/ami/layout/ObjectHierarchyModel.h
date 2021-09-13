#pragma once

#include <QAbstractItemModel>
#include <QIcon>
#include <gen/InformationBase.h>

namespace sf
{

/**
 * @brief Item model for viewing Variables in a tree view.
 */
class ObjectHierarchyModel :public QAbstractItemModel
{
	public:
		/**
		 * @brief Constructor.
		 */
		explicit ObjectHierarchyModel(bool multi, QObject* parent);

		/**
		 * @brief Overridden destructor.
		 */
		~ObjectHierarchyModel() override;

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
		 * @brief Gets the current item from the tree.
		 *
		 * @return When no current item nullptr.
		 */
		[[nodiscard]] QObject* getObject(QModelIndex index) const;

		/**
		 * @brief Updates the list data using the passed object as root.
		 */
		void updateList(QObject* root);

		/**
		 * @brief Inserts an object at the index
		 *
		 * @param parent Index of the parent in the tree.
		 * @param obj New child object of the parent indicated by the index.
		 * @return True wen parentIndex indeed points to the objects parent object.
		 */
		bool insertItemAt(const QModelIndex& parent, QObject* obj);

		/**
		 * @brief removes item at passed index.
		 */
		void removeItem(const QModelIndex& index);

		/**
		 * @brief Determines if this is the root item.
		 */
		bool isRoot(QModelIndex index) const;

		/**
		 * @brief Gets the model-index from the passed object.
		 */
		QModelIndex getObjectIndex(QObject* obj);

	private:

		/**
		 * @brief Entries in the list.
		 */
		struct TreeItem
		{

			/**
			 * @brief Constructor.
			 */
			explicit TreeItem(ObjectHierarchyModel* owner, TreeItem* parent, QObject* obj);

			/**
			 * @brief Destructor.
			 */
			~TreeItem();

			/**
			 * @brief Gets the row number of this item in the parent.
			 * @return -1 When there is no  parent (only when this is root).
			 */
			int getRow();

			/**
			 * @brief Holds the selected status.
			 */
			bool _selected{false};

			/**
			 * @brief holds the owner of the item.
			 */
			ObjectHierarchyModel* _owner{nullptr};
			/**
			 * @brief Holds the object to get the type from.
			 */
			QObject* _object{nullptr};
			/**
			 * @brief Holds the parent element.
			 */
			TreeItem* _parentItem{nullptr};
			/**
			 * @brief Holds the child elements.
			 */
			QList<TreeItem*> _childItems;
		};

		/**
		 * @brief Adds a child into the tree for a parent referencing an object.
		 *
		 * @param obj Referenced object.
		 * @param parent Parent item in the tree.
		 */
		void addChild(QObject* obj, TreeItem* parent);

		/**
		 * @brief Determines the multiple or single selection mode.
		 */
		bool _multi;
		/**
		 * @brief Holds the root item
		 */
		TreeItem* _rootItem;
		/**
		 * @brief Holds all items for lookup purposes.
		 */
		QList<TreeItem*> _items;
		/**
		 * @brief Holds the items selected in multi mode.
		 */
		QList<TreeItem*> _selected;

		QIcon _iconForm;
		QIcon _iconContainer;
		QIcon _iconWidget;

};

}
