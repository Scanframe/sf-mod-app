#include "RsaEmulator.h"

namespace sf
{

// Declaration of the dynamic library information.
SF_DL_INFORMATION("RSA Emulator", R"(Repetitive Signal Acquisition Emulator.
This acquisition module generates a signal which is analyzed as an ultrasonic device.)")

// Register this derived class.
SF_REG_CLASS
(
	RsaInterface, RsaInterface::Parameters, Interface,
	AcquisitionEmulator, "UT Emulator", "Repetitive signal acquisition UT emulator."
)

}