#ifndef TRIGGERUI_H
#define TRIGGERUI_H

#include "o1buffer.h"
class triggerUI
{
    int ch_sel = 1;
    o1buffer::trigger_settings both_ch_trigger_settings[2];
    o1buffer::trigger_settings* curr_ch_trigger_settings = &both_ch_trigger_settings[ch_sel-1];
public:
    void draw(bool chA_enable, bool chB_enable);
    int get_height();
};
#endif // SCOPEMOODEUI_H
