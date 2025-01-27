#pragma once

#include <misc/global.h>
#include <string>

namespace sf
{

/**
 * @brief Gets the GNU compiler version.
 */
inline std::string getGCCVersion()
{
#ifdef __GNUC__
	return std::to_string(__GNUC__) + '.' + std::to_string(__GNUC_MINOR__) + '.' + std::to_string(__GNUC_PATCHLEVEL__);
#else
	return "?.?.?";
#endif
}

/**
 * @brief Gets the C++ standard used.
 */
inline std::string_view getCppStandard()
{
#if __cplusplus == 199711L
	return "C++98/03";
#elif __cplusplus == 201103L
	return "C++11";
#elif __cplusplus == 201402L
	return "C++14";
#elif __cplusplus == 201703L
	return "C++17";
#elif __cplusplus == 202002L
	return "C++20";
#elif __cplusplus == 202302L
	return "C++23";
#else
	return "Unknown C++ standard";
#endif
}

/**
 * @brief Gets the OS and version used.
 */
_MISC_FUNC std::string getOSVersion();

/**
 * @brief Gets the CPU architecture returned from 'uname -m' command.
 */
_MISC_FUNC std::string_view getCpuArchitecture();

}// namespace sf
