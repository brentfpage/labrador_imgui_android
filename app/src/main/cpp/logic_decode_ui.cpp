#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "librador.h"
#include "custom_imgui.h"
#include "imgui_internal.h"
#include "logic_decode_ui.h"
#include "inputs_ui.h"

float logicDecodeUI::draw_grabber(const char * label)
{
    ImGui::InvisibleButton(label, ImVec2(-1, grabber_height));
    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();
    float hcenter = (p0.x + p1.x)/2.;
    float ycenter = (p0.y + p1.y)/2.;
    float yspan = (p1.y - p0.y)/2.;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddLine(ImVec2(hcenter - ImGui::GetFontSize(), ycenter - yspan/4),ImVec2(hcenter + ImGui::GetFontSize(), ycenter - yspan/4), IM_COL32(120, 120, 160, 255));
    draw_list->AddLine(ImVec2(hcenter - ImGui::GetFontSize(), ycenter + yspan/4),ImVec2(hcenter + ImGui::GetFontSize(), ycenter + yspan/4), IM_COL32(120, 120, 160, 255));
    if (ImGui::IsItemActive()) {
        return ImGui::GetIO().MouseDelta.y;
    } else {
        return 0.0;
    }
}

//#define USB_ON
void logicDecodeUI::print_stream(int id, const char * text, bool *at_bottom, float window_content_width, float ch_console_height)
{
    ImGui::PushID(id);
    if (ImGui::BeginChild("console",ImVec2(window_content_width, ch_console_height ))) {
#ifdef USB_ON
        ImGui::TextWrapped("%s", text);
#else
        static int addchar1 = 0;
        static int addchar2 = 0;
        static char temptext1[5012];
        static char temptext2[5012];
        if(id==1) {
            if((addchar1%6)==0)
            {
                char buf2[3];
                sprintf(buf2, "a%d", id);
                strcat(temptext1,buf2);
                addchar1=0;
            }
            addchar1+=1;
            ImGui::TextWrapped("%s", temptext1);
        } else {
            if((addchar2%6)==0)
            {
                char buf2[3];
                sprintf(buf2, "a%d", id);
                strcat(temptext2,buf2);
                addchar2=0;
            }
            addchar2+=1;
            ImGui::TextWrapped("%s", temptext2);
        }
#endif

        ImGuiContext& g = *ImGui::GetCurrentContext();
        ImGuiWindow* window = g.CurrentWindow;

        ImVec2 mouse_delta = ImGui::GetIO().MouseDelta;
        bool scrolling = ImGui::ScrollWhenDraggingAnywhere(ImVec2(0.0f, -mouse_delta.y), ImGuiMouseButton_Left);
        if(!scrolling && *at_bottom){
            ImGui::SetScrollY(window, ImGui::GetScrollMaxY());
        }
        if (ImGui::GetScrollMaxY() == window->Scroll.y)
            *at_bottom=true;
        else
            *at_bottom=false;

        ImGui::EndChild();
    }
    LOGW("ch_c%d_h%.2f",id,ch_console_height);
    ImGui::PopID();
}

void logicDecodeUI::draw_console(float window_content_width)
{
    float y_avail = ImGui::GetContentRegionAvail().y;
    for(int i: {0,1}) {
        ch_console_height[i] *= both_ch_uart_settings[i].decode_on;
    }
    for(int i:{1,0}) {
        if(both_ch_uart_settings[i].decode_on) {
            ch_console_height[i] = fmin(ch_console_height[i], y_avail - ch_console_height[(i+1)%2] - grabber_height * (1 + both_ch_uart_settings[(i+1)%2].decode_on) - 4);
            ch_console_height[i] = fmax(ch_console_height[i], 2 * grabber_height);
        } 
    }
    if(protocol_sel == Protocol::UART) {
        if(both_ch_uart_settings[0].decode_on)
        {
            print_stream(1,librador_get_uart_string(1), &uart_ch_console_at_bottom[0], window_content_width, ch_console_height[0]);
        }
        float next_ch1_height = ch_console_height[1];
        if(both_ch_uart_settings[0].decode_on && both_ch_uart_settings[1].decode_on)
        {
            float console_sep_delta = draw_grabber("chA_chB_splitter");
            console_sep_delta = fmin(console_sep_delta, (ch_console_height[1] - 2 * grabber_height));
            console_sep_delta = fmax(console_sep_delta, -(ch_console_height[0] - 2 * grabber_height));
            next_ch1_height -= console_sep_delta;
            ch_console_height[0] += console_sep_delta;
        }
        if(both_ch_uart_settings[1].decode_on)
        {
            print_stream(2, librador_get_uart_string(2), &uart_ch_console_at_bottom[1], window_content_width, ch_console_height[1]);
            ch_console_height[1] = next_ch1_height;
        }
    } else if(protocol_sel == Protocol::I2C) {
            print_stream(3, librador_get_i2c_string(), &i2c_console_at_bottom, window_content_width, ch_console_height[0]);
    }
    float console_height_delta = draw_grabber("plot_console_splitter");
    if(both_ch_uart_settings[1].decode_on) {
        ch_console_height[1] += console_height_delta;
    } else if (both_ch_uart_settings[0].decode_on) {
        ch_console_height[0] += console_height_delta;
    }
}

