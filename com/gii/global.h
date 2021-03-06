/*
This include file determines the the way the classes and functions in the
library are exported when they are used as a dynamic or as application and static library.
When building this Dynamic Library then _GII_PKG (package) should be defined.

List of the declaration modifiers for types:
	classes:  _GII_CLASS
	Function: _GII_FUNC
	_data:    _GII_DATA

Add compiler definition flags:
	* _GII_PKG when building a dynamic library (package)
	* _GII_ARC when including in a compile or using it as an archive.
*/

#pragma once

// Import of defines for this target.
#include "misc/target.h"

// When DL target and the gii PKG is not used the gii DL is being build.
#if IS_DL_TARGET && defined(_GII_PKG)
	#define _GII_DATA TARGET_EXPORT
	#define _GII_FUNC TARGET_EXPORT
	#define _GII_CLASS TARGET_EXPORT
// Is used as an archive so no importing is needed.
#elif defined(_GII_ARC)
#define _GII_DATA
	#define _GII_FUNC
	#define _GII_CLASS
// When no flags are defined assume the package is imported.
#else
	#define _GII_DATA TARGET_IMPORT
	#define _GII_FUNC TARGET_IMPORT
	#define _GII_CLASS TARGET_IMPORT
#endif
