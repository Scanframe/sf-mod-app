#pragma once

#include "../global.h"
#include "../gen/gen_utils.h"
#include <cstddef>
#if IS_WIN
	#if IS_GNU
		#include <windows.h>
	#else
		#include <Windows.h>
	#endif
#endif


_MISC_FUNC int strerror_r(int errnum, char* buf, size_t buflen);

namespace sf
{

_MISC_FUNC bool isRunningWine();

}
