#include <Windows.h>
#include "inject/game_hook.h"
#include <string>
#include <tlhelp32.h>
#include <iostream>

#include "client/module/ModuleManager.h"
#include "client/module/category/CategoryManager.h"
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
void AllocConsole_Init() {
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    std::ios::sync_with_stdio(true);
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
#include "ui/panel.h"
HANDLE g_thread = NULL;
extern "C" __declspec(dllexport) void CALLBACK Run(HWND hwnd, HINSTANCE hinst, LPSTR str, int ns) {
    AllocConsole_Init();

    CategoryManager::init();
    ModuleManager::init();
    localserver::init();

    if (localserver::check_port(8888)) {
        panel::currentPage = panel::Page::Main;
    }
    else {
        panel::currentPage = panel::Page::Login;
    }
    gui main_gui = gui();
    main_gui.init();
    FreeConsole();
    ExitProcess(0);

}