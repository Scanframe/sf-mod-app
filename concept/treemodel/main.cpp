#include <QApplication>
#include <QTreeView>
#include <misc/qt/qt_utils.h>
#include "treemodel.h"

int main(int argc, char* argv[])
{
	QApplication::setDesktopSettingsAware(false);
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName("ScanFrame");
	QCoreApplication::setApplicationName("Shared Library Concept");
	QCoreApplication::setApplicationVersion(QT_VERSION_STR);
	// InitializeBase using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath() + QDir::separator() + "config", fi.completeBaseName() + ".ini");
	// Create instance to handle settings.
	sf::ApplicationSettings settings;
	// Set the file path to the settings instance and make it watch changes.
	settings.setFilepath(fi.absoluteFilePath(), true);
	//
	TreeModel model;
	model.update();
	//
	QTreeView view;
	view.setModel(&model);
	view.setWindowTitle(QObject::tr("Tree Model Example"));
	settings.restoreWindowRect("MainWindow", &view);
	for (int i = 0; i < view.model()->columnCount({}) - 1; i++)
	{
		view.resizeColumnToContents(i);
	}
	view.show();
	auto rv =  QApplication::exec();
	settings.saveWindowRect("MainWindow", &view);
	return rv;
}
