#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "librador.h"
#include "custom_imgui.h"
#include <chrono>
#include "imgui_internal.h"
#include "psu_ui.h"

void psuUI::draw()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + style.FramePadding);
    const float psu_button_width = style.FramePadding.x*2 + ImGui::CalcTextSize("PSU").x;
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - psu_button_width - style.FramePadding.x - style.ItemInnerSpacing.x);
    ImGui::BeginGroup();
    if(ImGui::custom_SliderFloat("##psu_slider", "V", &psu, 4.5f, 12.0f, "%.1f V", ImGuiSliderFlags_ClampOnInput) || ImGui::IsItemDeactivatedAfterEdit()) {
        need_usb_send = true;
    }
    ImGui::SameLine();
    ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(-style.ItemSpacing.x + style.ItemInnerSpacing.x,0.f));
    button_common("PSU", "##psu_slider", ImVec2(0.f,0.f), style);
    ImGui::EndGroup();

    ImVec2 p0 = ImGui::GetItemRectMin() - style.FramePadding;
    ImVec2 p1 = ImGui::GetItemRectMax() + style.FramePadding;
    ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, p1.y));
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRect(p0, p1, IM_COL32(90, 90, 120, 255),0,0,3);
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


