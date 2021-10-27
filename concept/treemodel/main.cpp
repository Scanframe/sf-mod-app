#include <QApplication>
#include <QDialog>
#include <QTreeView>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QSortFilterProxyModel>
#include "treemodel.h"


void doChildrenExpand(QTreeView* tv, const QModelIndex& index = {}) // NOLINT(misc-no-recursion)
{
	if (!index.isValid())
	{
		return;
	}
	if (!tv->isExpanded(index))
	{
		tv->expand(index);
	}
	int childCount = index.model()->rowCount(index);
	for (int i = 0; i < childCount; i++)
	{
		doChildrenExpand(tv, tv->model()->index(i, 0, index));
	}
}

void childrenExpand(QTreeView* tv)
{
	int count = tv->model()->rowCount();
	for (int i = 0; i < count; i++)
	{
		doChildrenExpand(tv, tv->model()->index(i, 0));
	}
}

int main(int argc, char* argv[])
{
	QApplication::setDesktopSettingsAware(false);
	QApplication app(argc, argv);
	QDialog dlg;
	dlg.setWindowTitle("Tree Model Example");
	dlg.setLayout(new QVBoxLayout(&dlg));
	dlg.resize(300, 500);
	auto le = new QLineEdit;
	dlg.layout()->addWidget(le);
	auto tv = new QTreeView;
	dlg.layout()->addWidget(tv);
	auto model = new TreeModel(tv);
	auto pm = new QSortFilterProxyModel(&dlg);
	pm->setSourceModel(model);
	//pm->setDynamicSortFilter(false);
	pm->setRecursiveFilteringEnabled(true);
	pm->setFilterCaseSensitivity(Qt::CaseInsensitive);
	model->update();
	tv->setModel(pm);
	QDialog::connect(le, &QLineEdit::textChanged, pm, &QSortFilterProxyModel::setFilterFixedString);
	childrenExpand(tv);
	return dlg.exec();
}
