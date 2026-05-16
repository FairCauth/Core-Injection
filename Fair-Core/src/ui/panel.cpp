#include "panel.h"
#include "..\ImGui\imconfig.h"
#include "..\ImGui\imgui.h"
#include "..\ImGui\imgui_impl_dx9.h"
#include "..\ImGui\imgui_impl_win32.h"
#include "..\ImGui\imgui_internal.h"
#include "..\ImGui\imstb_rectpack.h"
#include "..\ImGui\imstb_textedit.h"
#include "..\ImGui\imstb_truetype.h"
#include <string>
#include <iostream>
void DrawGlitchText(ImVec2 pos, const char* text, ImFont* font)
{
    ImDrawList* draw = ImGui::GetWindowDrawList();

    float size = font->FontSize;
    float t = ImGui::GetTime();

    enum Phase { Idle, SmallGlitch, Pause, BigGlitch };

    static Phase phase = Idle;
    static float phaseEnd = 0;
    static float nextStart = 0;

    // glitch参数刷新控制
    static float nextUpdate = 0;

    struct Slice
    {
        float y;
        float h;
        float shift;
    };

    static Slice slices[8];
    static int sliceCount = 0;
    static float rgbOffset = 0;

    if (phase == Idle && t > nextStart)
    {
        phase = SmallGlitch;
        phaseEnd = t + 0.07f;
    }

    if (phase == SmallGlitch && t > phaseEnd)
    {
        phase = Pause;
        phaseEnd = t + 0.07f;
    }

    if (phase == Pause && t > phaseEnd)
    {
        phase = BigGlitch;
        phaseEnd = t + 0.18f;
    }

    if (phase == BigGlitch && t > phaseEnd)
    {
        phase = Idle;
        nextStart = t + 2.5f + (rand() % 200) / 100.0f;
    }

    ImVec2 base = ImFloor(pos);

    draw->AddText(font, size, base, IM_COL32_WHITE, text);

    if (phase == Idle)
        return;

    if (t > nextUpdate)
    {
        nextUpdate = t + 0.06f;

        if (phase == SmallGlitch)
        {
            sliceCount = 2;
            rgbOffset = 1.5f;
        }
        else
        {
            sliceCount = 5;
            rgbOffset = 3.0f;
        }

        for (int i = 0;i < sliceCount;i++)
        {
            slices[i].y = base.y + rand() % 40;
            slices[i].h = 5 + rand() % 8;
            slices[i].shift = (phase == SmallGlitch)
                ? rand() % 8 - 4
                : rand() % 24 - 12;
        }
    }

    draw->AddText(
        font, size,
        ImFloor(ImVec2(base.x - rgbOffset, base.y)),
        IM_COL32(255, 0, 0, 180),
        text);

    draw->AddText(
        font, size,
        ImFloor(ImVec2(base.x + rgbOffset, base.y)),
        IM_COL32(0, 255, 255, 180),
        text);

    for (int i = 0;i < sliceCount;i++)
    {
        draw->PushClipRect(
            ImVec2(0, slices[i].y),
            ImVec2(5000, slices[i].y + slices[i].h),
            true);

        draw->AddText(
            font, size,
            ImFloor(ImVec2(base.x + slices[i].shift, base.y)),
            IM_COL32_WHITE,
            text);

        draw->PopClipRect();
    }
}
void DrawMixedGlitchText(ImVec2 pos, const char* text, ImFont* font)
{
    ImDrawList* draw = ImGui::GetWindowDrawList();

    std::string str = text;
    float x = pos.x;
    float y = pos.y;
    float fontSize = font->FontSize;
    size_t i = 0;

    while (i < str.size())
    {
        if (str[i] == '{')
        {
            size_t end = str.find('}', i);
            if (end == std::string::npos)
                break;

            std::string glitchText = str.substr(i + 1, end - i - 1);

            DrawGlitchText(ImVec2(x, y), glitchText.c_str(), font);

            ImVec2 size = font->CalcTextSizeA(
                fontSize,
                FLT_MAX,
                0.0f,
                glitchText.c_str()
            );

            x += size.x;
            i = end + 1;
        }
        else
        {
            size_t next = str.find('{', i);

            std::string normal =
                str.substr(i,
                    next == std::string::npos
                    ? str.size() - i
                    : next - i);

            draw->AddText(
                font,
                fontSize,
                ImFloor(ImVec2(x, y)),
                IM_COL32_WHITE,
                normal.c_str()
            );

            ImVec2 size = font->CalcTextSizeA(
                fontSize,
                FLT_MAX,
                0.0f,
                normal.c_str()
            );

            x += size.x;

            if (next == std::string::npos)
                break;

            i = next;
        }
    }
}


