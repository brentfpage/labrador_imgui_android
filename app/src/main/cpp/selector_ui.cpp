#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "selector_ui.h"

void selectorUI::draw(float width_pixels, inputsUI* inputs_ui)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    bool need_pop = false;
    ImGui::PushID("selector ui");
    int n_disabled;
    for(int i=0; i < (n_ui_parts-1); i++) {
        if(!ui_parts[i]->is_expanded) {
            n_disabled++;
            if(ImGui::Button(ui_parts[i]->short_name)) {
                ui_parts[i]->is_expanded = true;
                n_disabled--;
            }
        }
        if(width == UI_part::Width::duplex) {
            if(((i%2)==0) && (i<(n_ui_parts-2))) {
                int free_space = width_pixels - ImGui::CalcTextSize(ui_parts[i]->short_name).x - ImGui::CalcTextSize(ui_parts[i+1]->short_name).x - 2 * CHECKBOX_SIZE - 2 * style.ItemInnerSpacing.x - style.FramePadding.x * 2;
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,ImVec2(free_space,0.f));
                need_pop = true;
                ImGui::SameLine();
            } else if (need_pop) {
                ImGui::PopStyleVar();
                need_pop = false;
            }
        }
    }
    n_lines = n_disabled / ((width == UI_part::Width::duplex) + 1);
    ImGui::PopID();
    ImGui::EndGroup();
}

void selectorUI::draw_popup()
{
    if(ImGui::BeginPopup("config_settings")) {
        ImGui::Text("Select widgets");
        ImGui::Separator();
        for (int i=0; i< n_ui_parts; i++) {
            if(ImGui::Checkbox(ui_parts[i]->name, &ui_parts[i]->is_visible)) {
                if(ui_parts[i]->is_visible)
                {
                    ui_parts[i]->is_expanded = true; // so that it opens up in an expanded state if it is opened again
                }
            }


        }
        ImGui::EndPopup();
    }
}

int selectorUI::get_height()
{
    ImGuiStyle& style = ImGui::GetStyle();
    return CHECKBOX_SIZE * n_lines + style.ItemSpacing.y * (n_lines - 1);
}
