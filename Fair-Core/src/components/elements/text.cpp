#include "..\..\ImGui\imconfig.h"
#include "..\..\ImGui\imgui_impl_dx9.h"
#include "..\..\ImGui\imgui_impl_win32.h"
#include "..\..\ImGui\imgui_internal.h"
#include "..\..\ImGui\imstb_rectpack.h"
#include "..\..\ImGui\imstb_textedit.h"
#include "..\..\ImGui\imstb_truetype.h"

#include "..\imgui_components.h"
bool imgui_components::TextButton(
    const char* id,
    const char* text,
    ImFont* font,
    float font_size,
    ImVec2 size,
    ImU32 normal_col,
    ImU32 hover_col,
    bool disabled)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    if (window->SkipItems)
        return false;

    ImDrawList* draw = ImGui::GetWindowDrawList();

    if (font == nullptr)
        font = ImGui::GetFont();

    if (font_size <= 0.0f)
        font_size = font->FontSize;

    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImVec2 text_size = font->CalcTextSizeA(
        font_size,
        FLT_MAX,
        0.0f,
        text
    );

    if (size.x <= 0.0f)
        size.x = text_size.x;

    if (size.y <= 0.0f)
        size.y = text_size.y;

    ImGui::InvisibleButton(id, size);

    bool hovered = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);

    if (disabled)
    {
        hovered = false;
        clicked = false;
    }

    ImU32 col = disabled
        ? IM_COL32(120, 120, 130, 120)
        : hovered ? hover_col : normal_col;

    ImVec2 text_pos = ImVec2(
        pos.x + (size.x - text_size.x) * 0.5f,
        pos.y + (size.y - text_size.y) * 0.5f
    );

    draw->AddText(
        font,
        font_size,
        text_pos,
        col,
        text
    );

    if (hovered)
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    return clicked;
}