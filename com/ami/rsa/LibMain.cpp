#include <ami/iface/AppModuleInterface.h>
#include "AcquisitionAppModule.h"

namespace sf
{

// Declaration of the dynamic library information.
SF_DL_INFORMATION("Acquisition",
	R"(Repetitive signal acquisition application module.)"
)

// Register this derived class.
SF_REG_CLASS
(
	AppModuleInterface, AppModuleInterface::Parameters, Interface,
	AcquisitionAppModule,
	"Acquisition", "Repetitive signal acquisition application module."
)

}

