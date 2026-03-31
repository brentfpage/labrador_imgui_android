#ifndef SELECTORUI_H
#define SELECTORUI_H

#include "ui_part.h"
class selectorUI : public UI_part
{
    int n_ui_parts;
    UI_part** ui_parts;
public:
    selectorUI(UI_part** ui_parts, int n_ui_parts) : ui_parts(ui_parts), n_ui_parts(n_ui_parts), UI_part("Selector", "Sel.", UI_part::Width::single, 6) {};
    void init_step_2(UI_part* ui_parts, int n_ui_parts);
    void draw(float width_pixels, inputsUI* inputs_ui = nullptr) override;
    int get_height() override;
    void draw_popup();
};

#endif // SELECTORUI_H
