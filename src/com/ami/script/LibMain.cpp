#include <ami/iface/AppModuleInterface.h>
#include "ScriptAppModule.h"

namespace sf
{

// Declaration of the dynamic library information.
SF_DL_INFORMATION("Scripting",
	R"(Scripting in AMI applications for manipulating GII.)"
)

// Register this derived class.
SF_REG_CLASS
(
	AppModuleInterface, AppModuleInterface::Parameters, Interface,
	ScriptAppModule,
	"Script", "Scripting in AMI application."
)

}

