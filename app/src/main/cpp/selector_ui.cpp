#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "selector_ui.h"

void selectorUI::draw_popup()
{
    if(ImGui::BeginPopup("config_settings")) {
        ImGui::Text("Select tiles");
        ImGui::Separator();
        for (int i=0; i< n_tiles; i++) {
            if(ImGui::Checkbox(tiles[i]->name, &tiles[i]->is_visible)) {
                if(tiles[i]->is_visible)
                {
                    tiles[i]->is_expanded = true; // so that it opens up in an expanded state if it is opened again
                }
            }


        }
        ImGui::EndPopup();
    }
}

int selectorUI::get_height()
{
    ImGuiStyle& style = ImGui::GetStyle();
    return CHECKBOX_SIZE * n_tiles + style.ItemSpacing.y * (n_tiles - 1) + style.WindowPadding.y*2 + 2 * style.ItemSpacing.y + style.SeparatorSize + ImGui::GetFontSize();
}
