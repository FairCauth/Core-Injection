#pragma once
#include "..\ImGui\imgui.h"
#include <string>
#include <functional>
#include <algorithm>
#include <cstring>

namespace imgui_components {
    struct UITheme
    {
        //40,116,228 蓝
        //224,172,0 黄
        ImVec4 focus_color = ImVec4(40 / 255.f, 116 / 255.f, 228 / 255.f, 1.0f);
    };
    inline UITheme theme;
    inline ImU32 ToU32(const ImVec4& col)
    {
        return ImGui::ColorConvertFloat4ToU32(col);
    }

    struct DropdownItem {
        std::string label;
        bool separator_before = false;
    };




    bool ToggleButton(
        const char* text_id,
        bool* v,
        bool        disabled = false,
        ImVec2      track_size= {42,22} );
    bool Dropdown(
        const char* label,
        const std::vector<DropdownItem>& items,
        int* current,
        const char* preview = "请选择...",
        float width = 160.f,
        bool disabled = false);

    bool SliderFloat(
        const char* label,
        float* v,
        float       v_min = 0.f,
        float       v_max = 1.f,
        float       width = 0.f,
        int         decimals = 2,
        bool        disabled = false);

    bool SliderFloat_cutout(
        const char* label,
        float* v,
        float       v_min,
        float       v_max,
        float       width,
        int         decimals, 
        ImU32 cutout_col
    );
    bool TextButton(
        const char* id,
        const char* text,
        ImFont* font = nullptr,
        float font_size = 0.0f,
        ImVec2 size = ImVec2(0, 0),
        ImU32 normal_col = IM_COL32(170, 170, 180, 255),
        ImU32 hover_col = IM_COL32(255, 255, 255, 255),
        bool disabled = false
    );
}