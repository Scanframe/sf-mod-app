#pragma once

#include "../gen/system.h"
#include "../global.h"
#include <cstddef>
#if IS_WIN
	#if IS_GNU
		#include <windows.h>
	#else
		#include <Windows.h>
	#endif
#endif

namespace sf
{

/**
 * @brief Determines if the application is running using Wine.
 */
_MISC_FUNC bool isRunningWine();

/**
 * @brief Gets the main thread id.
 */
_MISC_FUNC DWORD getMainThreadId();

}// namespace sf
