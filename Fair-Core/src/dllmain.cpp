#include <Windows.h>
#include "inject/game_hook.h"
DWORD WINAPI run(LPVOID lpParam) {
    game_hook::install_hook();
    return 0;
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, run, NULL, 0, NULL);
        break;
    }
    return TRUE;
}

