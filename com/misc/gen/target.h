/*
Making it easier to build libararies for the various targets and platforms.
Defines these defines with true (1) or false (0):
 * IS_GCC > GNU compiler detected.
 * IS_QT > QT compile target is detected.
 * IS_WIN > Windows compile target is detected.

 * IS_AB_TARGET > An application binary is the current target.
 * IS_DL_TARGET > A dynamic library is the current target.
 * IS_SL_TARGET > A static library is the current target.
*/

#pragma once

// Detect usage of the GCC GNU compiler.
#if defined(__GNUC__)
	#define IS_GNU 1
//#pragma GCC visibility
#else
	#define IS_GNU 0
#endif

// Check when the GNU compiler is active for a Windows target.
#if IS_GNU && defined(WIN32)
	#define IS_WIN 1
#else
	#define IS_WIN 0
#endif

// Determine if the build target a dynamically shared library.
#if defined(TARGET_DYNAMIC_LIB)
	#define IS_DL_TARGET 1
#else
	#define IS_DL_TARGET 0
#endif

// Determine if the build target a dynamically shared library.
#if defined(TARGET_STATIC_LIB)
	#define IS_SL_TARGET 1
#else
	#define IS_SL_TARGET 0
#endif

// CHeck if an application binary is targeted
#if IS_DL_TARGET || IS_SL_TARGET
	#define IS_AB_TARGET 0
#else
	#define IS_AB_TARGET 1
#endif

// Determine if QT is used in the target.
#if defined(TARGET_QT) || defined(QT_VERSION)
	#define IS_QT 1
#else
	#define IS_QT 0
#endif

#define TARGET_EXPORT __attribute__((visibility("default")))
#define TARGET_IMPORT
#define TARGET_HIDDEN  __attribute__((visibility("hidden")))

// Report current targeted result.
#if defined(REPORT_TARGET)
// Report when GNU GCC is used.
	#if IS_GNU
		#pragma message ("GNU compiler")
	#endif
	#if IS_WIN
		#pragma message ("Windows build")
	#endif
// Report the QT is linked.
	#ifdef IS_QT
		#pragma message ("Target: QT")
	#endif
// Report the target is a dynamically library.
	#ifdef IS_DL_TARGET
		#pragma message ("Target: Shared Library")
	#endif
// Report the target is a static library (archive).
	#ifdef IS_SL_TARGET
		#pragma message ("Target: Static Library")
	#endif
#endif // REPORT_TARGET
