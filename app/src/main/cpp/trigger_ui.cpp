#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "librador.h"
#include "custom_imgui.h"
#include <chrono>
#include "imgui_internal.h"
#include "trigger_ui.h"


void triggerUI::draw(const bool scope_enable[2])
{
    for (int ch:{1,2})
    {
        if(!scope_enable[ch-1]) {
            both_ch_trigger_settings[ch-1] = o1buffer::trigger_settings();
        }
    }
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::Text("Trigger");

//     if (ImGui::BeginTable("trigger_helper", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_BordersOuterH, ImVec2(ImGui::GetContentRegionAvail().x, 0.f)) {
    ImGui::BeginGroup();
        ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(style.FramePadding.x/2.,0.f)); // to give space for bounding rect
        ImGui::RadioButton("CH1", &ch_sel, 1); ImGui::SameLine();
        ImGui::RadioButton("CH2", &ch_sel, 2); 
    ImGui::EndGroup();
    curr_ch_trigger_settings = &both_ch_trigger_settings[ch_sel - 1];
    ImVec2 p0 = ImGui::GetItemRectMin() - ImVec2(0.f,style.FramePadding.x/2.);
    ImVec2 p1 = ImGui::GetItemRectMax() + ImVec2(style.FramePadding.x/2.,style.FramePadding.y/2.);
    ImVec2 slider_top_right = p1 + style.FramePadding/2.;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRect(p0, p1, IM_COL32(90, 90, 120, 255));

    ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0., style.FramePadding.y/2.));
    ImGui::BeginDisabled(!scope_enable[ch_sel-1]);
    ImGui::BeginGroup();
        ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(style.FramePadding.x/2.,0.f)); // to give space for bounding rect
        ImGui::custom_RadioButton("Rising", (int *) &curr_ch_trigger_settings->trigger_type, 1);
        ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(style.FramePadding.x/2.,0.f)); // to give space for bounding rect
        ImGui::custom_RadioButton("Falling", (int *) &curr_ch_trigger_settings->trigger_type, 2); 
//     ImGui::EndGroup();
    p1 = ImGui::GetItemRectMax() + ImVec2(style.FramePadding.x/2.,style.FramePadding.y);
    p0 = ImVec2(ImGui::GetItemRectMin().x - style.FramePadding.x/2., p1.y);
    float slider_left = p1.x + style.FramePadding.x/2.;
//     ImGui::BeginGroup();
//     ImGui::Separator();
        ImVec2 saved_pos = ImGui::GetCursorScreenPos();
        ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(style.FramePadding.x,(ImGui::CalcTextSize("S\ns").y - CHECKBOX_SIZE)/2.));
        ImGui::Checkbox("##ss", &curr_ch_trigger_settings->is_single_shot);
        draw_list = ImGui::GetWindowDrawList();
        draw_list->AddLine(p0, p1, IM_COL32(90, 90, 120, 255));
        ImGui::SameLine();
        ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, saved_pos.y));
    ImGui::Text("Single\n shot");
    ImGui::EndGroup();
    p0 = ImGui::GetItemRectMin() - ImVec2(0.f,style.FramePadding.x/2.);
    p1 = ImGui::GetItemRectMax() + ImVec2(style.FramePadding.x/2.,style.FramePadding.y/2.);
    draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRect(p0, p1, IM_COL32(90, 90, 120, 255));

    ImVec2 value_text_size = ImGui::CalcTextSize("-20.0 V", NULL);
    float slider_bottom = ImGui::GetCursorScreenPos().y + ImGui::GetContentRegionAvail().y;//value_text_size.y + style.FramePadding.y * 2;
    ImVec2 value_text_pos = ImVec2(slider_left, slider_bottom) - value_text_size - style.FramePadding - style.FramePadding;
    ImGui::SetCursorScreenPos(value_text_pos - ImVec2(style.FramePadding.x,0.f));
    button_common("##trigger_button", "##trigger_slider", value_text_size + style.FramePadding+ style.FramePadding, &trigger_button_press_start, style);
    ImGui::SetCursorScreenPos(ImVec2(slider_left, slider_top_right.y));

    ImGui::custom_VSliderFloat("##trigger_slider", "V",
            ImVec2(slider_top_right.x - slider_left , slider_bottom - slider_top_right.y),
            &curr_ch_trigger_settings->trigger_level, -20.f, 20.f, "%.1f V", ImGuiSliderFlags_ClampOnInput, value_text_pos  + ImVec2(0.f,style.FramePadding.y), value_text_size);
    ImGui::EndDisabled();

    if(curr_ch_trigger_settings->trigger_type != o1buffer::TriggerType::Disabled)
    {
        both_ch_trigger_settings[ch_sel%2].trigger_type = o1buffer::TriggerType::Disabled;
        librador_set_trigger_settings(ch_sel%2+1, both_ch_trigger_settings[ch_sel%2]);
    }

    librador_set_trigger_settings(ch_sel, both_ch_trigger_settings[ch_sel-1]);
}
