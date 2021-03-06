/*
This include-file determines the way the classes and functions in the
library are exported when they are used as a dynamic or as application and static library.
When building this Dynamic Library then _GSC_PKG (package) should be defined.

_list of the declaration modifiers for types:
	classes:  _GSC_CLASS
	Function: _GSC_FUNC
	Data:     _GSC_DATA

Add compiler definition flags:
	* _GSC_PKG when building a dynamic library (package)
	* _GSC_ARC when including in a compile or using it as an archive.
*/

#pragma once

// Import of defines for this target.
#include <misc/gen/target.h>

// When DL target and the PKG is not used the DL is being build.
#if IS_DL_TARGET && defined(_GSC_PKG)
	#define _GSC_DATA TARGET_EXPORT
	#define _GSC_FUNC TARGET_EXPORT
	#define _GSC_CLASS TARGET_EXPORT
// Is used as an archive so no importing is needed.
#elif defined(_GSC_ARC)
	#define _GSC_DATA
	#define _GSC_FUNC
	#define _GSC_CLASS
// When no flags are defined assume the package is imported.
#else
	#define _GSC_DATA TARGET_IMPORT
	#define _GSC_FUNC TARGET_IMPORT
	#define _GSC_CLASS TARGET_IMPORT
#endif
