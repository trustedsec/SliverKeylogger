#include "pch.h"
extern "C" {
    HINSTANCE hInst = nullptr;
    BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
    {
        if (hInst == nullptr) { hInst = hinstDLL; }
        switch (fdwReason)
        {
        case DLL_PROCESS_ATTACH:
            break;

        case DLL_PROCESS_DETACH:
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;
        }

        return TRUE;
    }
}