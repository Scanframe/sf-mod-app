#pragma once
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class TreeModel :public QAbstractItemModel
{
	public:
		explicit TreeModel(QObject* parent = nullptr);

		~TreeModel() override;

		[[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

		[[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;

		[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

		[[nodiscard]] QModelIndex index(int row, int column, const QModelIndex& parent) const override;

		[[nodiscard]] QModelIndex parent(const QModelIndex& child) const override;

		[[nodiscard]] int rowCount(const QModelIndex& parent) const override;

		[[nodiscard]] int columnCount(const QModelIndex& parent) const override;

		void update();

	private:

		struct TreeItem
		{
			explicit TreeItem(TreeItem* parent, const QString& name);

			~TreeItem();

			TreeItem* _parentItem;

			QList<TreeItem*> _childItems;

			QString _name;
		};

		TreeItem* _rootItem;

		TreeItem* createPath(const QStringList& namePath);

};
