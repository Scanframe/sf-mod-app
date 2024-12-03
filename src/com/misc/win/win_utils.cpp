#include "win_utils.h"
#include <cstring>
#include <stdexcept>
#include <tlhelp32.h>

namespace sf
{

bool isRunningWine()
{
	HMODULE handle = ::GetModuleHandleA("ntdll.dll");
	if (handle && ::GetProcAddress(handle, "wine_get_version"))
	{
		return true;
	}
	return false;
}

std::string getModulePath(const std::string& dll_name)
{
	HMODULE handle = ::GetModuleHandleA(dll_name.c_str());
	if (handle)
	{
		std::string rv(MAX_PATH, '\0');
		rv.resize(::GetModuleFileNameA(handle, rv.data(), rv.capacity()));
		return rv;
	}
	return {};
}

int getThreadCount()
{
	int thread_count = 0;
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
		CloseHandle(hThreadSnap);// Must clean up the snapshot object!
		return (0);
	}
	do
	{
		thread_count++;
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);
	return thread_count;
}

DWORD getMainThreadId()
{
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (handle == INVALID_HANDLE_VALUE)
	{
		throw std::runtime_error("GetMainThreadId failed");
	}
	THREADENTRY32 tEntry;
	tEntry.dwSize = sizeof(THREADENTRY32);
	DWORD result = 0;
	DWORD currentPid = ::GetCurrentProcessId();
	for (BOOL success = Thread32First(handle, &tEntry);
			 !result && success && GetLastError() != ERROR_NO_MORE_FILES;
			 success = Thread32Next(handle, &tEntry))
	{
		if (tEntry.th32OwnerProcessID == currentPid)
		{
			result = tEntry.th32ThreadID;
		}
	}
	::CloseHandle(handle);
	return result;
}

}// namespace sf
