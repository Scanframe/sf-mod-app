#include <ami/iface/AppModuleInterface.h>
#include "InformationServiceAppModule.h"

namespace sf
{

// Declaration of the dynamic library information.
SF_DL_INFORMATION("Information Service",
	R"(Information service implementation.)"
)

// Register this derived class.
SF_REG_CLASS
(
	AppModuleInterface, AppModuleInterface::Parameters, Interface,
	InformationServiceAppModule,
	"InformationService", "Information service."
)

}

