#ifndef SETTINGSPANEL_H
#define SETTINGSPANEL_H

#include "ui_tile.h"
#include "sig_gen_ui.h"
#include "inputs_ui.h"
#include "trigger_ui.h"
#include "selector_ui.h"
#include "virtual_transform_ui.h"
#include "psu_ui.h"
#include "logic_decode_ui.h"
inputsUI inputs_ui = inputsUI();
triggerUI trigger_ui = triggerUI();
virtualTransformUI virtual_transform_ui = virtualTransformUI();
sigGenUI sig_gen_ui = sigGenUI();
psuUI psu_ui = psuUI();
logicDecodeUI logic_decode_ui = logicDecodeUI();

const int n_tiles = 6;
UI_tile* tiles[n_tiles] = {&inputs_ui, &trigger_ui, &virtual_transform_ui, &sig_gen_ui, &psu_ui, &logic_decode_ui}; 
selectorUI selector_ui = selectorUI(tiles, n_tiles);

float pixel_6a_setting_panel_aspect = 1.13; // width to height
float settings_height_max;
float adjustment;
float tile_singlet_width_pixels;
float settings_width;

void do_settings_panel_layout(bool landscape, int x_size, int y_size, float dpi, float pixel_6a_dpi);
#endif // SETTINGSPANEL_H
