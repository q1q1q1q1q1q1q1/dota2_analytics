#include <Windows.h>
#include <iostream>



/*

int __fastcall Msg(__int64 a1, __int64 a2)
{
  return _Msg(a1, a2);
}

*/
typedef void __fastcall _MSG(const char* frmt, int, int, int);

_MSG* CMsg = (_MSG*)GetProcAddress(GetModuleHandleA("tier0.dll"), "ConMsg");


void __cdecl attach(HMODULE hmod)
{
	FILE* f;
	AllocConsole();
	freopen_s(&f, "CONOUT$", "w", stdout);
	printf("Injected\n");
	while ((GetAsyncKeyState(0x39) & 0x0001) == NULL)
	{
		if (GetAsyncKeyState(0x31) & 0x0001)
		{
			CMsg("mess", 0, 0, 0);
		}
	}
	printf("Deatach\n");
	FreeConsole();
	FreeLibraryAndExitThread(hmod, 0);
	
}




BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, LPVOID reserved)
{
	DisableThreadLibraryCalls(hmod);

	if (reason == DLL_PROCESS_ATTACH)
	{
		CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)attach, hmod, NULL, nullptr);
	}
	return TRUE;
}