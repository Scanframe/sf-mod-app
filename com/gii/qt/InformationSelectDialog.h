#pragma once

#include <QDialog>
#include <qt/InformationItemModel.h>
#include <QAbstractButton>
#include <QTreeView>
#include <QSettings>
#include "Namespace.h"

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

		Ui::InformationSelectDialog* ui;

		QSettings* _settings{nullptr};

		sf::InformationItemModel* _itemModel{nullptr};

		QAction* _actionCollapseAll{nullptr};

		QAction* _actionExpandAll{nullptr};

		Gii::SelectionMode _mode{Gii::Single};

		Gii::IdType _selectedId{0};
};

}