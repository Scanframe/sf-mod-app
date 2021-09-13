/*
This include file determines the the way the classes and functions in the
library are exported when they are used as a dynamic or as application and static library.
When building this Dynamic Library then _STO_PKG (package) should be defined.

_list of the declaration modifiers for types:
	classes:  _STO_CLASS
	Function: _STO_FUNC
	_data:     _STO_DATA

Add compiler definition flags:
	* _STO_PKG when building a dynamic library (package)
	* _STO_ARC when including in a compile or using it as an archive.
*/

#pragma once

// Import of defines for this target.
#include <misc/gen/target.h>

// When DL target and the PKG is not used the DL is being build.
#if IS_DL_TARGET && defined(_STO_PKG)
	#define _STO_DATA TARGET_EXPORT
	#define _STO_FUNC TARGET_EXPORT
	#define _STO_CLASS TARGET_EXPORT
// Is used as an archive so no importing is needed.
#elif defined(_STO_ARC)
	#define _STO_DATA
	#define _STO_FUNC
	#define _STO_CLASS
// When no flags are defined assume the package is imported.
#else
	#define _STO_DATA TARGET_IMPORT
	#define _STO_FUNC TARGET_IMPORT
	#define _STO_CLASS TARGET_IMPORT
#endif
