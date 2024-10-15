#pragma once

#include "../gen/system.h"
#include "../global.h"
#include <cstddef>
#include <string>
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
 * @brief Returns the file path from the passed DLL name.
 */
_MISC_FUNC std::string getModulePath(const std::string& dll_name);

/**
 * @brief Gets the main thread id.
 */
_MISC_FUNC DWORD getMainThreadId();

}// namespace sf
