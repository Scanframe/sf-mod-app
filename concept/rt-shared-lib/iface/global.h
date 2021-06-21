/*
This include file determines the the way the classes and functions in the
library are exported when they are used as a dynamic or as application and static library.
When building this Dynamic Library then _IFACE_PKG (package) should be defined.

_list of the declaration modifiers for types:
	classes:        _IFACE_CLASS
	Function calls: _IFACE_FUNC _IFACE_CALL
	_data:           _IFACE_DATA

Add compiler definition flags:
	* _IFACE_PKG when building a dynamic library (package)
	* _IFACE_ARC when including in a compile or using it as an archive.
*/

#pragma once

// Import of defines for this target.
#include "misc/gen/target.h"

// When DL target and the iface PKG is not used the iface DL is being build.
#if IS_DL_TARGET && defined(_IFACE_PKG)
	#define _IFACE_DATA TARGET_EXPORT
	#define _IFACE_FUNC TARGET_EXPORT
	#define _IFACE_CLASS TARGET_EXPORT
// Is used as an archive so no importing is needed.
#elif defined(_IFACE_ARC)
	#define _IFACE_DATA
	#define _IFACE_CLASS
	#define _IFACE_FUNC
// When no flags are defined assume the package is imported.
#else
	#define _IFACE_DATA TARGET_IMPORT
	#define _IFACE_CLASS TARGET_IMPORT
	#define _IFACE_FUNC TARGET_IMPORT
#endif
