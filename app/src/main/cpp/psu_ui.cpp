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

    const float psu_button_width = style.FramePadding.x*2 + ImGui::CalcTextSize("PSU").x;
    float close_button_width = ImGui::GetFontSize() + style.FramePadding.x+ style.ItemSpacing.x;
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemInnerSpacing.x,style.ItemSpacing.y));
    ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + style.CellPadding);
    const float x_padding = style.CellPadding.x * 2 + style.ItemSpacing.x;
    ImGui::PushItemWidth(width - psu_button_width - x_padding - 1 - close_button_width);  // -1 to give space for bounding rect
    ImGui::BeginGroup();
    button_common("PSU", "##psu_slider", ImVec2(psu_button_width,0.f), style);
    ImGui::PopStyleVar();
    ImGui::SameLine();
    if(ImGui::custom_SliderFloat("##psu_slider", "V", &psu, 4.5f, 12.0f, "%.1f V", ImGuiSliderFlags_ClampOnInput) || ImGui::IsItemDeactivatedAfterEdit()) {
        need_usb_send = true;
    }
    ImGui::EndGroup();

    ImVec2 p0 = ImGui::GetItemRectMin() - style.CellPadding;
    ImVec2 p1 = ImGui::GetItemRectMax() + style.CellPadding;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRect(p0, p1, IM_COL32(90, 90, 120, 255),0,0,2);

    ImGui::SameLine();
    if(ImGui::CloseButton(ImGui::GetID("psu_close"), ImGui::GetCursorScreenPos() + ImVec2(0.f, style.CellPadding.y))) {
        enable = false;
    }
    ImGui::Dummy({0.f,0.f});



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
    return 4 * style.FramePadding.y + ImGui::GetFontSize();
}


