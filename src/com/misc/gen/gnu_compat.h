#pragma once

#include "target.h"
#include <cstring>

#ifdef __cplusplus
	#define SF_CONSTRUCTOR_COMPAT_FUNC(f) \
		static void f(void); \
		struct f##_t_ { f##_t_(void) { f(); } }; static f##_t_ f##_; \
		static void f(void)
#elif defined(_MSC_VER)
#pragma section(".CRT$XCU",read)
	#define INITIALIZER2_(f,p) \
		static void f(void); \
		__declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f; \
		__pragma(comment(linker,"/include:" p #f "_")) \
		static void f(void)
	#ifdef _WIN64
		#define INITIALIZER(f) INITIALIZER2_(f,"")
	#else
		#define INITIALIZER(f) INITIALIZER2_(f,"_")
	#endif
#else
	#define INITIALIZER(f) \
		__attribute__((constructor)) void f(void)
#endif

#if IS_WIN

// Create ecvt_r() not present in when building for Windows.
inline
int ecvt_r(double value, int ndigit, int* decpt, int* sign, char* buf, size_t len)
{
	return _ecvt_s(buf, len, value, ndigit, decpt, sign);
}

inline
int strerror_r(int errnum, char* buf, size_t buflen)
{
	return strerror_s(buf, buflen, errnum);
}

#endif
