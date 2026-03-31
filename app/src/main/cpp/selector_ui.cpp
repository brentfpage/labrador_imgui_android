#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "selector_ui.h"

void selectorUI::init_step_2(UI_part** ui_parts_in, int n_ui_parts_in)
{
    ui_parts = ui_parts_in;
    n_ui_parts = n_ui_parts_in;
}
void selectorUI::draw(float width_pixels, inputsUI* inputs_ui)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::BeginGroup();
    bool need_pop = false;
    ImGui::PushID("selector ui");
    for(int i=0; i < (n_ui_parts-1); i++) {
        char buf[128];
        ImGui::Checkbox(ui_parts[i]->short_name, &ui_parts[i]->enable);
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
    ImGui::PopID();
    ImGui::EndGroup();
}

void selectorUI::draw_popup()
{
    if(ImGui::BeginPopup("config_settings")) {
        ImGui::Text("Select widgets");
        ImGui::Separator();
        for (int i=0; i< n_ui_parts; i++) {
            ImGui::Checkbox(ui_parts[i]->name, &ui_parts[i]->enable);
        }
        ImGui::EndPopup();
    }
}

int selectorUI::get_height()
{
    ImGuiStyle& style = ImGui::GetStyle();
    return CHECKBOX_SIZE * n_lines + style.ItemSpacing.y * (n_lines - 1);
}
