#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "librador.h"
#include "custom_imgui.h"
#include <chrono>
#include "imgui_internal.h"
#include "inputs_ui.h"

ImVec2 center_text(float col_width, float text_width, ImGuiStyle& style) {
    return ImGui::GetCursorScreenPos() + ImVec2((col_width - text_width)/2. - style.CellPadding.x, 0.0); // for centered text
}

ImVec2 center_checkbox_delta(float full_col_width, ImGuiStyle& style) {
    return ImVec2((full_col_width - CHECKBOX_SIZE)/2. - style.CellPadding.x, 0.0); // for centered checkbox
}

void draw_rules(ImVec2 p0, double row_height, double header_row_height, double col_width)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec2 row1_col1_pos = p0 + ImVec2(col_width, header_row_height);
    ImVec2 row1_col2_pos = row1_col1_pos + ImVec2(col_width,0);
    ImVec2 row3_col2_pos = row1_col1_pos + ImVec2(col_width,2*row_height);
    ImVec2 row4_col1_pos = row1_col1_pos + ImVec2(0.,3*row_height);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddLine(row1_col1_pos, row4_col1_pos, IM_COL32(120, 120, 160, 255));
    draw_list->AddLine(row1_col2_pos, row3_col2_pos, IM_COL32(120, 120, 160, 255));
}

void inputsUI::draw()
{
    ImGui::Text("Inputs");
    bool mode_update = false;
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(style.CellPadding.x, style.CellPadding.y * 2));// if this line is active, make sure that the line that resets CellPadding at the end of this function is active as well
    float header_row_height = ImGui::GetFontSize() + style.CellPadding.y*2;
    float row_height = (ImGui::GetFontSize() + (style.FramePadding.y + style.CellPadding.y)*2);
    float col_width;
    if (ImGui::BeginTable("scope_mode", 3, ImGuiTableFlags_SizingStretchProp|ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg , ImVec2(ImGui::GetContentRegionAvail().x,0.f)))
    {
        ImGui::TableNextRow();
        int i = 0;
        for(const char * ch_header : {"CH1","CH2"})
        {
            ImGui::TableNextColumn();
            ImGui::SetCursorScreenPos(center_text(ImGui::GetColumnWidth() + 2*style.CellPadding.x, ImGui::CalcTextSize(ch_header).x, style));
            ImGui::Text("%s", ch_header);
            col_width = ImGui::GetColumnWidth();
            i+=1;
            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImGuiCol_TableHeaderBg) );
        }

        bool scope_checkbox_enable[2] = {
            !logic_enable[1],
            scope_enable[0] && !(logic_enable[0]) && !scope750
        };
        bool logic_checkbox_enable[2] = {
            !scope_enable[1] && !scope750,
            logic_enable[0] && !scope_enable[0]
        };

        bool checkbox_enable[2][2] = {
            {scope_checkbox_enable[0], scope_checkbox_enable[1]},
            {logic_checkbox_enable[0], logic_checkbox_enable[1]}
        };

        bool* checkbox_bool[2][2] = {
            {&scope_enable[0], &scope_enable[1]},
            {&logic_enable[0], &logic_enable[1]}
        };
        const char * checkbox_label_base[2] = {"##enable_scope", "##enable_logic"};
        const char * glyphs[2] = {"\xee\xa4\x81","\xee\xa4\x80"};// includes custom glyphs defined in /font/waveform-glyphs3.ttf
        const char * checkbox_label_suffix[2] = {"_ch1","_ch2"};
        char full_checkbox_label[32]{};
        for(int j=0; j<2; j++)
        {
            ImGui::TableNextRow();
            for(int ch : {1,2} )
            {
                ImGui::TableNextColumn();
                strcpy(full_checkbox_label, checkbox_label_base[j]);
                strcat(full_checkbox_label, checkbox_label_suffix[ch-1]);
                ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + center_checkbox_delta(ImGui::GetColumnWidth() + 2*style.CellPadding.x, style));
                ImGui::BeginDisabled(!(checkbox_enable[j][ch-1]));
                if((ImGui::custom_Checkbox(full_checkbox_label, checkbox_bool[j][ch-1]))||(*checkbox_bool[j][ch-1] && !checkbox_enable[j][ch-1])) {
                    if(j!=2) { 
                        mode_update = true;
                        changed = true;
                    }
                }
                *(checkbox_bool[j][ch-1]) &= checkbox_enable[j][ch-1];

                ImGui::EndDisabled();
            }

            ImGui::TableNextColumn();
            ImGui::SetCursorScreenPos(center_text(ImGui::GetColumnWidth() + 2*style.CellPadding.x, ImGui::CalcTextSize(glyphs[j]).x, style));
            ImGui::Text("%s",glyphs[j]); 
        }
        ImGui::TableNextRow(0, row_height);
        ImGui::TableNextRow(0, row_height);
        ImGui::EndTable();
    }
    ImVec2 saved_pos = ImGui::GetCursorScreenPos();
    ImVec2 p0 = ImGui::GetItemRectMin();
    draw_rules(p0, row_height, header_row_height, col_width + 2* style.CellPadding.x );
    ImVec2 row3_pos = p0 + ImVec2(style.CellPadding.x,header_row_height + 2*row_height + style.CellPadding.y);;
    ImVec2 row4_pos = row3_pos + ImVec2(0.f,row_height);

    bool* checkbox_bool[2] = {&scope750, &xy};
    bool checkbox_enable[2] = {scope_enable[0] && !(scope_enable[1] || logic_enable[0]), (scope_enable[0] && scope_enable[1])};
    const char* print_labels[2] = {" 750 kHz", "XY"};
    const char* internal_labels[2] = {"##750 kHz","##XY Mode"};
    ImVec2 positions[2] = 
    {
        row3_pos + center_checkbox_delta(col_width + 2*style.CellPadding.x, style),
        row4_pos + center_checkbox_delta(2 * col_width + 4*style.CellPadding.x, style)
    };

    for(int i = 0; i < 2; i++)
    {
        ImGui::SetCursorScreenPos(positions[i]);
        ImGui::BeginDisabled(!checkbox_enable[i]);
        if((ImGui::custom_Checkbox(internal_labels[i], checkbox_bool[i])) || (*checkbox_bool[i] && !checkbox_enable[i])) {
            changed = true;
            if(i==0)
                mode_update = true;
        }
        *checkbox_bool[i] &= checkbox_enable[i];
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::Text("%s",print_labels[i]);
    }
    ImGui::PopStyleVar();
    ImGui::SetCursorScreenPos(saved_pos);

    if (mode_update)
        update_device_mode();
}

