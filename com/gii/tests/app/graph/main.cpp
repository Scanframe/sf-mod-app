#include <QApplication>
#include <QTimer>
#include <QDir>
#include <misc/qt/qt_utils.h>
#include <gen/Variable.h>
#include <test-GraphWindow.h>
#if IS_WIN
#include <windows.h>
#endif

int main(int argc, char* argv[])
{
#if IS_WIN
	if (!sf::IsDebug())
	{
		// Removes the console in windows application.
		FreeConsole();
	}
#endif
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
	QTimer::singleShot(0, [&]
	{
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
	sf::Variable::deinitialize();
	//
	return rv;
}

