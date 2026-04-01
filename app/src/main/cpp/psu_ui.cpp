#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "librador.h"
#include "custom_imgui.h"
#include <chrono>
#include "imgui_internal.h"
#include "psu_ui.h"

void psuUI::draw(float width, inputsUI* inputs_ui)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0.f,style.ItemSpacing.y)); // combined with lines in main.cpp, effectively folds itemspacing.y into the group covering this ui_tile.  

    const float psu_button_width = style.FramePadding.x*2 + ImGui::CalcTextSize(" PSU ").x;
    float close_button_width = ImGui::GetFontSize() + style.FramePadding.x;
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemInnerSpacing.x,style.ItemSpacing.y));
    ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + style.CellPadding);
    ImGui::BeginGroup(); // for bounding rect
    button_common(" PSU ", "##psu_slider", ImVec2(psu_button_width,0.f), style);
    ImGui::SameLine();
    ImGui::PopStyleVar();
    const float x_padding = style.CellPadding.x * 2 + style.ItemSpacing.x;
    ImGui::PushItemWidth(width - psu_button_width - x_padding - 1 - close_button_width);  // -1 to give space for bounding rect
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {style.ItemSpacing.x,style.CellPadding.y});
    if(ImGui::custom_SliderFloat("##psu_slider", "V", &psu, 4.5f, 12.0f, "%.1f V", ImGuiSliderFlags_ClampOnInput) || ImGui::IsItemDeactivatedAfterEdit()) {
        need_usb_send = true;
    }
    ImGui::SameLine();
    ImVec2 close_button_loc = ImGui::GetCursorScreenPos() + ImVec2(0.f, style.ItemSpacing.y);
    ImGui::EndGroup();
    ImGui::PopStyleVar();

    ImVec2 p0 = ImGui::GetItemRectMin() - style.CellPadding;
    ImVec2 p1 = ImGui::GetItemRectMax() + style.CellPadding;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRect(p0, p1, IM_COL32(90, 90, 120, 255),0,0,2);

    if(ImGui::CloseButton(ImGui::GetID("psu_close"), close_button_loc)) {
        is_expanded = false;
        is_visible = false;
    }

    ImGui::Dummy({0.f,0.f});
    ImGui::EndGroup();
//     ImGui::PopStyleVar();

    if(need_usb_send) {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::milliseconds>(now - last_usb_send) > between_usb_sends_min) {
            usb_send_data();
            last_usb_send = now;
            need_usb_send = false;
        }
    }
}

void psuUI::usb_send_data()
{
    librador_set_power_supply_voltage(psu);
}

int psuUI::get_height()
{
    ImGuiStyle& style = ImGui::GetStyle();
    return 2 * style.CellPadding.y + 2 * style.FramePadding.y + style.ItemSpacing.y + ImGui::GetFontSize();
}


