#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

extern HMODULE winmm_dll;

DWORD WINAPI Load(LPVOID lpParam);
