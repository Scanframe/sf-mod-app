#pragma once

#include "../qt/InformationItemModel.h"
#include "Namespace.h"
#include <QDialog>
#include <QAbstractButton>
#include <QTreeView>
#include <QSettings>
#include <QSortFilterProxyModel>

namespace sf
{

namespace Ui {class InformationSelectDialog;}

class _GII_CLASS InformationSelectDialog :public QDialog
{
	public:

		explicit InformationSelectDialog(QWidget* parent = nullptr);

		InformationTypes::IdVector execute(Gii::SelectionMode mode = Gii::Single, Gii::TypeId idType = Gii::Variable, QSettings* settings = nullptr);

		[[nodiscard]] InformationTypes::IdVector getSelectedIds() const;

	private:

		void childrenExpandCollapse(bool expand, const QModelIndex& index = {});

		void stateSaveRestore(bool save);

		void applyFilter(const QString& filter);

		Ui::InformationSelectDialog* ui;

		QSettings* _settings{nullptr};

		QSortFilterProxyModel* _proxyModel;

		sf::InformationItemModel* _itemModel{nullptr};

		QAction* _actionCollapseAll{nullptr};

		QAction* _actionExpandAll{nullptr};

		Gii::SelectionMode _mode{Gii::Single};

		Gii::IdType _selectedId{0};
};

}