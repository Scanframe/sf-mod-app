/*
This include file determines the the way the classes and functions in the
library are exported when they are used as a dynamic or as application and static library.
When building this Dynamic Library then _IPJ_PKG (package) should be defined.

_list of the declaration modifiers for types:
	classes:  _IPJ_CLASS
	Function: _IPJ_FUNC
	_data:     _IPJ_DATA

Add compiler definition flags:
	* _IPJ_PKG when building a dynamic library (package)
	* _IPJ_ARC when including in a compile or using it as an archive.
*/

#pragma once

// Import of defines for this target.
#include <misc/gen/target.h>

// When DL target and the PKG is not used the DL is being build.
#if IS_DL_TARGET && defined(_IPJ_PKG)
	#define _IPJ_DATA TARGET_EXPORT
	#define _IPJ_FUNC TARGET_EXPORT
	#define _IPJ_CLASS TARGET_EXPORT
// Is used as an archive so no importing is needed.
#elif defined(_IPJ_ARC)
	#define _IPJ_DATA
	#define _IPJ_FUNC
	#define _IPJ_CLASS
// When no flags are defined assume the package is imported.
#else
	#define _IPJ_DATA TARGET_IMPORT
	#define _IPJ_FUNC TARGET_IMPORT
	#define _IPJ_CLASS TARGET_IMPORT
#endif
