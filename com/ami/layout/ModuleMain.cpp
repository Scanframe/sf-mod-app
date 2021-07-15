#include <ami/iface/AppModuleInterface.h>
#include "LayoutEditorAppModule.h"

namespace sf
{

// Declaration of the dynamic library information.
SF_DL_INFORMATION("UI Layout Editor",
	R"(Editor for UI layout files.)"
)

// Register this derived class.
SF_REG_CLASS
(
	AppModuleInterface, AppModuleInterface::Parameters, Interface,
	LayoutEditorAppModule,
	"LayoutEditor", "Editor for UI layout files."
)

}

