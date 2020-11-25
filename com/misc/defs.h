/*
This include file determines the the way the classes and functions in the
library are exported when they are used as a dynamic or as application and static library.
When using this Dynamic Library then _MISC_PK (package) should be defined.

List of the declaration modifiers for types:
	classes:        _MISC_CLASS
	Function calls: _MISC_FUNC _MISC_CALL
	Data:           _MISC_DATA

Add compiler definition flag:
	* _MISC_PKG when using as dynamic library (package)
	* _MISC_ARC when including in a compile or using it as an archive.
*/

#ifndef MISC_DEFS_H
#define MISC_DEFS_H

// Import of defines for this target.
#include "target.h"

//#if IS_QT
//#include <QtCore/qglobal.h>
//#endif

// When DL target and the misc PKG is not used the misc DL is being build
#if IS_DL_TARGET && !defined(_MISC_PKG) && !defined(_MISC_ARC)
	#define _MISC_DATA TARGET_EXPORT
	#define _MISC_CALL TARGET_EXPORT
	#define _MISC_CLASS TARGET_EXPORT
	#define _MISC_FUNC

// Is used dynamically and importing is required.
#elif defined(_MISC_PKG)
	#define _MISC_DATA TARGET_IMPORT
	#define _MISC_CALL TARGET_IMPORT
	#define _MISC_CLASS TARGET_IMPORT
	#define _MISC_FUNC

// When used statically.
#else
	#define _MISC_DATA
	#define _MISC_CALL
	#define _MISC_CLASS
	#define _MISC_FUNC
#endif

#endif // MISC_DEFS_H
