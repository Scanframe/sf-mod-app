/*
This include-file determines the way the classes and functions in the
library are exported when they are used as a dynamic or as application and static library.
When building this Dynamic Library then _UNW_PKG (package) should be defined.

_list of the declaration modifiers for types:
	classes:  _UNW_CLASS
	Function: _UNW_FUNC
	Data:     _UNW_DATA

Add compiler definition flags:
	* _UNW_PKG when building a dynamic library (package)
	* _UNW_ARC when including in a compile or using it as an archive.
*/

#pragma once

// Import of defines for this target.
#include "../misc/gen/target.h"

// When DL target and the PKG is not used the DL is being build.
#if IS_DL_TARGET && defined(_UNW_PKG)
	#define _UNW_DATA TARGET_EXPORT
	#define _UNW_FUNC TARGET_EXPORT
	#define _UNW_CLASS TARGET_EXPORT
// Is used as an archive so no importing is needed.
#elif defined(_UNW_ARC)
	#define _UNW_DATA
	#define _UNW_FUNC
	#define _UNW_CLASS
// When no flags are defined assume the package is imported.
#else
	#define _UNW_DATA TARGET_IMPORT
	#define _UNW_FUNC TARGET_IMPORT
	#define _UNW_CLASS TARGET_IMPORT
#endif
