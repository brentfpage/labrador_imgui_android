#ifndef SELECTORUI_H
#define SELECTORUI_H

#include "ui_tile.h"
class selectorUI : public UI_tile
{
    int n_tiles;
    UI_tile** tiles;
public:
    selectorUI(UI_tile** tiles, int n_tiles) : tiles(tiles), n_tiles(n_tiles), UI_tile("Selector", "Sel.", UI_tile::Width::single, 6) {};
    void draw(float width_pixels, inputsUI* inputs_ui = nullptr) override;
    int get_height() override;
    void draw_popup();
};

#endif // SELECTORUI_H