static bool  g_LoginLoading = false;
static float g_ProgressTarget = 0.0f; 
static float g_ProgressCurrent = 0.0f;   
static const float EASE_SPEED = 8.0f;  

void SetLoginProgress(float target)
{
    g_ProgressTarget = ImClamp(target, 0.0f, 1.0f);
    g_LoginLoading = true;
}
static char text[1024] = "";
static char pwd[1024] = "";
static bool remember;
#include "..\socket\localserver.h"
std::string right_of(const std::string& s, const std::string& delimiter) {
    size_t pos = s.find(delimiter);
    if (pos == std::string::npos) return "";
    return s.substr(pos + delimiter.size());
}
std::string get_self_path() {
    char path[MAX_PATH];
    HMODULE hModule = NULL;
    GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)&get_self_path,
        &hModule
    );
    GetModuleFileNameA(hModule, path, MAX_PATH);
    std::string s(path);
    return s.substr(0, s.find_last_of("\\/"));
}
std::string get_self_name() {

    char path[MAX_PATH];

    HMODULE hModule = NULL;

    GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)&get_self_name,
        &hModule
    );

    GetModuleFileNameA(hModule, path, MAX_PATH);

    std::string s(path);
    return s.substr(s.find_last_of("\\/") + 1);
}
int max = 0;
#include "..\dist\json\json.h"
#include "..\client\module\Module.h"
#include "..\client\module\ModuleManager.h"

