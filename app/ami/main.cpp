#include "Application.h"
#if IS_WIN
#include <windows.h>
#endif

using namespace sf;

int main(int argc, char* argv[])
{
	Q_INIT_RESOURCE(resource);
	// Initialize the application with some defaults.
	Application::setOrganizationName("Scanframe");
	Application::setApplicationName("Modular Application");
	Application::setApplicationDisplayName("Modular Base Application");
	Application::setApplicationVersion(QT_VERSION_STR);
	Application app(argc, argv);
#if IS_WIN
	if (!IsDebug())
	{
		// Removes the console in windows application.
		FreeConsole();
	}
#endif
	// This call exists the application when
	app.parseCommandline();
	app.initialize(AppModuleInterface::Initialize);
	app.getMainWindow()->show();
	app.initialize(AppModuleInterface::Finalize);
	app.processCommandLine();
	auto rv = Application::exec();
	app.initialize(AppModuleInterface::Uninitialize);
	return rv;
}
