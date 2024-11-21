#include "GmiEmulator.h"

namespace sf
{

// Declaration of the dynamic library information.
SF_DL_INFORMATION("Motion Controller Emulator", R"(Emulator of a motion controller device.)")

// Register this derived class.
SF_REG_CLASS(
	gmi::Controller, gmi::Controller::Parameters, Interface,
	GmiEmulator,
	"emulator", "Editor for UI layout files."
)

}// namespace sf
