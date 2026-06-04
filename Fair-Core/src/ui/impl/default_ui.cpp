#include "default_ui.h"
#include "..\..\client\module\ModuleManager.h"
#include "..\..\components\imgui_components.h"
#include "..\..\ImGui\imconfig.h"
#include "..\..\ImGui\imgui.h"
#include "..\..\ImGui\imgui_impl_dx9.h"
#include "..\..\ImGui\imgui_impl_win32.h"
#include "..\..\ImGui\imgui_internal.h"
#include "..\..\ImGui\imstb_rectpack.h"
#include "..\..\ImGui\imstb_textedit.h"
#include "..\..\ImGui\imstb_truetype.h"
#include <string>
#include <iostream>
#include "..\font\fonts.h"

#include "..\..\client\module\ModuleManager.h"
#include "..\..\client\module\category\CategoryManager.h"
static Category* currentCategory = nullptr;
static bool globalFilter = true;

static char text[1024] = "";
static char pwd[1024] = "";
static bool remember;

void default_ui::on_draw(gui& gui) {
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