#include <Windows.h>
#include <iostream>
#include <vector>

#define ui unsigned long long

#include <Psapi.h>
#define InRange(x, a, b) (x >= a && x <= b)

#define getBit(x) (InRange((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xA): (InRange(x, '0', '9') ? x - '0': 0))

#define getByte(x) (getBit(x[0]) << 4 | getBit(x[1]))

ui FPat(const ui& start_address, const ui& end_address, const char* target_pattern) {

    const char* pattern = target_pattern;
    ui first_match = 0;

    for (ui position = start_address; position < end_address; position++) {

        if (!*pattern)

            return first_match;

        const unsigned char pattern_current = *reinterpret_cast<const unsigned char*>(pattern);
        const unsigned char memory_current = *reinterpret_cast<const unsigned char*>(position);

        if (pattern_current == '\?' || memory_current == getByte(pattern)) {

            if (!first_match)

                first_match = position;


            if (!pattern[2])

                return first_match;


            pattern += pattern_current != '\?' ? 3 : 2;
        }
        else {
            pattern = target_pattern;
            first_match = 0;
        }
    }
    return NULL;
}

inline ui GetAbsoluteAddress(ui instruction_ptr, int offset, int size)

{
    return instruction_ptr + *reinterpret_cast<int32_t*>(instruction_ptr + offset) + size;
}

ui FPat(const char* module, const char* target_pattern) {

    MODULEINFO module_info = { 0 };

    if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module), &module_info, sizeof(MODULEINFO)))
        return NULL;

    const ui start_address = ui(module_info.lpBaseOfDll);
    const ui end_address = start_address + module_info.SizeOfImage;
    return FPat(start_address, end_address, target_pattern);

}


void __cdecl attach(HMODULE hmod)
{
	FILE* f;
	AllocConsole();
	freopen_s(&f, "CONOUT$", "w", stdout);
	printf("Injected\n");


    typedef ui(__fastcall* fNextEnt)(ui, ui);
    fNextEnt NextEnt;
    ui CGameEntSystem;
    ui ent_find_pat;

	while ((GetAsyncKeyState(0x39) & 0x0001) == NULL)
	{
		if (GetAsyncKeyState(0x31) & 0x0001)
		{
            ent_find_pat = FPat("client.dll", "48 83 EC 28 83 3A 02 7D 12 48 8D 0D ? ? ? ? 48 83 C4 28 48 FF 25 ? ? ? ? 48 89 5C");

            if (ent_find_pat == 0) {
                printf("ent_find_pat is NULL");
                return;

            }

            NextEnt = (fNextEnt)GetAbsoluteAddress(ent_find_pat + 0x5d, 5, 2);
            CGameEntSystem = *(ui*)(GetAbsoluteAddress(ent_find_pat + 0x55, 3, 4));
            ui ent = NextEnt(CGameEntSystem, 0);
            printf("%u", ent);

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