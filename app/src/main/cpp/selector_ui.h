#ifndef SELECTORUI_H
#define SELECTORUI_H

#include "ui_part.h"
class selectorUI : public UI_part
{
//     const char * ui_part_short_names[n_ui_parts] = {"Inputs","Trigger","Virt.Tr.", "Sig.Gen.", "PSU", "Log.Dec.", "n.a."};
    int n_ui_parts;
    UI_part** ui_parts;
public:
    selectorUI() : UI_part("Selector", "Sel.", UI_part::Width::single, 6) {};
    void init_step_2(UI_part** ui_parts_in, int n_ui_parts_in);
    void draw(float width_pixels, inputsUI* inputs_ui = nullptr) override;
    int get_height() override;
    void draw_popup();
};

#endif // SELECTORUI_H
