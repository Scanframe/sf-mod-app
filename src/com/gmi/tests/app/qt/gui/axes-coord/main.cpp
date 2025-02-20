#include "misc/qt/ApplicationSettings.h"

#include <QApplication>
#include <QDir>
#include <gmi/iface/qt/AxesCoordEdit.h>
#include <misc/gen/dbgutils.h>
#include <misc/qt/FormDialog.h>
#include <misc/qt/Globals.h>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	// Ignore desktop settings because it gives the wrong icon colors.
	QApplication::setDesktopSettingsAware(false);
	// Initialize base using the application file path.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(fi.absolutePath() + QDir::separator() + "config", fi.completeBaseName() + ".ini");
	// Create instance to handle settings.
	sf::ApplicationSettings settings;
	// Set the file path to the settings instance and make it watch changes.
	settings.setFilepath(fi.absoluteFilePath(), true);
	// Set the plugin/module directory. For now not configurable.
	sf::setPluginDir(QCoreApplication::applicationDirPath() + QDir::separator() + "lib");
	if (argc <= 1)
	{
		QDialog dlg;
		settings.restoreWindowRect("Dialog", &dlg);
		auto ac = new sf::AxesCoordEdit(&dlg);
		dlg.exec();
		settings.saveWindowRect("Dialog", &dlg);
	}
	else
	{
		// Set the file location to the resource.
		sf::FormDialog dlg;
		dlg.Load(QFile(":/ui/dialog"));
		dlg.exec();
	}//
	return 0;
}
