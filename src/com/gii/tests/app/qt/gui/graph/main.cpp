#include "misc/qt/ApplicationSettings.h"

#include <QApplication>
#include <QDir>
#include <QTimer>
#include <gii/gen/Variable.h>
#include <misc/gen/dbgutils.h>
#include <test-GraphWindow.h>

int main(int argc, char* argv[])
{
	// Removes the console in Windows application.
	sf::freeConsole();
	QApplication app(argc, argv);
	// Initialize base using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath() + QDir::separator() + "config", fi.completeBaseName() + ".ini");
	// Create instance to handle settings.
	sf::ApplicationSettings settings;
	// Set the file path to the settings instance and make it watch changes.
	settings.setFilepath(fi.absoluteFilePath(), true);
	//
	sf::Variable::initialize();
	//
	QWidget* win{nullptr};
	//
	QTimer::singleShot(0, [&] {
		win = new GraphWindow();
		settings.restoreWindowRect("MainWindow", win);
		win->show();
	});
	//
	auto rv = QCoreApplication::exec();
	//
	settings.saveWindowRect("MainWindow", win);
	//
	sf::delete_null(win);
	//
	sf::Variable::uninitialize();
	//
	return rv;
}
