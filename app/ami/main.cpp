#include <QTimer>
#include "Application.h"

using namespace sf;

int main(int argc, char* argv[])
{
	Q_INIT_RESOURCE(resource);
	sf::Application app(argc, argv);
	sf::Application::setOrganizationName("ScanFrame");
	sf::Application::setApplicationName("Modular Application");
	sf::Application::setApplicationDisplayName("Modular Base Application");
	sf::Application::setApplicationVersion(QT_VERSION_STR);
	// This call exists the application when
	app.parseCommandline();
	app.initialize(AppModuleInterface::Initialize);
	app.getMainWindow()->show();
	app.initialize(AppModuleInterface::Finalize);
	app.processCommandLine();
	auto rv = sf::Application::exec();
	app.initialize(AppModuleInterface::Uninitialize);
	return rv;
}
