/*
This include file determines the the way the classes and functions in the
library are exported when they are used as a dynamic or as application and static library.
When building this Dynamic Library then _RSA_PKG (package) should be defined.

_list of the declaration modifiers for types:
	classes:  _RSA_CLASS
	Function: _RSA_FUNC
	_data:     _RSA_DATA

Add compiler definition flags:
	* _RSA_PKG when building a dynamic library (package)
	* _RSA_ARC when including in a compile or using it as an archive.
*/

#pragma once

// Import of defines for this target.
#include <misc/gen/target.h>

// When DL target and the PKG is not used the DL is being build.
#if IS_DL_TARGET && defined(_RSA_PKG)
	#define _RSA_DATA TARGET_EXPORT
	#define _RSA_FUNC TARGET_EXPORT
	#define _RSA_CLASS TARGET_EXPORT
// Is used as an archive so no importing is needed.
#elif defined(_RSA_ARC)
	#define _RSA_DATA
	#define _RSA_FUNC
	#define _RSA_CLASS
// When no flags are defined assume the package is imported.
#else
	#define _RSA_DATA TARGET_IMPORT
	#define _RSA_FUNC TARGET_IMPORT
	#define _RSA_CLASS TARGET_IMPORT
#endif