bool logicDecodeUI::decoding_on()
{
    return both_ch_uart_settings[0].decode_on || both_ch_uart_settings[1].decode_on || protocol_sel == Protocol::I2C;
}

void logicDecodeUI::draw(float width_pixels, inputsUI* inputs_ui)
{
    ImGui::BeginGroup();
    standard_header(width_pixels);
    if(!is_expanded)
    {
        ImGui::EndGroup();
        return;
    }

    bool logic_enable[2];
    if(inputs_ui->scopelogic_mode()) {
        logic_enable[0] = false;
        logic_enable[1] = true;
    } else {
        memcpy(logic_enable, inputs_ui->logic_enable, 2 * sizeof(bool));
    }
    bool uart_changed = false;
    bool i2c_changed = false;
    bool uart_allowed = logic_enable[0] || logic_enable[1];
    bool i2c_allowed = logic_enable[0] && logic_enable[1] && !both_ch_uart_settings[0].decode_on && !both_ch_uart_settings[1].decode_on;

    ImGuiStyle& style = ImGui::GetStyle();

    ImGui::BeginDisabled(!(logic_enable[0] || logic_enable[1])); //covers nearly entire fn.

    bool* changed[2] = {&uart_changed, &i2c_changed};
    Protocol prots[2] = {Protocol::UART, Protocol::I2C};
    const char * labels[2] = {"UART", "I2C"};

    bool open_ch_serial_settings = false;
    char chAB[2] = {'A', 'B'};

//     ImVec2 saved_pos = ImGui::GetCursorScreenPos();
//     ImGui::SetCursorScreenPos(saved_pos);
    ImGui::BeginGroup(); // for bounding rect
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,{0.f,0.f});
    ImGui::Dummy(ImVec2(width_pixels,0.f));
    ImGui::PopStyleVar();
    ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2((width_pixels - ImGui::CalcTextSize("UART").x)/2.,style.FramePadding.y));
    ImGui::Text("UART");
    ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2( (width_pixels - ImGui::CalcTextSize("CH ACH B").x - style.ItemSpacing.x - 4 * style.FramePadding.x)/2., 0.f ));
    for (int ch: {1,2})
    {
        ImGui::BeginDisabled(!logic_enable[ch-1] || !(protocol_sel==Protocol::UART));
        char buf[20];
        sprintf(buf,"CH %c##serial_decode",chAB[ch-1]);
        if(ImGui::Button(buf)) {
            open_ch_serial_settings = true;
            ch_sel = ch;
        }
        ImGui::EndDisabled(); 
        ImGui::SameLine();
    }
    ImGui::NewLine();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddLine(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImVec2(width_pixels,0.f), IM_COL32(90, 90, 120, 255));
    
    ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2( (width_pixels - ImGui::CalcTextSize("I2C").x - style.ItemInnerSpacing.x - CHECKBOX_SIZE)/2., style.FramePadding.y ));
    ImGui::BeginDisabled(!i2c_allowed);
    ImGui::Checkbox("I2C", (bool *) &protocol_sel);
    ImGui::EndDisabled();

    ImGui::EndGroup();
    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax() + ImVec2(0.f,style.FramePadding.y);
    draw_list->AddRect(p0, p1, IM_COL32(90, 90, 120, 255));
    ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0.f,style.FramePadding.y - style.ItemSpacing.y));
    ImGui::Dummy({0.f,0.f}); // prevents issue with this draw() command affecting the vertical alignment of whatever ui element comes after it
    ImGui::EndGroup();


