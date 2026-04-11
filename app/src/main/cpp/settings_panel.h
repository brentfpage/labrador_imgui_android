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

#endif // SETTINGSPANEL_H
