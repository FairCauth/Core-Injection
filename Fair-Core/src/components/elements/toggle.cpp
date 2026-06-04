#include "..\..\ImGui\imconfig.h"
#include "..\..\ImGui\imgui_impl_dx9.h"
#include "..\..\ImGui\imgui_impl_win32.h"
#include "..\..\ImGui\imgui_internal.h"
#include "..\..\ImGui\imstb_rectpack.h"
#include "..\..\ImGui\imstb_textedit.h"
#include "..\..\ImGui\imstb_truetype.h"

#include "..\imgui_components.h"
namespace imgui_components {
    bool ToggleButton(
        const char* label,
        bool* v,
        bool        disabled,
        ImVec2      track_size)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* dl = ImGui::GetWindowDrawList();

        ImGuiID id = ImGui::GetID(label);

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImRect bb(pos, ImVec2(pos.x + track_size.x, pos.y + track_size.y));

        ImGui::ItemSize(track_size);

        if (!ImGui::ItemAdd(bb, id))
            return false;

        bool hovered = false;
        bool held = false;

        bool clicked = false;

        if (!disabled)
        {
            clicked = ImGui::ButtonBehavior(
                bb,
                id,
                &hovered,
                &held
            );

            if (clicked)
                *v = !*v;
        }

        static std::unordered_map<ImGuiID, float> anim_map;
        float& anim = anim_map[id];

        float target = *v ? 1.0f : 0.0f;
        anim += (target - anim) * ImMin(1.0f, io.DeltaTime * 18.0f);

        ImVec4 col_off(0, 0, 0, 0);
        //ImVec4 col_on(0.82f, 0.02f, 0.05f, 1.0f);

        ImVec4 track_col = ImLerp(col_off, theme.focus_color, anim);

        if (disabled)
            track_col.w = 0.4f;

        float rounding = track_size.y * 0.5f;

        ImU32 fill_col = ImGui::ColorConvertFloat4ToU32(track_col);


        dl->AddRect(
            bb.Min,
            bb.Max,
            IM_COL32(80, 80, 90, 180),
            rounding,
            0,
            2.0f
        );
        dl->AddRectFilled(
            bb.Min,
            bb.Max,
            fill_col,
            rounding
        );

        float thumb_r = track_size.y * 0.5f - 3.0f;
        float thumb_d = thumb_r * 2.0f;

        float thumb_x =
            bb.Min.x + 4.0f + thumb_r +
            anim * (track_size.x - thumb_d - 8.0f);

        float thumb_y =
            bb.Min.y + track_size.y * 0.5f;

        ImVec4 circle_col_off(73.0f / 255.0f, 73 / 255.0f, 77 / 255.0f, 1.0f);
        ImVec4 circle_col_on(1, 1, 1, 1.0f);
        ImVec4 circle_col = ImLerp(circle_col_off, circle_col_on, anim);
        ImU32 circle_col_u32 = ImGui::ColorConvertFloat4ToU32(circle_col);
        dl->AddCircleFilled(
            ImVec2(thumb_x, thumb_y),
            thumb_r,
            circle_col_u32
        );

        return clicked;
    }

}