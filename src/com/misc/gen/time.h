#pragma once
#include <misc/global.h>
#include <ostream>

namespace sf
{

/**
 * @brief Compares the 2 'timespec' time structures.
 * Helper function.
 * @return -1, 0, 1 respectively for smaller, equal en larger.
 */
_MISC_FUNC int timespecCompare(const timespec& ts1, const timespec& ts2);

/**
 * @brief Gets the timespec as function return value as clock_gettime() for the current time.
 * @param real_time When true the real time is returned.
 * 	      When false the time is used for timers and un effected by system time changes.
 */
_MISC_FUNC timespec getTime(bool real_time = false);

}// namespace sf