std::string panel::from_client(std::string msg) {
    //std::cout << "[recv] " << msg << std::endl;
    bool printLog = true;
    if (msg.find("run!") != std::string::npos) {
        std::cout << "收到启动命令" << std::endl;
        SetLoginProgress(0.1f);
        return get_self_path();
    }
    if (msg.find("ask_dll_name") != std::string::npos) {
        std::cout << "ask_dll_name" << std::endl;
        SetLoginProgress(0.15f);
        return get_self_name();
    }
    std::string result = right_of(msg, "start transformer ");
    if (result != "") {
        max = std::stoi(result);
        SetLoginProgress(0.2f);
    }
    if (right_of(msg, "-> Transform OK") != "") {
        float step = (0.8f - 0.2f) / max;
        SetLoginProgress(g_ProgressTarget + step);
    }
    
    if (msg.find("init ok") != std::string::npos) {
        SetLoginProgress(1.0f);
        panel::currentPage = panel::Page::Main;
        //localserver::shutdown();
    }
    if (msg.find("init gui") != std::string::npos) {
        std::cout << "初始化GUI" << std::endl;
        ModuleManager::clear();
        SettingManager::clear();
    }
    if (msg.find("{") != std::string::npos) {
        printLog = false;
        Json::Reader reader;
        Json::Value root;
        bool success = reader.parse(msg, root);
        if (!success) {
            std::cout << "Error json " << msg << std::endl;
        }
        std::string type = root["type"].asString();
        if (type == "register_module") {
            std::string moduleName = root["module"].asString();
            std::string category = root["category"].asString();
            int key = root["key"].asInt();
            bool enable = root["enable"].asInt();
            std::cout << "register_module " << moduleName << " " << category << std::endl;
            Module* module = new Module(moduleName, category, key);
            module->setEnable(enable);
            ModuleManager::registerModule(module);
        }
        if (type == "register_setting") {
            std::string moduleName = root["module"].asString();
            std::string settingName = root["setting"].asString();
            std::cout << "register_setting " << moduleName << " " << settingName << std::endl;
            std::string setting_type = root["setting_type"].asString();
            int level = root["level"].asInt();
            bool display = root["display"].asBool();
            Module* mod = ModuleManager::getModule(moduleName);
            if (setting_type == "boolean") {
                bool value = root["value"].asBool();
                Setting<bool>* setting = new BooleanSetting(settingName, value);
                setting->level = level;
                setting->display = display;
                mod->registerSetting(setting);
            }
            else if (setting_type == "number") {
                float value = root["value"].asFloat();
                float min = root["min"].asFloat();
                float max = root["max"].asFloat();
                std::string precise = root["precise"].asString();
                Setting<float>* setting = new NumberSetting(settingName, value, min, max, precise);
                setting->level = level;
                setting->display = display;
                mod->registerSetting(setting);
            }
            else if (setting_type == "mode") {
                std::string value = root["value"].asString();
                Json::Value modes = root["modes"];

                std::vector<std::string> modeList;
                for (int i = 0; i < modes.size(); i++) 
                    modeList.push_back(modes[i].asString());
                
                Setting<std::string>* setting = new ModeSetting(settingName, value, modeList);
                setting->level = level;
                setting->display = display;
                mod->registerSetting(setting);
            }

        }

        if (type == "update_module") {
            std::string moduleName = root["module"].asString();
            int key = root["key"].asInt();
            bool enable = root["enable"].asInt();
            Module* module = ModuleManager::getModule(moduleName);
            module->setEnable(enable);
            module->key = key;
        }
        if (type == "update_setting") {
            std::string moduleName = root["module"].asString();
            std::string settingName = root["setting"].asString();
            //std::cout << "update_setting " << moduleName << " " << settingName << std::endl;
            std::string setting_type = root["setting_type"].asString();
            bool display = root["display"].asBool();
            Module* mod = ModuleManager::getModule(moduleName);
            if (mod == nullptr) {
                std::cout << "update_setting: module not found: " << moduleName << std::endl;
                return "";
            }
            if (setting_type == "boolean") {
                bool value = root["value"].asBool();
                Setting<bool>* setting = SettingManager::getSetting<bool>(mod, settingName);
                if (setting == nullptr) {
                    std::cout << "update_setting: setting not found: " << settingName << std::endl;
                    return "";
                }
                setting->setValue(value);
                setting->display = display;
            }
            else if (setting_type == "number") {
                float value = root["value"].asFloat();
                Setting<float>* setting = SettingManager::getSetting<float>(mod, settingName);
                if (setting == nullptr) {
                    std::cout << "update_setting: setting not found: " << settingName << std::endl;
                    return "";
                }
                setting->setValue(value);
                setting->display = display;
            }
            else if (setting_type == "mode") {
                std::string value = root["value"].asString();
                Setting<std::string>* setting = SettingManager::getSetting<std::string>(mod, settingName);
                if (setting == nullptr) {
                    std::cout << "update_setting: setting not found: " << settingName << std::endl;
                    return "";
                }
                ModeSetting* mode = dynamic_cast<ModeSetting*>(setting);
                for (size_t i = 0; i < mode->values.size(); i++) {
                    if (mode->values[i] == value) mode->current = i;
                }
                setting->setValue(value);
                setting->display = display;
                //Setting<std::string>* setting = new ModeSetting(settingName, value);
            }
        }


    }

    if (printLog) {
        std::cout << "[recv] " << msg << std::endl;
    }
    return "";
}
#include "..\common\utils.h"
#include <vector>
#include <psapi.h>
#include <locale>
#include <codecvt>
#include <tlhelp32.h>
#include <comdef.h>
static bool show_list = false;
static int selected = -1;
struct JavaProcess {
    DWORD pid;
    std::string windowTitle;
};
static std::vector<JavaProcess> java_processes;
struct EnumData {
    DWORD pid;
    std::string title;
};

