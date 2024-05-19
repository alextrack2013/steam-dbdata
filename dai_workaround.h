#include "pch.h"
#include <MinHook.h>

template <typename T>
inline MH_STATUS MH_CreateHookEx(LPVOID pTarget, LPVOID pDetour, T** ppOriginal)
{
	return MH_CreateHook(pTarget, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}

template <typename T>
inline MH_STATUS MH_CreateHookApiEx(
	LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, T** ppOriginal)
{
	return MH_CreateHookApi(
		pszModule, pszProcName, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}

namespace daiworkaround
{
	typedef BOOL(WINAPI* NtUserAttachThreadInput)(DWORD, DWORD, BOOL);

	typedef HWND(WINAPI* GetFocus)(void);

	NtUserAttachThreadInput fpNtUserAttachThreadInput = NULL;

	GetFocus fpGetFocus = NULL;

	BOOL DetourNtUserAttachThreadInput(DWORD from, DWORD to, BOOL attach)
	{
		static int visited = 0;
		static DWORD fromThreadForHack = 0;
		static DWORD toThreadForHack = 0;
		
		if (!visited)
		{
			fromThreadForHack = from;
			toThreadForHack = to;
			visited = 1;
		}
		
		if (from == 0 && to == 0 && visited)
		{
			from = fromThreadForHack;
			to = toThreadForHack;
		}

		return fpNtUserAttachThreadInput(from, to, attach);
	}

	HWND DetourGetFocus(void)
	{
		HWND retValueWindow;
		static HWND prev = 0;

		retValueWindow = fpGetFocus();

		if (retValueWindow == 0 && prev != 0)
		{
			DetourNtUserAttachThreadInput(0, 0, 1);
		}
		else
		{
			prev = retValueWindow;
		}

		return retValueWindow;
	}

	static void init()
	{
		logger::info("Start DAI workaround.");

		if (MH_Initialize() != MH_OK)
		{
			logger::info("Could not initialize MinHook.");
			return;
		}

		if (MH_CreateHookApiEx(L"win32u", "NtUserAttachThreadInput", &DetourNtUserAttachThreadInput, &fpNtUserAttachThreadInput) != MH_OK)
		{
			logger::info("Could not hook NtUserAttachThreadInput.");
			return;
		}

		logger::info("Hooked NtUserAttachThreadInput.");

		if (MH_CreateHookApiEx(L"user32", "GetFocus", &DetourGetFocus, &fpGetFocus) != MH_OK)
		{
			logger::info("Could not hook GetFocus.");
			return;
		}

		logger::info("Hooked GetFocus.");

		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		{
			logger::info("Could not enable hooks.");
			return;
		}

		logger::info("MinHook initialization finished.");
	}
}