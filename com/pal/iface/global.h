/*
This include-file determines the way the classes and functions in the
library are exported when they are used as a dynamic or as application and static library.
When building this Dynamic Library then _PAL_PKG (package) should be defined.

_list of the declaration modifiers for types:
	classes:  _PAL_CLASS
	Function: _PAL_FUNC
	Data:     _PAL_DATA

Add compiler definition flags:
	* _PAL_PKG when building a dynamic library (package)
	* _PAL_ARC when including in a compile or using it as an archive.
*/

#pragma once

// Import of defines for this target.
#include <misc/gen/target.h>

// When DL target and the PKG is not used the DL is being build.
#if IS_DL_TARGET && defined(_PAL_PKG)
	#define _PAL_DATA TARGET_EXPORT
	#define _PAL_FUNC TARGET_EXPORT
	#define _PAL_CLASS TARGET_EXPORT
// Is used as an archive so no importing is needed.
#elif defined(_PAL_ARC)
	#define _PAL_DATA
	#define _PAL_FUNC
	#define _PAL_CLASS
// When no flags are defined assume the package is imported.
#else
	#define _PAL_DATA TARGET_IMPORT
	#define _PAL_FUNC TARGET_IMPORT
	#define _PAL_CLASS TARGET_IMPORT
#endif
