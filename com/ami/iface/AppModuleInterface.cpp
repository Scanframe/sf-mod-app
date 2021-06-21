#include <QSettings>
#include "AppModuleInterface.h"
#include "ModuleSelectionDialog.h"

namespace sf
{

SF_IMPL_IFACE(AppModuleInterface, AppModuleInterface::Parameters, Interface)

AppModuleInterface::AppModuleInterface(const AppModuleInterface::Parameters& parameters)
	:QObject(parameters._parent)
{
}

AppModuleInterface::~AppModuleInterface() = default;

bool AppModuleInterface::initialize()
{
	return doInitialize(true);
}

bool AppModuleInterface::uninitialize()
{
	return doInitialize(false);
}

void AppModuleInterface::openConfiguration(QSettings& settings, QWidget* parent)
{
	ModuleSelectionDialog::getDialog(&settings, parent)->show();
}

}
