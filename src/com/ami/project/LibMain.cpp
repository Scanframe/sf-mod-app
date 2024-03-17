#include <ami/iface/AppModuleInterface.h>
#include "ProjectAppModule.h"

namespace sf
{

// Declaration of the dynamic library information.
SF_DL_INFORMATION("Project",
	R"(Project application module where controllers and acquisition devices can be configured.)"
)

// Register this derived class.
SF_REG_CLASS
(
	AppModuleInterface, AppModuleInterface::Parameters, Interface,
	ProjectAppModule,
	"Project", "Project application module."
)

}

