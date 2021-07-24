#include <QApplication>
#include <QTableWidget>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QTreeView>
#include <misc/qt/qt_utils.h>
#include "SpinBoxDelegate.h"
#include "stardelegate.h"
#include "stareditor.h"
#include "starrating.h"
#include "ComboBoxDelegate.h"

void populateTableWidget(QTableWidget* tableWidget)
{
	static constexpr struct
	{
		const char* title;
		const char* genre;
		const char* artist;
		int rating;
	} staticData[] = {
		{"Mass in B-Minor", "Baroque", "J.S. Bach", 5},
		{"Three More Foxes", "Jazz", "Maynard Ferguson", 4},
		{"Sex Bomb", "Pop", "Tom Jones", 3},
		{"Barbie Girl", "Pop", "Aqua", 5},
		{nullptr, nullptr, nullptr, 0}

	};

	for (int row = 0; staticData[row].title != nullptr; ++row)
	{
		auto item0 = new QTableWidgetItem(staticData[row].title);
		auto item1 = new QTableWidgetItem(staticData[row].genre);
		auto item2 = new QTableWidgetItem(staticData[row].artist);
		auto item3 = new QTableWidgetItem;
		item3->setData(0,
			QVariant::fromValue(StarRating(staticData[row].rating)));

		tableWidget->setItem(row, 0, item0);
		tableWidget->setItem(row, 1, item1);
		tableWidget->setItem(row, 2, item2);
		tableWidget->setItem(row, 3, item3);
	}
}

QWidget* SpinBox(QWidget* parent = nullptr)
{
	auto tv = new QTreeView(parent);
	auto model = new QStandardItemModel(4, 2, tv);
	auto spin = new SpinBoxDelegate(tv);
	auto combo = new ComboBoxDelegate(tv);
	//
	tv->setWindowTitle(QObject::tr("Spin Box Delegate"));
	//
	tv->setModel(model);
	tv->setItemDelegateForColumn(0, spin);
	tv->setItemDelegateForColumn(1, combo);
	tv->header()->setStretchLastSection(true);
	//
	model->setHorizontalHeaderLabels({"First", "Second"});
	for (int row = 0; row < 8; ++row)
	{
		for (int column = 0; column < 2; ++column)
		{
//			auto index = model->index(row, column, QModelIndex());
//			model->setData(index, QVariant((row + 1) * (column + 1)));
			auto item= new QStandardItem();
			item->setTextAlignment(column ? Qt::AlignCenter : Qt::AlignRight);
			item->setBackground(column ? QColorConstants::Yellow : QColorConstants::Green);
			item->setText(QVariant((row + 1) * (column + 1)).toString());
			model->setItem(row, column, item);
		}
	}
	return tv;
}

QWidget* StarTable(QWidget* parent = nullptr)
{
	auto tw = new QTableWidget(4, 4, parent);
	tw->setItemDelegate(new StarDelegate);
	tw->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
	tw->setSelectionBehavior(QAbstractItemView::SelectRows);
	tw->setHorizontalHeaderLabels({"Title", "Genre", "Artist", "Rating"});

	populateTableWidget(tw);
	tw->resizeColumnsToContents();
	tw->resize(500, 300);
	return tw;
}

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	QApplication::setOrganizationName("ScanFrame");
	QApplication::setApplicationName("Delegates");
	QApplication::setApplicationVersion(QT_VERSION_STR);
	// InitializeBase using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath(), fi.completeBaseName() + ".ini");
	// Create instance to handle settings.
	sf::ApplicationSettings settings;
	// Set the file path to the settings instance and make it watch changes.
	settings.setFilepath(fi.absoluteFilePath(), true);
	//
	auto win = SpinBox();
	//auto win = StarTable();
	//
	win->setWindowIcon(QIcon(":logo/ico/scanframe"));
	//
	settings.restoreWindowRect("MainWindow", win);
	//
	win->show();
	//
	auto exit_code = QApplication::exec();
	//
	settings.saveWindowRect("MainWindow", win);
	//
	delete win;
	//
	return exit_code;
}

