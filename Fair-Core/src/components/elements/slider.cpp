#include "..\..\ImGui\imconfig.h"
#include "..\..\ImGui\imgui_impl_dx9.h"
#include "..\..\ImGui\imgui_impl_win32.h"
#include "..\..\ImGui\imgui_internal.h"
#include "..\..\ImGui\imstb_rectpack.h"
#include "..\..\ImGui\imstb_textedit.h"
#include "..\..\ImGui\imstb_truetype.h"
#include "..\..\ImGui\imgui.h"
#include "..\imgui_components.h"
#include <iostream>
namespace imgui_components {

	static constexpr float ROW_H = 22.f;
	static constexpr float TRACK_H = 2.f;
	static constexpr float THUMB_R = 8.f;
	static constexpr float LABEL_W = 70.f;
	static constexpr float VAL_W = 42.f;
    bool _SliderFloat(
        const char* label,
        float* v,
        float v_min,
        float v_max,
        float width,
        int decimals, ImU32 cutout_col,
        bool disabled)
    {
        ImGuiStyle& sty = ImGui::GetStyle();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImGuiIO& io = ImGui::GetIO();

        if (width <= 0.f)
            width = ImGui::GetContentRegionAvail().x;

        ImVec2 pos = ImGui::GetCursorScreenPos();

        ImGuiID id = ImGui::GetID(label);

        ImRect bb(
            pos,
            ImVec2(pos.x + width, pos.y + ROW_H)
        );

        ImGui::ItemSize(bb.GetSize());

        if (!ImGui::ItemAdd(bb, id))
            return false;

        float range = v_max - v_min;

        float t = 0.0f;
        if (range != 0.0f)
            t = (*v - v_min) / range;

        t = ImClamp(t, 0.0f, 1.0f);

        float track_x0 = pos.x + THUMB_R;
        float track_x1 = pos.x + width - THUMB_R;
        float track_w = track_x1 - track_x0;

        float track_cy = pos.y + ROW_H * 0.5f;

        ImRect interact_bb(
            ImVec2(pos.x, pos.y),
            ImVec2(pos.x + width, pos.y + ROW_H)
        );

        bool changed = false;
        bool hovered = false;
        bool held = false;

        if (!disabled)
        {
            ImGui::ButtonBehavior(
                interact_bb,
                id,
                &hovered,
                &held,
                ImGuiButtonFlags_PressedOnClick |
                ImGuiButtonFlags_AllowOverlap
            );

            if (held)
            {
                float new_t = ImClamp(
                    (io.MousePos.x - track_x0) / track_w,
                    0.0f,
                    1.0f
                );

                float new_v = v_min + new_t * range;

                if (new_v != *v)
                {
                    *v = new_v;
                    changed = true;
                }

                t = new_t;
            }
        }


        static std::unordered_map<ImGuiID, float> anim_map;

        auto it = anim_map.find(id);
        if (it == anim_map.end())
        {
            anim_map[id] = t;
        }

        float& anim_t = anim_map[id];

        //animation
        float anim_speed = 18.0f;
        anim_t += (t - anim_t) * ImMin(1.0f, io.DeltaTime * anim_speed);

        anim_t = ImClamp(anim_t, 0.0f, 1.0f);

        float thumb_x = track_x0 + anim_t * track_w;


        ImU32 col_track = disabled
            ? IM_COL32(50, 50, 60, 255)
            : IM_COL32(58, 58, 69, 255);

        ImU32 col_fill = disabled
            ? IM_COL32(80, 80, 90, 255)

            : ToU32(theme.focus_color);

        ImU32 col_thumb = col_fill;


        dl->AddRectFilled(
            ImVec2(track_x0, track_cy - TRACK_H * 0.5f),
            ImVec2(track_x1, track_cy + TRACK_H * 0.5f),
            col_track,
            TRACK_H * 0.5f
        );


        dl->AddRectFilled(
            ImVec2(track_x0, track_cy - TRACK_H * 0.5f),
            ImVec2(thumb_x, track_cy + TRACK_H * 0.5f),
            col_fill,
            TRACK_H * 0.5f
        );

        float thumb_r = held ? THUMB_R + 1.5f : hovered ? THUMB_R + 0.8f : THUMB_R;
        ImVec2 thumb_center(thumb_x, track_cy);
        if (cutout_col != NULL) {
            float cutout_w = 3.0f;
            dl->AddCircleFilled(
                thumb_center,
                thumb_r + cutout_w,
                cutout_col
            );
        }
       
        dl->AddCircleFilled(
            thumb_center,
            thumb_r,
            col_thumb
        );

        return changed;
    }
    bool SliderFloat(
        const char* label,
        float* v,
        float v_min,
        float v_max,
        float width,
        int decimals,
        bool disabled)
    {
        return _SliderFloat(label, v, v_min, v_max, width, decimals, NULL, disabled);
    }
    bool SliderFloat_cutout(
        const char* label,
        float* v,
        float v_min,
        float v_max,
        float width,
        int decimals, 
        ImU32 cutout_col
    )
    {
        return _SliderFloat(label, v, v_min, v_max, width, decimals, cutout_col,  false);
    }

}