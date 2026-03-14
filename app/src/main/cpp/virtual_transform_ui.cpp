#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "librador.h"
#include "custom_imgui.h"
#include <chrono>
#include "imgui_internal.h"
#include "virtual_transform_ui.h"


void virtualTransformUI::draw()
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
    for (int ch : {1,2}) {
        both_ch_settings[ch-1].is_paused = librador_get_paused(ch); // could have been set to true by a singleshot trigger
    }
//                 if(xy && j==2) // sync ch1 and ch2 pause states in xy mode
//                     *(checkbox_bool[j] + (i+1)%2) = *(checkbox_bool[j] + i);
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::Text("Virtual Transforms");
    ImGui::BeginGroup();
    if(ImGui::BeginTable("helper1",2, ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_NoHostExtendX, ImVec2(0., 0.))) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::RadioButton("CH A", &ch_sel, 1);
        ImGui::TableNextColumn();
        ImGui::RadioButton("CH B", &ch_sel, 2); 
        ImGui::EndTable();
    }

    const float offset_button_width = style.FramePadding.x*2 + ImGui::CalcTextSize("Offset").x;
//     ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - offset_button_width - style.ItemInnerSpacing.x);
    if(ImGui::BeginTable("helper2",2, ImGuiTableFlags_SizingStretchProp|ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg , ImVec2(ImGui::GetContentRegionAvail().x, 0.))) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 0.75f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 0.25f);
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-1); 
        ImGui::custom_SliderFloat("##offset", "V", &curr_ch_settings->offset, -20.f, 20.f, "%.1f V", ImGuiSliderFlags_ClampOnInput);
        ImGui::TableNextColumn();
        ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() - ImVec2(style.CellPadding.x,0.f));
        button_common("Offset", "##offset", ImVec2(0.f,0.f), &offset_button_press_start, style);
        ImGui::EndTable();
    }
    if(ImGui::BeginTable("helper3", 3, ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 0.55f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 0.225);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 0.225);
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Combo("Gain", &curr_ch_settings->gain_sel, gain_labels, IM_COUNTOF(gain_labels));
        ImGui::TableNextColumn();
        ImGui::Checkbox("AC", &curr_ch_settings->is_ac);
        ImGui::TableNextColumn();
        ImGui::Checkbox("||", &curr_ch_settings->is_paused);
        ImGui::EndTable();
    }
    ImGui::EndGroup();
    curr_ch_settings = &both_ch_settings[ch_sel-1];
    const ImVec2 p0 = ImGui::GetItemRectMin() - ImVec2(0.f,style.FramePadding.x/2.);
    const ImVec2 p1 = ImGui::GetItemRectMax() + ImVec2(style.FramePadding.x/2.,style.FramePadding.x/2.);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
//     draw_list->AddRect(p0, p1, IM_COL32(90, 90, 120, 255));
    librador_set_virtual_transform_settings(ch_sel, 
            (o1buffer::virtual_transform_settings) 
            {.offset=curr_ch_settings->offset, .gain=gains[curr_ch_settings->gain_sel], .is_ac=curr_ch_settings->is_ac, .is_paused=curr_ch_settings->is_paused}); 
    ImGui::PopStyleVar(); //itemspacing
}
