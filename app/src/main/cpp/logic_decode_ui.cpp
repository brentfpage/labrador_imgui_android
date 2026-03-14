#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "librador.h"
#include "custom_imgui.h"
#include "imgui_internal.h"
#include "logic_decode_ui.h"

float logicDecodeUI::get_console_height(float avail_y)
{
    float cushion = grabber_height * (1 + both_ch_uart_settings[0].decode_on + both_ch_uart_settings[1].decode_on + (protocol_sel == Protocol::I2C));
    console_height = std::max(console_height, cushion);
    console_height = std::min(console_height, avail_y - grabber_height * 2);
    return console_height;
}

void logicDecodeUI::draw_separator(const char * label, float *item_below_height)
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
    if (ImGui::IsItemActive())
    {
        *item_below_height -= ImGui::GetIO().MouseDelta.y;
    }
}

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
    ImGui::PopID();
}

void logicDecodeUI::draw_console(float window_content_width)
{
    draw_separator("plot_console_splitter", &console_height);
    if(protocol_sel == Protocol::UART) {
        if(both_ch_uart_settings[0].decode_on)
        {
            print_stream(1,librador_get_uart_string(1), &uart_ch_console_at_bottom[0], window_content_width, ImGui::GetContentRegionAvail().y - ch_console_height[1]);
        }
        if(both_ch_uart_settings[0].decode_on && both_ch_uart_settings[1].decode_on)
        {
            draw_separator("chA_chB_splitter", &ch_console_height[1]);
            ch_console_height[1] = std::max(ch_console_height[1], 2*grabber_height);
            console_height = std::max(console_height, grabber_height * 4);
            ch_console_height[1] = std::min(ch_console_height[1], console_height - 2*grabber_height);
        }
        if(both_ch_uart_settings[1].decode_on)
        {
            print_stream(2, librador_get_uart_string(2), &uart_ch_console_at_bottom[1], window_content_width, ImGui::GetContentRegionAvail().y);
        }
    } else if(protocol_sel == Protocol::I2C) {
            print_stream(3, librador_get_i2c_string(), &i2c_console_at_bottom, window_content_width, ImGui::GetContentRegionAvail().y);
    }
}

bool logicDecodeUI::decoding_on()
{
    return both_ch_uart_settings[0].decode_on || both_ch_uart_settings[1].decode_on || protocol_sel == Protocol::I2C;
}

void logicDecodeUI::draw_settings(const bool logic_enable_in[2], bool scopelogic_mode)
{
    bool logic_enable[2];
    if(scopelogic_mode) {
        logic_enable[0] = false;
        logic_enable[1] = true;
    } else {
        memcpy(logic_enable, logic_enable_in, 2 * sizeof(bool));
    }
    ch_console_height[0] = console_height - ch_console_height[1];
    bool uart_changed = false;
    bool i2c_changed = false;
    bool uart_allowed = logic_enable[0] || logic_enable[1];
    bool i2c_allowed = logic_enable[0] && logic_enable[1];

    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::Separator();

    ImGui::BeginDisabled(!(logic_enable[0] || logic_enable[1])); //covers nearly entire fn.
    ImGui::Text("Logic Decoding");

    bool allowed[2] = {uart_allowed, i2c_allowed};
    bool* changed[2] = {&uart_changed, &i2c_changed};
    Protocol prots[2] = {Protocol::UART, Protocol::I2C};
    const char * labels[2] = {"UART", "I2C"};

    bool open_ch_serial_settings = false;
    char chAB[2] = {'A', 'B'};

    if (ImGui::BeginTable("logic_settings_table", 2, ImGuiTableFlags_SizingStretchProp|ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH , ImVec2(ImGui::GetContentRegionAvail().x, 0.f)) )
    {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 0.75f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 0.25f);
        ImGui::TableNextRow();
        for(int j : {1,2}) {
            ImGui::TableNextColumn();
            ImGui::BeginDisabled(!allowed[j-1]);
            if(ImGui::custom_RadioButton(labels[j-1], (int *) &protocol_sel, j))
                *changed[j-1] = true;
            ImGui::EndDisabled();
            if(!allowed[j-1] && protocol_sel==prots[j-1])
            {
                *changed[j-1] = true;
                protocol_sel = Protocol::None;
            }

            if(prots[j-1] == Protocol::UART) {
                for (int ch: {1,2})
                {
                    ImGui::SameLine();
                    ImGui::BeginDisabled(!logic_enable[ch-1] || !(protocol_sel==Protocol::UART));
                    char buf[20];
                    sprintf(buf,"CH %c##serial_decode",chAB[ch-1]);
                    if(ImGui::Button(buf)) {
                        open_ch_serial_settings = true;
                        ch_sel = ch;
                    }
                    ImGui::EndDisabled(); 
                }
            }
        }
        ImGui::EndTable();
    }
//     ImGui::EndGroup();

    for (int ch : {1,2})
    {
        if((!logic_enable[ch-1] || !(protocol_sel==Protocol::UART)) && both_ch_uart_settings[ch-1].decode_on) 
        {
            uart_changed = true;
            both_ch_uart_settings[ch-1].decode_on = false; 
            ch_console_height[ch-1] = 0.f;
        }
    }

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
            console_height = init_console_height_per_ch - grabber_height;
    }

    if(protocol_sel == Protocol::UART)
        console_height = ch_console_height[0] + ch_console_height[1];
    else if (protocol_sel == Protocol::None)
        console_height = 0.f;
}
