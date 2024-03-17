#include <ami/iface/AppModuleInterface.h>
#include "ServiceAppModule.h"

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
	ServiceAppModule,
	"InformationService", "Information service."
)

}

