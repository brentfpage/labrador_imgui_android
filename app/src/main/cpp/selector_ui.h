#ifndef SELECTORUI_H
#define SELECTORUI_H

#include "ui_part.h"
class selectorUI : public UI_part
{
    bool scope750 = false;
    bool changed = false;
public:
    selectorUI() : UI_part(UI_part::Width::single, 6) {};
    void draw(float width, inputsUI* inputs_ui = nullptr) override;
};

#endif // SELECTORUI_H
