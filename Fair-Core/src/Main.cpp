#include "client/module/ModuleManager.h"
#include "client/module/category/CategoryManager.h"
#include <Windows.h>
#include "inject/game_hook.h"
#include <string>
#include <tlhelp32.h>
#include <iostream>

#include "ui/panel.h"
int main() {

    CategoryManager::init();
    ModuleManager::init();
    localserver::init();

    panel::currentPage = panel::Page::Login;
    gui main_gui = gui();
    main_gui.init();
    FreeConsole();
    ExitProcess(0);

	return 0;
}