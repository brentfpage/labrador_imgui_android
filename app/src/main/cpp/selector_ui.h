#ifndef SELECTORUI_H
#define SELECTORUI_H

#include "ui_tile.h"
class selectorUI
{
    int n_tiles;
    UI_tile** tiles;
public:
    selectorUI(UI_tile** tiles, int n_tiles) : tiles(tiles), n_tiles(n_tiles) {};
    int get_height();
    int get_width();
    void draw_popup();
};

#endif // SELECTORUI_H
