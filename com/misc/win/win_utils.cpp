#include "win_utils.h"
#include <windows.h>
#include <tlhelp32.h>
#include <cstring>

int strerror_r(int errnum, char* buf, size_t buflen)
{
	return strerror_s(buf, buflen, errnum);
}

namespace sf
{

bool isRunningWine()
{
	HMODULE ntdllMod = GetModuleHandle(L"ntdll.dll");
	if (ntdllMod && GetProcAddress(ntdllMod, "wine_get_version"))
	{
		return true;
	}
	return false;
}

size_t getThreadCount()
{
	size_t dwThreadCount = 0;
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		return (0);
	}
	te32.dwSize = sizeof(THREADENTRY32);
	if (!Thread32First(hThreadSnap, &te32))
	{
		CloseHandle(hThreadSnap);     // Must clean up the snapshot object!
		return (0);
	}
	do
	{
		dwThreadCount++;
/*
		printf("THREAD ID = 0x%08X (base priority  = %d | delta priority = %d)", te32.th32ThreadID, te32.tpBasePri, te32.tpDeltaPri);
		printf("\n");
*/
	}
	while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);
/*
	printf("\n\n\t Count %d threads.", dwThreadCount);
*/
	return dwThreadCount;
}

}
