/*
This include-file determines the way the classes and functions in the
library are exported when they are used as a dynamic or as application and static library.
When building this Dynamic Library then _GMI_PKG (package) should be defined.

_list of the declaration modifiers for types:
	classes:  _GMI_CLASS
	Function: _GMI_FUNC
	Data:     _GMI_DATA

Add compiler definition flags:
	* _PAL_PKG when building a dynamic library (package)
	* _PAL_ARC when including in a compile or using it as an archive.
*/

#pragma once

// Import of defines for this target.
#include <misc/gen/target.h>

// When DL target and the PKG is not used the DL is being build.
#if IS_DL_TARGET && defined(_GMI_PKG)
	#define _GMI_DATA TARGET_EXPORT
	#define _GMI_FUNC TARGET_EXPORT
	#define _GMI_CLASS TARGET_EXPORT
// Is used as an archive so no importing is needed.
#elif defined(_GMI_ARC)
	#define _GMI_DATA
	#define _GMI_FUNC
	#define _GMI_CLASS
// When no flags are defined assume the package is imported.
#else
	#define _GMI_DATA TARGET_IMPORT
	#define _GMI_FUNC TARGET_IMPORT
	#define _GMI_CLASS TARGET_IMPORT
#endif
