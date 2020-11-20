/*
Making it easier to build libararies for the various targets and platforms.
Defines:
	* IS_GCC > GNU compiler detected.
	* IS_QT > QT comile targtet is detected.

*/

#ifndef MISC_TARGET_H
#define MISC_TARGET_H

// Detect usage of the GCC GNU compiler.
#if defined(__GNUC__)
	#define IS_GNU 1
#else
	#define IS_GNU 0
#endif

// Determine if the build is for a dynamic shared library.
#if defined(TARGET_SHARED)
	#define IS_DL_TARGET 1
#else
	#define IS_DL_TARGET 0
#endif

// Determine if QT is used in the target.
#if defined(TARGET_QT) || defined(QT_VERSION)
	#define IS_QT_TARGET 1
#else
	#define IS_QT_TARGET 0
#endif

// Report current targetted result.
#if defined(REPORT_TARGET)
	// Report when GNU GCC is used.
	#if IS_GNU
		#pragma message ("GNU compiler")
	#endif
	// Report the QT is linked.
	#ifdef IS_QT_TARGET
		#pragma message ("Target QT")
	#endif
// Report the QT is linked.
#ifdef IS_SL_TARGET
	#pragma message ("Target Shared Library")
#endif
#endif // REPORT_TARGET

#endif // MISC_TARGET_H
