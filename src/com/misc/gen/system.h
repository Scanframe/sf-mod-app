#pragma once

#include "../global.h"
#include <cstddef>
#include <ctime>

namespace sf
{

/**
 * @brief Returns the thread count from the current process.
 */
_MISC_FUNC size_t getThreadCount();

/**
 * @brief Gets the time this application is running.
 *
 * Is used in combination with getTime(false) to get the time since the start of the application.
 */
_MISC_FUNC timespec getTimeRunning();

}// namespace sf
