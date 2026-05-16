#include "scroll.h"
#include "..\ImGui\imconfig.h"
#include "..\ImGui\imgui_impl_dx9.h"
#include "..\ImGui\imgui_impl_win32.h"
#include "..\ImGui\imgui_internal.h"
#include "..\ImGui\imstb_rectpack.h"
#include "..\ImGui\imstb_textedit.h"
#include "..\ImGui\imstb_truetype.h"
#include "..\ImGui\imgui.h"
void scroll::ApplySmoothScroll(
    const char* id,
    float wheel_speed,
    float anim_speed) {
    ImGuiIO& io = ImGui::GetIO();

    ImGuiID scroll_id = ImGui::GetID(id);
    SmoothScrollState& st = g_smooth_scrolls[scroll_id];

    float real_scroll = ImGui::GetScrollY();
    float max_scroll = ImGui::GetScrollMaxY();

    if (!st.initialized)
    {
        st.current = real_scroll;
        st.target = real_scroll;
        st.initialized = true;
    }

    if (max_scroll <= 0.0f)
    {
        st.current = 0.0f;
        st.target = 0.0f;
        ImGui::SetScrollY(0.0f);
        return;
    }

    bool hovered = ImGui::IsWindowHovered(
        ImGuiHoveredFlags_AllowWhenBlockedByActiveItem |
        ImGuiHoveredFlags_AllowWhenBlockedByPopup
    );


    if (hovered && io.MouseWheel != 0.0f && !ImGui::IsAnyItemActive())
    {
        st.target -= io.MouseWheel * wheel_speed;
        st.target = ImClamp(st.target, 0.0f, max_scroll);
    }


    st.current += (st.target - st.current)
        * ImMin(1.0f, io.DeltaTime * anim_speed);

    if (fabsf(st.current - st.target) < 0.35f)
        st.current = st.target;

    st.current = ImClamp(st.current, 0.0f, max_scroll);

    ImGui::SetScrollY(st.current);
}