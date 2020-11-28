/*
This include file determines the the way the classes and functions in the
library are exported when they are used as a dynamic or as application and static library.
When building this Dynamic Library then _MISC_PKG (package) should be defined.

List of the declaration modifiers for types:
	classes:  _MISC_CLASS
	Function: _MISC_FUNC
	Data:     _MISC_DATA

Add compiler definition flags:
	* _MISC_PKG when building a dynamic library (package)
	* _MISC_ARC when including in a compile or using it as an archive.
*/

#ifndef MISC_GLOBAL_H
#define MISC_GLOBAL_H

// Import of defines for this target.
#include "target.h"

// When DL target and the misc PKG is not used the misc DL is being build.
#if IS_DL_TARGET && defined(_MISC_PKG)
	#define _MISC_DATA TARGET_EXPORT
	#define _MISC_FUNC TARGET_EXPORT
	#define _MISC_CLASS TARGET_EXPORT
// Is used as an archive so no importing is needed.
#elif defined(_MISC_ARC)
#define _MISC_DATA
	#define _MISC_FUNC
	#define _MISC_CLASS
// When no flags are defined assume the package is imported.
#else
	#define _MISC_DATA TARGET_IMPORT
	#define _MISC_FUNC TARGET_IMPORT
	#define _MISC_CLASS TARGET_IMPORT
#endif

#endif // MISC_GLOBAL_H
