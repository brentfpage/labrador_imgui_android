#ifndef SELECTORUI_H
#define SELECTORUI_H

#include "ui_part.h"
class selectorUI : public UI_part
{
public:
    static const int n_ui_parts = 7;
private:
    const char * ui_part_names[n_ui_parts] = {"Inputs","Trigger","Virtual Transforms", "Signal Generator", "PSU", "Logic Decoding", "Quick Select"};
    const char * ui_part_short_names[n_ui_parts] = {"Inputs","Trigger","Virt.Tr.", "Sig.Gen.", "PSU", "Log.Dec.", "n.a."};
public:
    bool ui_parts_enable[n_ui_parts] = {true, true, true, true, true, false, true};
    selectorUI() : UI_part("Selector", UI_part::Width::single, 6) {};
    void draw(float width_pixels, bool* enable, inputsUI* inputs_ui = nullptr) override;
    int get_height() override;
    void draw_popup();
};

#endif // SELECTORUI_H
