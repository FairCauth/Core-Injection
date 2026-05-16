#include "vapelite.h"
#include "..\..\client\module\ModuleManager.h"
#include "..\..\components\imgui_components.h"
#include "..\..\client\module\category\CategoryManager.h"
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
static Category* currentCategory = nullptr;


static bool setting_panel_opened = false;
static Module* setting_panel_module = nullptr;
static float settings_panel_anim = 0.0f;
void DrawModuleCard(Module* module, float width)
{
    ImGui::PushID(module);

    ImDrawList* draw = ImGui::GetWindowDrawList();

    constexpr float CARD_H = 70.0f;
    constexpr float ROUNDING = 8.0f;

    ImVec2 row_size(width, CARD_H);

    ImVec2 start = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("##card", row_size);

    bool card_hovered = ImGui::IsItemHovered();
    bool card_clicked = ImGui::IsItemClicked();

    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();


    ImVec2 next_cursor = ImGui::GetCursorScreenPos();

    ImU32 bg_col = module->enable ? IM_COL32(38, 38, 40, 255) : card_hovered
        ? IM_COL32(36, 36, 36, 255)
        : IM_COL32(31, 31, 33, 255);

    draw->AddRectFilled(
        min,
        max,
        bg_col,
        ROUNDING
    );



    #pragma region 图标绘制
    float icon_w = 62.0f;

    ImU32 icon_bg = module->enable
        ? imgui_components::ToU32(imgui_components::theme.focus_color)
        : IM_COL32(36, 36, 38, 255);

    draw->AddRectFilled(
        min,
        ImVec2(min.x + icon_w, max.y),
        icon_bg,
        ROUNDING,
        ImDrawFlags_RoundCornersLeft
    );



    ImVec2 icon_center(min.x + icon_w * 0.5f, min.y + CARD_H * 0.5f);
    const char* icon = "e";
    float icon_size = 30;

    ImVec2 text_size = fonts::icons->CalcTextSizeA(
        icon_size,
        FLT_MAX,
        0.0f,
        icon
    );

    draw->AddText(
        fonts::icons,
        icon_size,
        ImVec2(
            icon_center.x - text_size.x * 0.5f,
            icon_center.y - text_size.y * 0.5f
        ),
        module->enable
        ? IM_COL32(255, 255, 255, 255)
        : IM_COL32(188, 188, 188, 255),
        icon
    );
    #pragma endregion


    //draw->AddRect(
    //    ImVec2(icon_center.x - 7, icon_center.y - 7),
    //    ImVec2(icon_center.x + 7, icon_center.y + 7),
    //    IM_COL32(230, 230, 230, 230),
    //    2.0f,
    //    0,
    //    1.5f
    //);

    //module name
    draw->AddText(
        fonts::font_16,
        17.0f,
        ImVec2(min.x + icon_w + 18, min.y + 26),
        module->enable ? IM_COL32(255,255,255,255) : IM_COL32(188, 188, 188, 255),
        module->getName().c_str()
    );
    //outline
    //draw->AddRect(
    //    min,
    //    max,
    //    IM_COL32(55, 55, 65, 255),
    //    ROUNDING,
    //    0,
    //    1.0f
    //);

    //const char* desc = "Text Health";

    //draw->AddText(
    //    ImGui::GetFont(),
    //    14.0f,
    //    ImVec2(min.x + icon_w + 140, min.y + 21),
    //    IM_COL32(140, 140, 150, 255),
    //    desc
    //);

    #pragma region 设置图标绘制
    ImVec2 toggle_size(44, 22);
    ImVec2 toggle_pos(
        max.x - 20.0f - 18.0f - toggle_size.x,
        min.y + (CARD_H - toggle_size.y) * 0.5f
    );
    ImGui::SetCursorScreenPos(toggle_pos);
    imgui_components::ToggleButton(
        "##toggle",
        &module->enable,
        false,
        toggle_size
    );


    ImVec2 menu_min(max.x - 20.0f, min.y);
    ImVec2 menu_max(max.x, max.y);

    bool menu_hovered = ImGui::IsMouseHoveringRect(menu_min, menu_max);

    draw->AddRectFilled(
        menu_min,
        menu_max,
        (menu_hovered || module->enable) ? IM_COL32(47, 46, 51, 255) : IM_COL32(38, 38, 40, 255),
        ROUNDING,
        ImDrawFlags_RoundCornersRight
    );

    ImVec2 center(
        (menu_min.x + menu_max.x) * 0.5f,
        (menu_min.y + menu_max.y) * 0.5f
    );

    ImU32 dot_col = menu_hovered
        ? IM_COL32(255, 255, 255, 230)
        : IM_COL32(180, 180, 190, 150);

    float dot_r = 1.6f;
    float spacing = 6.0f;

    draw->AddCircleFilled(ImVec2(center.x, center.y - spacing), dot_r, dot_col);
    draw->AddCircleFilled(center, dot_r, dot_col);
    draw->AddCircleFilled(ImVec2(center.x, center.y + spacing), dot_r, dot_col);
    #pragma endregion 


    if (menu_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        //ImGui::OpenPopup("module_settings");
        setting_panel_opened = true;
        setting_panel_module = module;
    }

    //打开模块事件
    if (card_clicked && !menu_hovered)
    {
        module->toggle();
        Json::Value json;
        Json::FastWriter writer;
        json["type"] = "update_module";
        json["module"] = module->getName();
        json["enable"] = module->enable;
        //std::cout << module->getName() << module->enable << " " << std::endl;
        const std::string data = writer.write(json);
        localserver::send(data);
        //module->toggle();
    }


    ImGui::SetCursorScreenPos(next_cursor);

    ImGui::Dummy(ImVec2(0, 8));

    ImGui::PopID();
}
#include "..\..\components\scroll.h"
void DrawSettingsPanel(Module* module, float width)
{

    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImGui::BeginChild(
        "##setting",
        ImVec2(width, 0),
        false,
        ImGuiWindowFlags_NoScrollbar
    );
    {

        ImVec2 title_min = ImGui::GetWindowPos();
        ImVec2 title_max = ImVec2(
            title_min.x + ImGui::GetWindowWidth(),
            title_min.y + ImGui::GetWindowHeight()
        );

        //标题栏填充颜色
        draw->AddRectFilled(
            title_min,
            title_max,
            IM_COL32(24, 24, 26, 255)
        );

        //分割线
        draw->AddLine(
            ImVec2(title_min.x, title_max.y - 1.0f),
            ImVec2(title_max.x, title_max.y - 1.0f),
            IM_COL32(42, 42, 48, 255),
            1.0f
        );

        ImGui::SetCursorPos(ImVec2(12, 16));

        //返回图标按钮
        if (imgui_components::TextButton(
            "##back_button",
            "j",
            fonts::icons,
            22,
            ImVec2(28, 24),
            IM_COL32(170, 170, 180, 255),
            IM_COL32(255, 255, 255, 255)
        ))
        {
            setting_panel_opened = false;
        }


        ImGui::SameLine();

        ImGui::SetCursorPosY(18);
        ImGui::Text("%s Settings", module->getName().c_str());  
    



        ImGui::SetCursorPos(ImVec2(0, 55));
        #pragma region 设置组件绘制
        ImGui::BeginChild(
            "##module_setting_content",
            ImVec2(width, 0), 0, ImGuiWindowFlags_NoScrollWithMouse
        );

        int settingY = 10;
        {

            ImVec2 content_min = ImGui::GetWindowPos();
            ImVec2 content_max = ImVec2(
                content_min.x + ImGui::GetWindowWidth(),
                content_min.y + ImGui::GetWindowHeight()
            );

            draw->AddRectFilled(
                content_min,
                content_max,
                IM_COL32(16, 16, 20, 255)
            );
            for (auto setting : SettingManager::getSettings(module))
            {
                if (auto booleanSetting = dynamic_cast<BooleanSetting*>(setting.second)) {
                    bool value = booleanSetting->getValue();

                    ImGui::SetCursorPos(ImVec2(14, settingY));
                    ImGui::PushStyleColor(ImGuiCol_Text, value ? ImVec4(1, 1, 1, 1.0f) : ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
                    ImGui::TextUnformatted(booleanSetting->getName().c_str());
                    ImGui::PopStyleColor();

                    ImGui::SetCursorPos(ImVec2(width - 70, settingY));
                    if (imgui_components::ToggleButton(booleanSetting->getName().c_str(), &value)) {
                        Json::Value json;
                        Json::FastWriter writer;
                        json["type"] = "update_setting";
                        json["update_type"] = "boolean";
                        json["module"] = module->getName();
                        json["setting"] = booleanSetting->getName();
                        json["value"] = value;
                        const std::string data = writer.write(json);
                        localserver::send(data);
                    }
                }
                else if (auto numberSetting = dynamic_cast<NumberSetting*>(setting.second)) {
                    ImGui::SetCursorPos(ImVec2(14, settingY));

                    ImGui::PushFont(fonts::font_14);
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
                    ImGui::Text(numberSetting->getName().c_str());

                    char valueText[32];
                    snprintf(valueText, sizeof(valueText), "%.2f", numberSetting->getValue());

                    ImVec2 textSize = ImGui::CalcTextSize(valueText);

                    ImGui::SetCursorPos(ImVec2(
                        width - 30 - textSize.x,
                        settingY
                    ));

                    ImGui::TextUnformatted(valueText);

                    ImGui::PopFont();

                    settingY += 20;
                    ImGui::SetCursorPos(ImVec2(14, settingY));
                    float value = numberSetting->getValue();
                    if (imgui_components::SliderFloat(
                        numberSetting->getName().c_str(),
                        &value,
                        numberSetting->minValue,
                        numberSetting->maxValue, width - 50)) {

                        Json::Value json;
                        Json::FastWriter writer;
                        json["type"] = "update_setting";
                        json["update_type"] = "number";
                        json["module"] = module->getName();
                        json["setting"] = numberSetting->getName();
                        json["value"] = value;
                        const std::string data = writer.write(json);
                        localserver::send(data);
                    }
                    ImGui::PopStyleColor();
                }
                else if (auto modeSetting = dynamic_cast<ModeSetting*>(setting.second)) {
                    ImGui::SetCursorPos(ImVec2(14, settingY));

                    ImGui::TextUnformatted(modeSetting->getName().c_str());
                    settingY += 20;
                    ImGui::SetCursorPos(ImVec2(14, settingY));
                    int current = modeSetting->current;
                    ImGui::PushID(setting.second);

                    std::vector<imgui_components::DropdownItem> items;
                    items.reserve(modeSetting->values.size());

                    for (int i = 0; i < modeSetting->values.size(); i++)
                    {
                        items.push_back({
                            modeSetting->arr[i]
                            });
                    }
                    if (imgui_components::Dropdown(
                        "##mode",
                        items,
                        &current,
                        setting.first.c_str(), 
                        width - 60.0f
                    ))
                    {
                        modeSetting->setModeValue(current);
                        Json::Value json;
                        Json::FastWriter writer;
                        json["type"] = "update_setting";
                        json["update_type"] = "mode";
                        json["module"] = module->getName();
                        json["setting"] = modeSetting->getName();
                        json["value"] = modeSetting->values[current];
                        const std::string data = writer.write(json);
                        localserver::send(data);
                        //modeSetting->current = current;
                    }
                    settingY += 10;
                    ImGui::PopID();
                }
                settingY += 35;
            }

        }
        #pragma endregion


        ImGui::SetCursorPosY((float)settingY);
        ImGui::Dummy(ImVec2(1, 1));
        scroll::ApplySmoothScroll("##module_smooth_scroll", 80.0f, 18.0f);

        ImGui::EndChild();
        //ImGui::EndChild();
    }
    
    ImGui::EndChild();
}
float MoveTowards(float current, float target, float maxDelta)
{
    if (current < target)
    {
        current += maxDelta;
        if (current > target)
            current = target;
    }
    else if (current > target)
    {
        current -= maxDelta;
        if (current < target)
            current = target;
    }

    return current;
}
void vapelite_ui::on_draw(gui& gui) {
    ImDrawList* draw = ImGui::GetWindowDrawList();

    ImVec2 win_min = ImGui::GetWindowPos();
    ImVec2 win_max = ImVec2(
        win_min.x + ImGui::GetWindowWidth(),
        win_min.y + ImGui::GetWindowHeight()
    );
    //背景颜色
    draw->AddRectFilled(
        win_min,
        win_max,
        IM_COL32(20, 22, 24, 255), 
        10.0f 
    );



    #pragma region 上面导航栏绘制
    //Home
    ImGui::SetCursorPos({ 40, 25 });
    if (imgui_components::TextButton(
        "##home_btn",
        "a",
        fonts::icons,
        30,
        ImVec2(28, 24),
        IM_COL32(255, 255, 255, 255),
        IM_COL32(255, 255, 255, 255)
    ))
    {

    }
    draw->AddText(
        fonts::font_16,
        18,
        {80, 26},
        IM_COL32(255, 255, 255, 255),
        "Home"
    );

    //setting button
    ImGui::SetCursorPos({ gui::WINDOW_WIDTH - 80, 25 });
    if (imgui_components::TextButton(
        "##setting_btn",
        "f",
        fonts::icons,
        26,
        ImVec2(28, 24),
        IM_COL32(150, 150, 150, 255),
        IM_COL32(255, 255, 255, 255)
    ))
    {
        
    }
    ImGui::SetCursorPos({ gui::WINDOW_WIDTH - 80 - 60, 25 });

    if (imgui_components::TextButton(
        "##reset_btn",
        "c",
        fonts::icons,
        26,
        ImVec2(28, 24),
        IM_COL32(150, 150, 150, 255),
        IM_COL32(255, 255, 255, 255)
    ))
    {
        localserver::send_to_java("reconnect_gui");
    }
    #pragma endregion



    float panel_target_w = 280.0f;
    bool show_panel = setting_panel_opened && setting_panel_module != nullptr;

    #pragma region SettingPanel启动动画
    //settings_panel_anim += ((show_panel ? 1.0f : 0.0f) - settings_panel_anim)
    //    * ImMin(1.0f, ImGui::GetIO().DeltaTime * 14.0f);
    float target = show_panel ? 1.0f : 0.0f;
    float duration = 0.15f;
    float speed = ImGui::GetIO().DeltaTime / duration;
    settings_panel_anim = MoveTowards(
        settings_panel_anim,
        target,
        speed
    );
    if (!setting_panel_opened && settings_panel_anim < 0.001f)
    {
        setting_panel_module = nullptr;
        settings_panel_anim = 0.0f;
    }
    #pragma endregion

    float shift = (panel_target_w - 130.0f) * settings_panel_anim;

    ImGui::SetCursorPosY(80);
    ImVec2 nav_base = ImGui::GetCursorScreenPos();
    float nav_h = ImGui::GetContentRegionAvail().y;
    ImGui::SetCursorScreenPos(ImVec2(
        nav_base.x - shift,
        nav_base.y
    ));
    constexpr float NAV_W = 180.f;
    //Test
    #pragma region Category导航栏绘制
    ImGui::BeginChild("##nav", ImVec2(NAV_W, nav_h), false);
    //for (auto categoryMap : CategoryManager::getCategories()) {
    for (auto categoryMap : CategoryManager::getCategories()) {
        ImGui::SetCursorPosX(30);

        bool selected = (currentCategory == categoryMap.second);

        //楂樹寒閫変腑椤?
        if (selected)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
        }
        else
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.55f, .55f, .6f, 1));


        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));
        //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 16));
        if (ImGui::Selectable(categoryMap.first.c_str(), selected,
            ImGuiSelectableFlags_None, ImVec2(NAV_W - 8.f, 40.f)))
        {
            currentCategory = categoryMap.second;
        }
        ImGui::PopStyleColor(3);
        //ImGui::PopStyleVar();

        ImGui::PopStyleColor();

        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();

        if (selected) {


            draw->AddRectFilled(
                ImVec2(min.x + 4, max.y - 18),
                ImVec2(min.x + 4 + 16, max.y - 14),
                imgui_components::ToU32(imgui_components::theme.focus_color),
                3.f,
                ImDrawFlags_RoundCornersAll
            );
        }
    }

    ImGui::EndChild();
    #pragma endregion

    if (currentCategory == nullptr) 
        return;
   // ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, .5f));
    float spacing = ImGui::GetStyle().ItemSpacing.x;

    ImVec2 module_base = ImVec2(
        nav_base.x + NAV_W + spacing,
        nav_base.y
    );

    float module_area_w = gui::WINDOW_WIDTH - NAV_W;
    float module_area_w_1 = gui::WINDOW_WIDTH - NAV_W - 120;
    float module_area_h = nav_h;

    ImGui::SetCursorScreenPos(ImVec2(
        module_base.x - shift,
        module_base.y
    ));

    #pragma region 绘制模块
    ImGui::BeginChild("##module", ImVec2(module_area_w_1, module_area_h), false
    );
    std::vector<Module*> modules = ModuleManager::getModulesFromCategory(*currentCategory, "");
    float card_width = module_area_w_1 - 20;
    for (auto module : modules) {
        DrawModuleCard(module, card_width);

    }
    ImGui::EndChild();
    #pragma endregion

    #pragma region 绘制设置窗口
    if (settings_panel_anim > 0.001f && setting_panel_module != nullptr)
    {
        float panel_x =
            module_base.x + module_area_w - (panel_target_w * settings_panel_anim);

        ImGui::SetCursorScreenPos(ImVec2(
            panel_x,
            module_base.y - 30
        ));

        DrawSettingsPanel(setting_panel_module, panel_target_w);
    }

    #pragma endregion



    //ImGui::PopStyleColor();
}
