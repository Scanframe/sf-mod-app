#include "version.h"
#include <string>
#if IS_WIN
	#include "../win/win_utils.h"
#else
	#include <sys/utsname.h>
#endif

namespace sf
{
#if defined(_WIN32) || defined(_WIN64)
std::string getOSVersion()
{
	OSVERSIONINFO versionInfo = {sizeof(OSVERSIONINFO)};
	if (GetVersionEx(&versionInfo))
	{
		return std::string(isRunningWine() ? "Wine " : "Windows ") + std::to_string(versionInfo.dwMajorVersion) + "." + std::to_string(versionInfo.dwMinorVersion);
	}
	return "Unknown Windows Version";
}
#elif defined(__APPLE__) || defined(__MACH__)
	#include <sys/utsname.h>
std::string getOSVersion()
{
	struct utsname sysInfo;
	if (uname(&sysInfo) == 0)
	{
		return std::string("macOS ") + sysInfo.release;
	}
	return "Unknown macOS Version";
}
#elif defined(__linux__)
	#include <sys/utsname.h>
std::string getOSVersion()
{
	struct utsname sysInfo;
	if (uname(&sysInfo) == 0)
	{
		return std::string("Linux ") + sysInfo.release;
	}
	return "Unknown Linux Version";
}
#else
std::string getOSVersion()
{
	return "Unknown OS";
}
#endif

std::string_view getCpuArchitecture()
{
#if IS_WIN
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	switch (sysinfo.wProcessorArchitecture)
	{
		case PROCESSOR_ARCHITECTURE_AMD64:
			return "x86_64";
		case PROCESSOR_ARCHITECTURE_ARM:
			return "arm";
		case PROCESSOR_ARCHITECTURE_ARM64:
			return "aarch64";
		default:
			return "Unknown";
	}
#elif defined(__linux__)
	static struct utsname uname_data;
	if (uname(&uname_data) == 0)
	{
		return uname_data.machine;
	}
	return "Unknown";
#else
	// Handle other platforms if needed
	return "Unknown";
#endif
}

}// namespace sf
