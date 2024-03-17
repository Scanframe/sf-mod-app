#include <ami/iface/AppModuleInterface.h>
#include "UnitConversionAppModule.h"

namespace sf
{

// Declaration of the dynamic library information.
SF_DL_INFORMATION("Unit Conversion",
	R"(Unit conversion server implementation.)"
)

// Register this derived class.
SF_REG_CLASS
(
	AppModuleInterface, AppModuleInterface::Parameters, Interface,
	UnitConversionAppModule,
	"UnitConverter", "Units conversion server."
)

}

