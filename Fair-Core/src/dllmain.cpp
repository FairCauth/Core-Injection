#include <Windows.h>
#include "inject/game_hook.h"
#include <string>
#include <tlhelp32.h>
DWORD WINAPI run(LPVOID lpParam) {
    game_hook::install_hook();
    return 0;
}
bool isRunningInRundll32() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::wstring exeName(path);
    size_t pos = exeName.rfind(L'\\');
    if (pos != std::wstring::npos) {
        exeName = exeName.substr(pos + 1);
    }
    for (auto& c : exeName) c = towlower(c);
    return exeName == L"rundll32.exe";
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if(!isRunningInRundll32())
            CreateThread(NULL, 0, run, NULL, 0, NULL);
        
        break;
    }
    return TRUE;
}
#include "ui/Gui.h"
HANDLE g_thread = NULL;
extern "C" __declspec(dllexport) void CALLBACK Run(HWND hwnd, HINSTANCE hinst, LPSTR str, int ns) {
    gui main_gui = gui();
    main_gui.init();
    ExitProcess(0);

}