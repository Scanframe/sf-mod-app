/*

Copyright (c) ScanFrame 20.
All Rights Reserved.

This include file determines the the way the classes and functions in the
basics library are exported when they are used as a DLL with an import library.
When using the basics DLL in an other DLL or EXE then
_MISCDLL should be defined to prevend errors.

Usage of the type modifiers:

	classes:   _MISC_CLASS
	functions: _MISC_FUNC
	data:      _MISC_DATA

*/

#ifndef MISC_DEFS_H
#define MISC_DEFS_H

// Import of IS_xxx_TARGET defines.
#include "target.h"

// When compiling a dynamic shared library and not using the misc shared lib it is build.
#if IS_DL_TARGET && !defined(_MISC_DL) && !defined(_MISC_LIB)
	#if IS_GNU
		#define _MISC_CLASS __declspec(dllexport)
		#define _MISC_FUNC __declspec(dllexport)
		#define _MISC_CALL
		#define _MISC_DATA __declspec(dllexport)
		#define _MISC_MEM
	#else
		#define _MISC_CLASS
		#define _MISC_FUNC
		#define _MISC_CALL
		#define _MISC_DATA
		#define _MISC_MEM
	#endif
#else
	#ifdef _MISCDLL
		#define _MISC_CLASS __import
		#define _MISC_FUNC
		#define _MISC_CALL __import
		#define _MISC_DATA __import
		#define _MISC_MEM __import
	#else
		#define _MISC_CLASS
		#define _MISC_FUNC
		#define _MISC_CALL
		#define _MISC_DATA
		#define _MISC_MEM
	#endif
#endif

#endif // MISC_DEFS_H
