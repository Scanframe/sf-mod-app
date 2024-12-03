#pragma once
#include <QAbstractButton>
#include <QDialog>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <gii/qt/InformationItemModel.h>
#include <gii/qt/Namespace.h>

namespace sf
{

namespace Ui
{
class InformationSelectDialog;
}

class _GII_CLASS InformationSelectDialog
	: public QDialog
{
	public:
		explicit InformationSelectDialog(QWidget* parent = nullptr);

		InformationTypes::IdVector execute(Gii::SelectionMode mode = Gii::Single, Gii::TypeId idType = Gii::Variable, QSettings* settings = nullptr);

		[[nodiscard]] InformationTypes::IdVector getSelectedIds() const;

	protected:
		bool eventFilter(QObject* watched, QEvent* event) override;

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

}// namespace sf