bool inputsUI::ch_enabled(int ch)
{
    // for argument 'ch': 1==ChA ; 2==ChB, where ChA/B refer to plotted lines.  Except in ScopeLogic mode, ChA is scope or logic Ch1 and ChB is scope or logic Ch2.
    if(mode == Mode::ScopeLogic)
        return true;
    else
        return scope_enable[ch-1]||logic_enable[ch-1];
    return false;
}

bool inputsUI::changed_since_last()
{
    bool changed_temp = changed;
    changed = false;
    return changed_temp;
}

bool inputsUI::logic_on()
{
    return (logic_enable[0] || logic_enable[1]);
}

bool inputsUI::scopelogic_mode()
{
    return mode == Mode::ScopeLogic;
}

void inputsUI::update_device_mode()
{
    if(scope_enable[0] && !scope_enable[1] && !logic_enable[0] && !scope750)
        mode = Mode::Ch1Scope;
    else if (scope_enable[0] && logic_enable[0])
        mode = Mode::ScopeLogic;
    else if (scope_enable[0] && scope_enable[1])
        mode = Mode::ScopeScope;
    else if (logic_enable[0] && !logic_enable[1] && !scope_enable[0])
        mode = Mode::Ch1Logic;
    else if (logic_enable[0] && logic_enable[1])
        mode = Mode::LogicLogic;
    else if (!(scope_enable[0] || scope_enable[1] || logic_enable[0] || logic_enable[1]))
        mode = Mode::None;
    else if (scope750)
        mode = Mode::Scope750;

    librador_set_device_mode((int) mode);
}

