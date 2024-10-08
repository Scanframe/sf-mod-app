#include "UnitConversionAppModule.h"
#include <ami/iface/AppModuleInterface.h>

namespace sf
{

// Declaration of the dynamic library information.
SF_DL_INFORMATION("unit Conversion", R"(unit conversion server implementation.)")

// Register this derived class.
SF_REG_CLASS(
	AppModuleInterface, AppModuleInterface::Parameters, Interface,
	UnitConversionAppModule,
	"UnitConverter", "Units conversion server."
)

}// namespace sf
