#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "selector_ui.h"
void selectorUI::draw(float width_pixels, inputsUI* inputs_ui)
{
    ImGui::BeginGroup();
    bool need_pop = false;
    for(int i=0; i < (n_ui_parts-1); i++) {
        ImGui::Checkbox(ui_part_short_names[i], &ui_parts_enable[i]);
        if(width == UI_part::Width::duplex) {
            if(((i%2)==0) && (i<(n_ui_parts-2))) {
                int free_space = width_pixels - ImGui::CalcTextSize(ui_part_short_names[i]).x - ImGui::CalcTextSize(ui_part_short_names[i+1]).x - 2 * CHECKBOX_SIZE - 2 * style.ItemInnerSpacing.x - style.FramePadding.x * 2;
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,{free_space});
                need_pop = true;
                ImGui::SameLine();
            } else if (need_pop) {
                ImGui::PopStyleVar();
                need_pop = false;
            }
        }
    }
    ImGui::EndGroup();
}

void selectorUI::draw_popup()
{
    if(ImGui::BeginPopup("config_settings")) {
        ImGui::Text("Select widgets");
        ImGui::Separator();
        for (int i=0; i< sizeof(ui_parts_enable); i++) {
            ImGui::Checkbox(ui_part_names[i], &ui_parts_enable[i]);
        }
        ImGui::EndPopup();
    }
}
