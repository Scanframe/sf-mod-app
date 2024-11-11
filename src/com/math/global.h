/*
This include-file determines the way the classes and functions in the
library are exported when they are used as a dynamic or as application and static library.
When building this Dynamic Library then _MATH_PKG (package) should be defined.

_list of the declaration modifiers for types:
	classes:  _MATH_CLASS
	Function: _MATH_FUNC
	Data:     _MATH_DATA

Add compiler definition flags:
	* _GII_PKG when building a dynamic library (package)
	* _GII_ARC when including in a compile or using it as an archive.
*/

#pragma once

// Import of defines for this target.
#include <misc/gen/target.h>

// When DL target and the gii PKG is not used the gii DL is being build.
#if IS_DL_TARGET && defined(_MATH_PKG)
	#define _MATH_DATA TARGET_EXPORT
	#define _MATH_FUNC TARGET_EXPORT
	#define _MATH_CLASS TARGET_EXPORT
// Is used as an archive so no importing is needed.
#elif defined(_MATH_ARC)
	#define _MATH_DATA
	#define _MATH_FUNC
	#define _MATH_CLASS
// When no flags are defined assume the package is imported.
#else
	#define _MATH_DATA TARGET_IMPORT
	#define _MATH_FUNC TARGET_IMPORT
	#define _MATH_CLASS TARGET_IMPORT
#endif
