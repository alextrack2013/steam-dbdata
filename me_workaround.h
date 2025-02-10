#include "pch.h"
#include <string>
#include <MinHook.h>

namespace meworkaround
{
	typedef HANDLE(WINAPI* CreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

	CreateFileW fpCreateFileW = NULL;

	HANDLE DetourCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
	{
		std::wstring ws(lpFileName);
		std::string fileName = std::string(ws.begin(), ws.end());

		logger::info("Reading file '" + fileName + "' with flags: " + std::to_string(dwFlagsAndAttributes) + ".");
		//logger::info("Doing my job.");

		return fpCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	static void init()
	{
		logger::info("Start ME workaround.");

		if (MH_Initialize() != MH_OK)
		{
			logger::info("Could not initialize MinHook.");
			return;
		}

		HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
		FARPROC procCreateFileW = GetProcAddress(kernel32, "CreateFileW");

		if (MH_CreateHookEx(procCreateFileW, &DetourCreateFileW, &fpCreateFileW) != MH_OK)
		{
			logger::info("CreateFileW.");
			return;
		}

		logger::info("Hooked CreateFileW.");

		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		{
			logger::info("Could not enable hooks.");
			return;
		}

		logger::info("MinHook initialization finished.");
	}
}