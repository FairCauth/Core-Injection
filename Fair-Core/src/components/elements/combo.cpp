#include "..\..\ImGui\imconfig.h"
#include "..\..\ImGui\imgui_impl_dx9.h"
#include "..\..\ImGui\imgui_impl_win32.h"
#include "..\..\ImGui\imgui_internal.h"
#include "..\..\ImGui\imstb_rectpack.h"
#include "..\..\ImGui\imstb_textedit.h"
#include "..\..\ImGui\imstb_truetype.h"
#include "..\..\ImGui\imgui.h"
#include "..\imgui_components.h"

#include <unordered_map>
#include <vector>
#include <cmath>

namespace imgui_components {

    static constexpr float ROW_H = 34.f;
    static constexpr float BORDER_W = 1.f;
    static constexpr float ANIM_SPEED = 14.f;
    static constexpr float MAX_ROWS = 56.f;

    struct DdState {
        float open_t = 0.f;
    };

    static std::unordered_map<ImGuiID, DdState> s_states;

    bool Dropdown(
        const char* label,
        const std::vector<DropdownItem>& items,
        int* current,
        const char* preview,
        float width,
        bool disabled)
    {
        ImGuiID id = ImGui::GetID(label);
        DdState& st = s_states[id];

        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImGuiStyle& sty = ImGui::GetStyle();


        ImVec2 btn_pos = ImGui::GetCursorScreenPos();

        ImVec2 btn_min = btn_pos;
        ImVec2 btn_max = ImVec2(
            btn_pos.x + width,
            btn_pos.y + ROW_H
        );

        ImRect btn_bb(btn_min, btn_max);

        ImGui::ItemSize(ImVec2(width, ROW_H));

        if (!ImGui::ItemAdd(btn_bb, id))
            return false;

        bool hov = false;
        bool held = false;
        bool clicked = false;

        if (!disabled)
        {
            clicked = ImGui::ButtonBehavior(
                btn_bb,
                id,
                &hov,
                &held
            );
        }

        bool is_open = ImGui::IsPopupOpen(id, ImGuiPopupFlags_None);

        if (clicked)
        {
            if (is_open)
            {
                ImGui::ClosePopupToLevel(0, true);
                is_open = false;
            }
            else
            {
                ImGui::OpenPopupEx(id);
                is_open = true;
            }
        }


        float target = is_open ? 1.f : 0.f;

        st.open_t += (target - st.open_t)
            * ImMin(1.f, io.DeltaTime * ANIM_SPEED);


        ImVec4 col_idle(0, 0, 0, 0);
        ImVec4 col_act(1, 1, 1, 1.f);

        //st.open_t

        ImVec4 fill_col = ImLerp(
            col_idle,
            col_act,
            ImSaturate((hov) ? 0.4f : 0.f)
        );
        if (disabled)
            fill_col.w = 0.35f;

        //fill_col.w = 0.10f;

        ImU32 fill_u32 = ImGui::ColorConvertFloat4ToU32(fill_col);

        // =========================
        // 4. 绘制按钮
        // =========================

        float rnd = 4.f;

        dl->AddRectFilled(
            btn_min,
            btn_max,
            fill_u32,
            rnd
        );

        dl->AddRect(
            btn_min,
            btn_max,
            IM_COL32(80,80,80,255),
            rnd,
            0,
            1.5f
        );

        // 当前选中文字
        const char* disp =
            (*current >= 0 && *current < (int)items.size())
            ? items[*current].label.c_str()
            : preview;

        ImVec2 text_pos(
            btn_min.x + 10.f,
            btn_min.y + (ROW_H - ImGui::GetTextLineHeight()) * 0.5f
        );

        ImU32 text_col = disabled
            ? IM_COL32(160, 160, 160, 90)
            : ImGui::ColorConvertFloat4ToU32(sty.Colors[ImGuiCol_Text]);

        dl->AddText(
            text_pos,
            text_col,
            disp
        );

        //arrow
        float arr_cx = btn_max.x - 14.f;
        float arr_cy = btn_min.y + ROW_H * 0.5f;

        float angle = st.open_t * IM_PI;

        float cs = cosf(angle);
        float sn = sinf(angle);

        auto rot = [&](ImVec2 p) -> ImVec2 {
            return ImVec2(
                arr_cx + p.x * cs - p.y * sn,
                arr_cy + p.x * sn + p.y * cs
            );
            };

        ImU32 arr_col = disabled
            ? IM_COL32(130, 130, 130, 80)
            : IM_COL32(255, 255, 255, 255);

        dl->AddTriangleFilled(
            rot(ImVec2(-4.f, -2.f)),
            rot(ImVec2(4.f, -2.f)),
            rot(ImVec2(0.f, 3.f)),
            arr_col
        );


        //计算下拉框方向
        bool changed = false;

        float gap = 3.f;

        float visible_rows = ImMin(
            (float)items.size(),
            MAX_ROWS
        );

        float wanted_popup_h =
            visible_rows * ROW_H + 6.f;

        if (wanted_popup_h < ROW_H + 6.f)
            wanted_popup_h = ROW_H + 6.f;

        ImGuiWindow* parent_window = ImGui::GetCurrentWindow();

        float top_limit = parent_window->InnerClipRect.Min.y;
        float bottom_limit = parent_window->InnerClipRect.Max.y;

        float down_y = btn_max.y + gap;
        float up_y = btn_min.y - wanted_popup_h - gap;

        float space_down = bottom_limit - down_y;
        float space_up = btn_min.y - top_limit;

        bool open_up =
            space_down < wanted_popup_h &&
            space_up > space_down;

        float popup_h = wanted_popup_h;

        if (open_up)
        {
            popup_h = ImMin(
                wanted_popup_h,
                space_up - gap
            );

            if (popup_h < ROW_H + 6.f)
                popup_h = ROW_H + 6.f;

            up_y = btn_min.y - popup_h - gap;
        }
        else
        {
            popup_h = ImMin(
                wanted_popup_h,
                space_down
            );

            if (popup_h < ROW_H + 6.f)
                popup_h = ROW_H + 6.f;
        }

        ImVec2 popup_pos = open_up
            ? ImVec2(btn_min.x, up_y)
            : ImVec2(btn_min.x, down_y);


        ImGui::SetNextWindowPos(popup_pos);
        ImGui::SetNextWindowSize(ImVec2(width, popup_h));

        ImGuiWindowFlags popup_flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove;


        if (items.size() <= MAX_ROWS)
        {
            popup_flags |= ImGuiWindowFlags_NoScrollbar;
        }


        if (ImGui::BeginPopupEx(id, popup_flags))
        {
            ImGui::SetCursorPosY(
                ImGui::GetCursorPosY() + 3.f
            );

            for (int i = 0; i < (int)items.size(); ++i)
            {
                const auto& item = items[i];

                if (item.separator_before)
                    ImGui::Separator();

                bool selected = (i == *current);

                if (ImGui::Selectable(
                    item.label.c_str(),
                    selected,
                    ImGuiSelectableFlags_None,
                    ImVec2(0, ROW_H - 8.f)))
                {
                    *current = i;
                    changed = true;
                    ImGui::CloseCurrentPopup();
                }

                if (selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndPopup();
        }

        return changed;
    }
}