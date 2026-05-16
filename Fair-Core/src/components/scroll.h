#pragma once
#include <unordered_map>
#include <vector>
#include "..\ImGui\imgui.h"
namespace scroll {
    struct SmoothScrollState
    {
        float current = 0.0f;
        float target = 0.0f;
        bool initialized = false;
    };
    inline std::unordered_map<ImGuiID, SmoothScrollState> g_smooth_scrolls;
    void ApplySmoothScroll(
        const char* id,
        float wheel_speed = 70.0f,
        float anim_speed = 16.0f);
}
