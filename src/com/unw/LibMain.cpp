#include "Backtrace.h"

#include "misc/gen/TClassRegistration.h"

// Declaration of the dynamic library information.
SF_DL_INFORMATION("Unwind", R"(Module using unwind library to install SEGFAULT and exception handler
which prints the call stack to trace the origin of the problem.)")

__attribute__((constructor)) void LibraryInitialize()
{
	sf::backtrace::initialize();
}

__attribute__((destructor)) void LibraryUninitialize()
{
	sf::backtrace::uninitialize();
}
