#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class IAT
{
public:
	static LPVOID Hook(LPCSTR lpModuleName, LPCSTR lpFunctionName, const LPVOID lpFunction, LPCSTR lpTargetModuleName);
	static LPVOID Hook(LPCSTR lpModuleName, LPCSTR lpFunctionName, const LPVOID lpFunction);
private:
	static LPVOID GetCurrentProcessModule();
};
