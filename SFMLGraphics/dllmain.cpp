// dllmain.cpp : Defines the entry point for the DLL application.

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <CoreInterfaces.h>
#include "WindowType.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
	switch (reason) {
		case DLL_PROCESS_ATTACH:
			::DisableThreadLibraryCalls(hModule);
			break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

extern "C" int WINAPI InitModule(Dynamix::IRuntime* rt) {
	rt->RegisterType(WindowType::Get());
	return 0;
}
