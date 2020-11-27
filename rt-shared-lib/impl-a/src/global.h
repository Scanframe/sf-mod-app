/*
This include file determines the the way the classes and functions in the
library are exported when they are used as a dynamic or as application and static library.
When building this Dynamic Library then _IMPL_A_PKG (package) should be defined.

List of the declaration modifiers for types:
	classes:        _IMPL_A_CLASS
	Function calls: _IMPL_A_FUNC _IMPL_A_CALL
	Data:           _IMPL_A_DATA

Add compiler definition flags:
	* _IMPL_A_PKG when building a dynamic library (package)
	* _IMPL_A_ARC when including in a compile or using it as an archive.
*/

#ifndef IMPL_A_GLOBAL_H
#define IMPL_A_GLOBAL_H

// Import of defines for this target.
#include "com/misc/target.h"

// When DL target and the iface PKG is not used the iface DL is being build.
#if IS_DL_TARGET && defined(_IMPL_A_PKG)
	#define _IMPL_A_DATA TARGET_EXPORT
	#define _IMPL_A_FUNC TARGET_EXPORT
	#define _IMPL_A_CLASS TARGET_EXPORT
// Is used as an archive so no importing is needed.
#elif defined(_IMPL_A_ARC)
	#define _IMPL_A_DATA
	#define _IMPL_A_CLASS
	#define _IMPL_A_FUNC
// When no flags are defined assume the package is imported.
#else
	#define _IMPL_A_DATA TARGET_IMPORT
	#define _IMPL_A_FUNC
	#define _IMPL_A_CLASS TARGET_IMPORT
#endif

#endif // IMPL_A_GLOBAL_H
