#ifndef TRIGGERUI_H
#define TRIGGERUI_H

#include "o1buffer.h"
#include "widget.h"
#include "inputs_ui.h"
class triggerUI : public Widget
{
    int ch_sel = 1;
    o1buffer::trigger_settings both_ch_trigger_settings[2];
    o1buffer::trigger_settings* curr_ch_trigger_settings = &both_ch_trigger_settings[ch_sel-1];
public:
    triggerUI() : Widget(Widget::Width::one_third) {};
    void draw(inputsUI* inputs_ui = nullptr) override;
    int get_height() override;
};
#endif
