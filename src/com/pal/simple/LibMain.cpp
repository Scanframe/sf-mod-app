#include "SimplePalette.h"

namespace sf
{

// Declaration of the dynamic library information.
SF_DL_INFORMATION("Simple Palette", R"(Simple Palette implementation.)")

// Register this derived class.
SF_REG_CLASS
(
	PaletteInterface, PaletteInterface::Parameters, Interface,
	SimplePalette, "Simple", "Simple palette implementation."
)

}