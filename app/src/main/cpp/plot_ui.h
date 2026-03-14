#ifndef PLOTUI_H
#define PLOTUI_H

#include "inputs_ui.h"
#include <vector>

#define GRAPH_SAMPLES 512
class plotUI
{
    double xmin = -.1;
    double xmax = 0.;
    double ymin = -2.;
    double ymax = 2.;
    double x_constraint_min = -10.;
    double x_constraint_max = 0.;
public:
    void recompute_x_bounds(bool mode_changed, inputsUI::Mode mode, bool xy);
    void draw(bool iso_thread_active, inputsUI::Mode mode, bool chA_enabled, bool chB_enabled, bool xy, double data_width, double plot_height);
    double delay;
    double time_window;
};
#endif // PLOTUI_H
