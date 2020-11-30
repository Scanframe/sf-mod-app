#include "rt-impl-a.h"

// Register the same derived class as an alias.
SF_REG_CLASS
(
	RuntimeIface, RuntimeIface::Parameters, Interface,
	RuntimeLibImplementationA, "Class-B", "Alias version of the class."
)

