#include "AmplitudePalette.h"

namespace sf
{

// Declaration of the dynamic library information.
SF_DL_INFORMATION("Amplitude Palette", R"(Amplitude Palette implementation.)")

// Register this derived class.
SF_REG_CLASS
(
	PaletteInterface, PaletteInterface::Parameters, Interface,
	AmplitudePalette, "Amplitude", "Amplitude palette implementation."
)

}