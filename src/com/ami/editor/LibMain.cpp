#include <ami/iface/AppModuleInterface.h>
#include "TextEditorAppModule.h"
#include "CodeEditorAppModule.h"

namespace sf
{

// Declaration of the dynamic library information.
SF_DL_INFORMATION("Text/Code Editors",
	R"(Editors MDI for code and text using syntax highlighting.)"
)

// Register this derived class.
SF_REG_CLASS
(
	AppModuleInterface, AppModuleInterface::Parameters, Interface,
	CodeEditorAppModule,
	"CodeEditor", "Syntax highlighted code editor."
)

// Register this derived class.
SF_REG_CLASS
(
	AppModuleInterface, AppModuleInterface::Parameters, Interface,
	TextEditorAppModule,
	"TextEditor", "Simple text editor."
)

}

