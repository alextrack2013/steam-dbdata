#include "pch.h"

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

	static bool is_wine()
	{
		HMODULE hntdll = GetModuleHandle(L"ntdll.dll");
		if (!hntdll)
		{
			return false;
		}

		if (GetProcAddress(hntdll, "wine_get_version") != NULL)
		{
			return true;
		}
		
		return false;
	}

	static void init()
	{
		if (!is_wine())
		{
			logger::info("Windows detected: DAI workaround is not needed.");
			return;
		}

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