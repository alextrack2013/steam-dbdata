#include "pch.h"
#include <string>
#include <fileapi.h>
#include <Xinput.h>

namespace srttrworkaround
{
	typedef DWORD(WINAPI* XInputGetCapabilitiesOrg)(DWORD, DWORD, XINPUT_CAPABILITIES*);

	DWORD XInputGetCapabilitiesLoc(DWORD dwUserIndex, XINPUT_CAPABILITIES* pCapabilities)
	{
		auto hModule = LoadLibrary(L"xinput1_3");

		if (hModule == NULL)
		{
			return ERROR_ACCESS_DENIED;
		}

		auto fun = (XInputGetCapabilitiesOrg)GetProcAddress(hModule, "XInputGetCapabilities");

		if (fun == NULL)
		{
			return ERROR_ACCESS_DENIED;
		}

		return fun(dwUserIndex, 0, pCapabilities);
	}

	int64_t TimeToSeconds(tm* tm)
	{
		int64_t res = 0;

		res += tm->tm_sec;
		res += static_cast<int64_t>(tm->tm_min) * 60;
		res += static_cast<int64_t>(tm->tm_hour) * 3600;
		res += static_cast<int64_t>(tm->tm_yday) * 86400;
		res += static_cast<int64_t>(tm->tm_year) * 31536000;

		return res;
	}

	int64_t workaroundTime = 0;
	int64_t startTime = 0;

	bool shouldKill = false;

	HANDLE workaroundThreadHandle = NULL;

	DWORD WINAPI WorkaroundFun(LPVOID lpParam)
	{
		while (true)
		{
			SleepEx(1500, true);

			auto tempTime = workaroundTime;

			auto t = std::time(NULL);
			struct tm tm;
			localtime_s(&tm, &t);

			auto currTime = TimeToSeconds(&tm);

			if (tempTime != 0)
			{
				bool isGamepadConnected = false;

				for (int i = 0; i < 4; i++)
				{
					XINPUT_CAPABILITIES capabilities;
					ZeroMemory(&capabilities, sizeof(XINPUT_CAPABILITIES));

					if (XInputGetCapabilitiesLoc(i, &capabilities) == ERROR_SUCCESS)
					{
						isGamepadConnected = true;
						break;
					}
				}

				if (!isGamepadConnected)
				{
					logger::info("Reset timer.");

					tempTime = 0;
					workaroundTime = 0;
				}
			}

			if (tempTime != 0 && currTime - tempTime >= 3 && currTime - startTime >= 35)
			{
				if (shouldKill)
				{
					logger::info("Kill game, temp time: " + std::to_string(tempTime) + ", curr time: " + std::to_string(currTime) + ".");

					auto procId = GetCurrentProcessId();
					auto procHandle = OpenProcess(PROCESS_TERMINATE, false, procId);
					TerminateProcess(procHandle, 0);
					CloseHandle(procHandle);
				}
				else
				{
					shouldKill = true;
				}
			}
			else
			{
				shouldKill = false;
			}
		}

		return 0;
	}

	typedef DWORD(WINAPI* XInputGetStateLoc)(DWORD, XINPUT_STATE*);

	XInputGetStateLoc fpXInputGetState = NULL;

	DWORD DetourXInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
	{
		if (fpXInputGetState == NULL)
		{
			return 0;
		}

		//logger::info("Retireving XInput state.");

		auto t = std::time(NULL);
		struct tm tm;
		localtime_s(&tm, &t);

		workaroundTime = TimeToSeconds(&tm);

		if (startTime == 0)
		{
			startTime = workaroundTime;
		}

		return fpXInputGetState(dwUserIndex, pState);
	}

	static void init()
	{
		logger::info("Start SRTTR workaround.");

		if (MH_Initialize() != MH_OK)
		{
			logger::info("Could not initialize MinHook.");
			return;
		}

		logger::info("Hooked CloseHandle.");

		auto res = LoadLibrary(L"xinput1_3");

		if (res == NULL)
		{
			logger::info("Could not load xinput1_3.");
			return;
		}

		if (MH_CreateHookApiEx(L"xinput1_3", "XInputGetState", &DetourXInputGetState, &fpXInputGetState) != MH_OK)
		{
			logger::info("Could not hook XInputGetState.");
			return;
		}

		logger::info("Hooked XInputGetState.");

		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		{
			logger::info("Could not enable hooks.");
			return;
		}

		logger::info("MinHook initialization finished.");

		workaroundThreadHandle = CreateThread(NULL, 0, WorkaroundFun, NULL, 0, NULL);
	}
}