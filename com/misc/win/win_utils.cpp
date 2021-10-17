#include "win_utils.h"
#include <cstring>
#include <windows.h>

int strerror_r(int errnum, char *buf, size_t buflen)
{
	return strerror_s(buf, buflen, errnum);
}

bool isRunningWine()
{
	HMODULE ntdllMod = GetModuleHandle(L"ntdll.dll");
	if (ntdllMod && GetProcAddress(ntdllMod, "wine_get_version"))
		return true;
	return false;
}