//     if (ImGui::BeginTable("logic_settings_table", 2, ImGuiTableFlags_SizingStretchProp|ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH , ImVec2(width_pixels, 0.f)) )
//     {
//         ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 0.5f);
//         ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 0.5f);
//         ImGui::TableNextRow();
//         for(int j : {1,2}) {
//             ImGui::TableNextColumn();
//             ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2( (ImGui::GetContentRegionAvail().x - CHECKBOX_SIZE - ImGui::CalcTextSize(labels[j-1]).x - style.ItemInnerSpacing.x)/2., 0.f ));
//             ImGui::BeginDisabled(!allowed[j-1]);
//             if(ImGui::custom_RadioButton(labels[j-1], (int *) &protocol_sel, j))
//                 *changed[j-1] = true;
//             ImGui::EndDisabled();
//             if(!allowed[j-1] && protocol_sel==prots[j-1])
//             {
//                 *changed[j-1] = true;
//                 protocol_sel = Protocol::None;
//             }
//         }
//         ImGui::TableNextRow();
//         ImGui::TableNextColumn();
//         ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2( (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("CH ACH B").x - style.ItemSpacing.x - 4 * style.FramePadding.x)/2., 0.f ));
//         for (int ch: {1,2})
//         {
//             ImGui::BeginDisabled(!logic_enable[ch-1] || !(protocol_sel==Protocol::UART));
//             char buf[20];
//             sprintf(buf,"CH %c##serial_decode",chAB[ch-1]);
//             if(ImGui::Button(buf)) {
//                 open_ch_serial_settings = true;
//                 ch_sel = ch;
//             }
//             ImGui::EndDisabled(); 
//             ImGui::SameLine();
//         }
//         ImGui::EndTable();
//     }
//     ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0.f, -style.ItemSpacing.y));
// 
//     for (int ch : {1,2})
//     {
//         if((!logic_enable[ch-1] || !(protocol_sel==Protocol::UART)) && both_ch_uart_settings[ch-1].decode_on) 
//         {
//             uart_changed = true;
//             both_ch_uart_settings[ch-1].decode_on = false; 
//         }
//     }

    if(open_ch_serial_settings)
    {
        curr_ch_uart_settings = &both_ch_uart_settings[ch_sel-1];
        ImGui::OpenPopup("ch_serial_settings");
    }

    if(ImGui::BeginPopup("ch_serial_settings")) {
        ImGui::Text("CH%c serial settings", chAB[ch_sel-1]);
        ImGui::Separator();
        if(ImGui::Checkbox("Enable decoding", &curr_ch_uart_settings->decode_on))
        {
            uart_changed=true;
            if (curr_ch_uart_settings->decode_on) {
                ch_console_height[ch_sel-1] = init_console_height_per_ch - grabber_height;
            } else {
                ch_console_height[ch_sel-1] = 0.f;
            }
        }
        const char * uart_options_labels[2] = {"Baud Rate", "Parity"};
        const char ** uart_options_sublabels[2] = {baud_rate_labels, parity_labels};
        int sublabels_counts[2] = {IM_COUNTOF(baud_rate_labels), IM_COUNTOF(parity_labels)};
        int * curr_options_sel[2] = {&curr_ch_uart_settings->baud_idx_sel, &curr_ch_uart_settings->parity_idx_sel};
        ImGui::BeginDisabled(!curr_ch_uart_settings->decode_on);
        for(int k: {0,1})
        {
            ImGui::PushItemWidth(ImGui::CalcTextSize(baud_rate_labels[IM_COUNTOF(baud_rate_labels)-1]).x + 2*ImGui::GetFontSize());
            ImGui::PushID(uart_options_labels[k]);
            if(ImGui::Combo("##uart_option_combo", curr_options_sel[k], uart_options_sublabels[k], sublabels_counts[k])) uart_changed=true;
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::Text("%s",uart_options_labels[k]);
        }
        ImGui::EndDisabled();
        ImGui::EndPopup();
    }
    ImGui::EndDisabled(); //!logic_enable[0] && !logic_enable[1]);


    if(uart_changed)
        librador_set_uart_decode_settings(ch_sel, 
                (UartSettings)
                {.decode_on=curr_ch_uart_settings->decode_on, .baudRate=static_cast<double>(baud_rates[curr_ch_uart_settings->baud_idx_sel]), .parity=parities[curr_ch_uart_settings->parity_idx_sel]});
    if(i2c_changed)
    {
        librador_set_i2c_is_decoding(protocol_sel == Protocol::I2C);
        if(protocol_sel == Protocol::I2C)
            ch_console_height[0] = init_console_height_per_ch - grabber_height;
    }
}

int logicDecodeUI::get_height()
{
    ImGuiStyle& style = ImGui::GetStyle();
    int calc_height = 2 * style.ItemSpacing.y + ImGui::GetFontSize() + \
                      style.FramePadding.y + ImGui::GetFontSize() + style.ItemSpacing.y + \
                      2 * style.FramePadding.y + ImGui::GetFontSize() + 2 * style.ItemSpacing.y + \
                      3 * style.FramePadding.y + ImGui::GetFontSize();
    return calc_height;
}