static BOOL CALLBACK FindWindowByPid(HWND hwnd, LPARAM lParam) {
    auto* data = (EnumData*)lParam;
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == data->pid) {
        char title[256];
        GetWindowTextA(hwnd, title, sizeof(title));
        if (strlen(title) > 0) {
            data->title = title;
            return FALSE;
        }
    }
    return TRUE;
}
void RefreshJavaList() {
    java_processes.clear();

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);

    if (Process32FirstW(snapshot, &pe)) {
        do {
            std::wstring name(pe.szExeFile);
            if (name == L"java.exe" || name == L"javaw.exe") {
                EnumData data{ pe.th32ProcessID, "" };
                EnumWindows(FindWindowByPid, (LPARAM)&data);
                if (data.title.empty())
                    data.title = "PID " + std::to_string(pe.th32ProcessID);

                java_processes.push_back({ pe.th32ProcessID, data.title });
            }
        } while (Process32NextW(snapshot, &pe));
    }

    CloseHandle(snapshot);
}
#include "..\components\imgui_components.h"

void draw_login_gui(gui& gui) {
    if (ImGui::BeginTabBar("Bar1")) {
        if (ImGui::BeginTabItem("Login")) {
            std::string str = "Test Client";
            ImFont* glitchText = gui.glitchText;
            ImVec2 size = glitchText->CalcTextSizeA(
                glitchText->FontSize,
                FLT_MAX,
                0.0f,
                str.c_str()
            );
            DrawMixedGlitchText({ gui::WINDOW_WIDTH / 2.0f - (size.x / 2.0f) ,60 }, "{Test Client}", glitchText);
            float child_width = 400, child_height = 200;

            ImGui::SetCursorPos({ gui::WINDOW_WIDTH / 2.0f - child_width / 2.0f, gui::WINDOW_HEIGHT / 2.0f - child_height / 2.0f });

            ImGui::BeginChild("##A", { child_width, child_height }, false, ImGuiWindowFlags_NoScrollbar);
            if (!g_LoginLoading)
            {


                ImGui::SetCursorPos({ 25, 20 });
                ImGui::Text("Account");

                ImGui::SetCursorPos({ 100, 20 });
                ImGui::InputText("##A", text, sizeof(text));

                ImGui::SetCursorPos({ 25, 60 });
                ImGui::Text("Password");

                ImGui::SetCursorPos({ 100, 60 });
                ImGui::InputText("##B", pwd, sizeof(pwd), ImGuiInputTextFlags_Password);


                ImGui::SetCursorPos({ 230, 90 });
                ImGui::Checkbox("Remember Me", &remember);


                ImGui::SetCursorPos({ 150, 140 });
                if (ImGui::Button("Login", { 100, 40 }))
                {
                    std::cout << get_self_path() << " " << get_self_name() << std::endl;
                    RefreshJavaList();
                    show_list = true;
                    selected = -1;
                }

                if (show_list) {
                    ImGui::SetNextWindowSize({ 350, 200 }, ImGuiCond_Once);
                    ImGui::SetNextWindowPos({ 200, 150 }, ImGuiCond_Once);
                    ImGui::SetNextWindowFocus();
                    ImGui::Begin(u8"选择进程", &show_list);

                    for (int i = 0; i < java_processes.size(); i++) {
                        std::string label = "[" + std::to_string(java_processes[i].pid) + "] " + java_processes[i].windowTitle;
                        if (ImGui::Selectable(label.c_str(), selected == i))
                            selected = i;
                    }

                    if (selected >= 0 && ImGui::Button(u8"确认", { 100, 30 })) {
                        DWORD pid = java_processes[selected].pid;
                        SetLoginProgress(0.05f);
                        const char* dllPath = (get_self_path() + "\\" + get_self_name()).c_str();
                        localserver::init();
                        Sleep(1000);
                        utils::others::inject_dll(pid, dllPath);
                        max = 0;
                        
                        show_list = false;
                    }

                    ImGui::End();
                }
            }
            else
            {
                float dt = ImGui::GetIO().DeltaTime;
                g_ProgressCurrent += (g_ProgressTarget - g_ProgressCurrent)
                    * (1.0f - expf(-EASE_SPEED * dt));
                if (fabsf(g_ProgressTarget - g_ProgressCurrent) < 0.001f)
                    g_ProgressCurrent = g_ProgressTarget;
                ImVec2 winPos = ImGui::GetWindowPos();
                float  barW = child_width - 50.0f;
                float  barH = 8.0f;
                ImVec2 barMin = { winPos.x + 25.0f, winPos.y + 145.0f };
                ImVec2 barMax = { barMin.x + barW,  barMin.y + barH };

                ImDrawList* dl = ImGui::GetWindowDrawList();
                dl->AddRectFilled(barMin, barMax, IM_COL32(60, 60, 60, 255), 4.0f);
                dl->AddRectFilled(
                    barMin,
                    { barMin.x + barW * g_ProgressCurrent, barMax.y },
                    IM_COL32(80, 160, 255, 255), 4.0f);

                //char buf[16];
                //snprintf(buf, sizeof(buf), "%d%%", (int)(g_ProgressCurrent * 100));
                //ImVec2 textSz = ImGui::CalcTextSize(buf);
                //ImVec2 textPos = {
                //    barMin.x + (barW - textSz.x) * 0.5f,
                //    barMin.y + (barH - textSz.y) * 0.5f
                //};
                //dl->AddText(textPos, IM_COL32_WHITE, buf);
                if (g_ProgressCurrent >= 1.0f)
                {
                    g_LoginLoading = false;
                    g_ProgressCurrent = 0.0f;
                    g_ProgressTarget = 0.0f;
                    
                    //MessageBoxA(NULL, "Injected", NULL, NULL);
                }
            }
            ImGui::EndChild();


            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Register")) {
         
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

}

#include "..\client\module\ModuleManager.h"
#include "..\client\module\category\CategoryManager.h"
static Category* currentCategory = nullptr;
static bool globalFilter = true;
void draw_main_gui(gui& gui) {
    static int selectedItem = -1;
    #pragma region CategoriesMenu
    ImGui::SetCursorPos({ 2,3 });

    if (ImGui::BeginTabBar("MyTabBar")) {
        for (auto categoryMap : CategoryManager::getCategories()) {
            if (ImGui::BeginTabItem(categoryMap.first.c_str()))
            {
                currentCategory = categoryMap.second;
                ImGui::EndTabItem();
            }
        }
    }

    #pragma endregion

    if (currentCategory != nullptr) {
        std::string filterText(text);

        std::vector<Module*> modules = (globalFilter && !filterText.empty()) ? 
            ModuleManager::getFilteredModules(filterText) : 
            ModuleManager::getModulesFromCategory(*currentCategory, filterText
        );
        
        #pragma region 搜索框
        if (selectedItem > modules.size() - 1) selectedItem = -1;
        ImGui::SetNextItemWidth(240);
        ImGui::InputText("##Filter", text, sizeof(text));
        ImGui::SameLine();
        ImGui::Text("Filter");
        ImGui::SameLine();

        ImGui::Checkbox("All Modules", &globalFilter);
        
        #pragma endregion

        #pragma region 模块列表框
        if (ImGui::BeginListBox("##listbox", ImVec2(150, gui::WINDOW_HEIGHT - 70))) {
            int index = 0;
            for (auto module : modules) {
                bool isSelected = (currentCategory->selectedItemIndex == index);
                if (ImGui::Selectable(module->getName().c_str(), isSelected || module->isEnable())) {
                    selectedItem = index;
                    //currentCategory = module->category;
                    currentCategory->selectedItemIndex = index;
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
                ++index;
            }
            ImGui::EndListBox();
        }
        #pragma endregion
        ImGui::SameLine();
        if (selectedItem != -1 && !modules.empty() && currentCategory->selectedItemIndex != -1) {

            Module* selectedModule = modules[currentCategory->selectedItemIndex];
            ImGui::BeginChild("##A", { gui::WINDOW_WIDTH - 170 , gui::WINDOW_HEIGHT }, false, ImGuiWindowFlags_NoScrollbar);


            #pragma region 模块标题栏 
            ImGui::BeginChild("##B", { gui::WINDOW_WIDTH - 170  , 23 }, false);
            ImGui::Text(selectedModule->getName().c_str());
            //ImGui::SameLine();
            ImGui::Separator();
            ImGui::EndChild();
            #pragma endregion

            //设置
            ImGui::BeginChild("##C", { gui::WINDOW_WIDTH - 170 , gui::WINDOW_HEIGHT - 100 }, false);
            bool enable = selectedModule->enable;
            if (ImGui::Checkbox("Enable", &enable)) {
                Json::Value json;
                Json::FastWriter writer;
                json["type"] = "update_module";
                json["module"] = selectedModule->getName();
                json["enable"] = enable;
                const std::string data = writer.write(json);
                localserver::send(data);
            }
            #pragma region 设置组件
            for (auto setting : SettingManager::getSettings(selectedModule))
            {
                auto getInfo = [&](int level, bool display) -> int {
                    if (!display) return -1;
                    int offsetX = level * 20 + 4;
                    if (offsetX != 4) {
                        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
                        ImVec2 nodePos = ImVec2(cursorPos.x, cursorPos.y + ImGui::GetTextLineHeight() * 0.6f);
                        ImDrawList* draw_list = ImGui::GetWindowDrawList();
                        draw_list->AddLine(nodePos, ImVec2(nodePos.x + level * 20 - 6, nodePos.y), IM_COL32(200, 200, 200, 255), 1.0f);
                        draw_list->AddLine(nodePos, ImVec2(nodePos.x, nodePos.y - 26), IM_COL32(200, 200, 200, 255), 1.0f);
                    }
                    return offsetX;
                };

                #pragma region BooleanSetting
                if (auto booleanSetting = dynamic_cast<BooleanSetting*>(setting.second)) {
                    int offsetX = getInfo(booleanSetting->level, booleanSetting->display);
                    if (offsetX == -1) continue;
                    ImGui::SetCursorPosX(offsetX);
                    bool value = booleanSetting->getValue();
                    if (ImGui::Checkbox(booleanSetting->getName().c_str(), &value)) {
                        Json::Value json;
                        Json::FastWriter writer;
                        json["type"] = "update_setting";
                        json["update_type"] = "boolean";
                        json["module"] = selectedModule->getName();
                        json["setting"] = booleanSetting->getName();
                        json["value"] = value;
                        const std::string data = writer.write(json);
                        localserver::send(data);

  
                    }
                }
                #pragma endregion

                #pragma region NumberSetting
                else if (auto numberSetting = dynamic_cast<NumberSetting*>(setting.second)) {
                    int offsetX = getInfo(numberSetting->level, numberSetting->display);
                    if (offsetX == -1) continue;

                    ImGui::SetCursorPosX(offsetX);

                    float value = numberSetting->getValue();
                    if (ImGui::SliderFloat(numberSetting->getName().c_str(), &value, numberSetting->minValue, numberSetting->maxValue, numberSetting->precisePattern.c_str())) {
                        Json::Value json;
                        Json::FastWriter writer;
                        json["type"] = "update_setting";
                        json["update_type"] = "number";
                        json["module"] = selectedModule->getName();
                        json["setting"] = numberSetting->getName();
                        json["value"] = value;
                        const std::string data = writer.write(json);
                        localserver::send(data);
                    }
                }
                #pragma endregion

                #pragma region ModeSetting
                else if (auto modeSetting = dynamic_cast<ModeSetting*>(setting.second)) {
                    int offsetX = getInfo(modeSetting->level, modeSetting->display);
                    if (offsetX == -1) continue;

                    ImGui::SetCursorPosX(offsetX);
                    ImGui::SetNextItemWidth(160);
                    int current = modeSetting->current;
                    if (ImGui::Combo(setting.first.c_str(), &current, modeSetting->arr, modeSetting->values.size())) {
                        modeSetting->setModeValue(current);
                        Json::Value json;
                        Json::FastWriter writer;
                        json["type"] = "update_setting";
                        json["update_type"] = "mode";
                        json["module"] = selectedModule->getName();
                        json["setting"] = modeSetting->getName();
                        json["value"] = modeSetting->values[current];
                        const std::string data = writer.write(json);
                        localserver::send(data);
                    }
                }
                #pragma endregion
            }
            #pragma endregion
            ImGui::EndChild();

        }
    }
}



#include "vape/vapelite.h"
void panel::on_draw(gui& gui) {

    switch (currentPage)
    {
    case Login:
        draw_login_gui(gui);
        //vapelite_ui::on_draw(gui);
        break;
    case Main:
        vapelite_ui::on_draw(gui);
        //draw_main_gui(gui);
        break;
    }


}

