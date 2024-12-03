#pragma once
#include <cstddef>
#include <ctime>
#include <misc/global.h>
#include <string>

namespace sf
{

/**
 * @brief Gets the thread count from the current process.
 * @return On failure -1 else the actual thread count.
 */
_MISC_FUNC int getThreadCount();

/**
 * @brief Gets the time this application is running.
 *
 * Is used in combination with getTime(false) to get the time since the start of the application.
 */
_MISC_FUNC timespec getTimeRunning();

/**
 * @brief Loads a dynamic library from the given path.
 * @param path Path to the library.
 * @return True when library loading was successful.
 */
_MISC_FUNC bool loadDynamicLibrary(const std::string& path);

}// namespace sf
