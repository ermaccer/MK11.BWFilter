// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "utils/addr.h"
#include "utils/MemoryMgr.h"
#include "utils/Trampoline.h"
#include "utils/Patterns.h"

using namespace Memory::VP;
using namespace hook::txn;

void SetLevelDesaturation(int64 ptr, float value)
{
    value = 0.95f;
    *(int*)(ptr + 272) |= 0x2000u;
    *(float*)(ptr + 700) = value;
    if (*(int*)(ptr + 256))
    {
        *(int*)(ptr + 272) |= 0x400u;
        *(float*)(ptr + 688) = value;
    }
}

void Init()
{
    static uintptr_t pat = 0;

    pat = (uintptr_t)get_pattern("81 89 10 01 00 00 00 20 00 00");

    if (pat)
    {
        Trampoline* tramp = Trampoline::MakeTrampoline(GetModuleHandle(nullptr));
        InjectHook(pat, tramp->Jump(SetLevelDesaturation), PATCH_JUMP);
    }
    else
        MessageBoxA(0, "Failed to find required code pattern!\n\nGame or plugin needs updating.", "MK11.BWFilter", MB_ICONERROR);
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Init();
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

