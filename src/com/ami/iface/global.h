/*
This include-file determines the way the classes and functions in the
library are exported when they are used as a dynamic or as application and static library.
When building this Dynamic Library then _AMI_PKG (package) should be defined.

_list of the declaration modifiers for types:
	classes:  _AMI_CLASS
	Function: _AMI_FUNC
	Data:     _AMI_DATA

Add compiler definition flags:
	* _AMI_PKG when building a dynamic library (package)
	* _AMI_ARC when including compile-time or using it as an archive.
*/

#pragma once

// Import of defines for this target.
#include <misc/gen/target.h>

// When DL target and the PKG is not used the DL is being build.
#if IS_DL_TARGET && defined(_AMI_PKG)
	#define _AMI_DATA TARGET_EXPORT
	#define _AMI_FUNC TARGET_EXPORT
	#define _AMI_CLASS TARGET_EXPORT
// Is used as an archive so no importing is needed.
#elif defined(_AMI_ARC)
	#define _AMI_DATA
	#define _AMI_FUNC
	#define _AMI_CLASS
// When no flags are defined assume the package is imported.
#else
	#define _AMI_DATA TARGET_IMPORT
	#define _AMI_FUNC TARGET_IMPORT
	#define _AMI_CLASS TARGET_IMPORT
#endif
