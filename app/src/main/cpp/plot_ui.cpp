#include "implot.h"
#include "plot_ui.h"
#include "librador.h"

void plotUI::recompute_x_bounds(bool mode_changed, inputsUI::Mode mode, bool xy)
{
    if(mode_changed)
    {
        if(mode==inputsUI::Mode::Scope750) {
            time_window = std::min(5., time_window);
            delay = std::min(5. - time_window, delay);
            ImPlot::SetNextAxisLimits(ImAxis_X1, -(delay+time_window), -delay, ImPlotCond_Always);
            x_constraint_min = -5.;
            x_constraint_max = 0.;
        } else if (xy) {
            xmin = ymin;
            xmax = ymax;
            ImPlot::SetNextAxisLimits(ImAxis_X1, xmin, xmax, ImPlotCond_Always);
            x_constraint_min = -20.;
            x_constraint_max = 20.;
        } else {
            time_window = std::min(10., time_window);
            delay = std::min(10. - time_window, delay);
            ImPlot::SetNextAxisLimits(ImAxis_X1, -(delay+time_window), -delay, ImPlotCond_Always);
            x_constraint_min = -10.;
            x_constraint_max = 0.;
        }
    } else {
        if(!xy) {
            delay = -xmax;
            time_window = (xmax - xmin);
        }
    }
}

void plotUI::draw(bool iso_thread_active, inputsUI::Mode mode, bool chA_enabled, bool chB_enabled, bool xy, double data_width, double plot_height)
{
    std::vector<double> *from_librador_chA;
    std::vector<double> *from_librador_chB;
    std::vector<double> blank_data{};
    std::vector<double> time_array;

    if(iso_thread_active){
        time_array = librador_get_time_array(delay, time_window, GRAPH_SAMPLES);
        switch(mode) {
        case inputsUI::Mode::Ch1Scope:
            from_librador_chA = librador_get_analog_data(1,time_window,GRAPH_SAMPLES,delay, 0);
            break;
        case inputsUI::Mode::ScopeLogic:
            from_librador_chA = librador_get_analog_data(1,time_window,GRAPH_SAMPLES,delay, 0);
            from_librador_chB = librador_get_digital_data(2,time_window,GRAPH_SAMPLES,delay);
            break;
        case inputsUI::Mode::ScopeScope:
            from_librador_chA = librador_get_analog_data(1,time_window,GRAPH_SAMPLES,delay, 0);
            from_librador_chB = librador_get_analog_data(2,time_window,GRAPH_SAMPLES,delay, 0);
            break;
        case inputsUI::Mode::Ch1Logic:
            from_librador_chA = librador_get_digital_data(1,time_window,GRAPH_SAMPLES,delay);
            break;
        case inputsUI::Mode::LogicLogic:
            from_librador_chA = librador_get_digital_data(1,time_window,GRAPH_SAMPLES,delay);
            from_librador_chB = librador_get_digital_data(2,time_window,GRAPH_SAMPLES,delay);
            break;
        case inputsUI::Mode::None:
            break;
        case inputsUI::Mode::Scope750:
            from_librador_chA = librador_get_analog_data(1,time_window,GRAPH_SAMPLES,delay, 0);
            break;
        }
    } else {
        from_librador_chA = &blank_data;
        from_librador_chB = &blank_data;
        time_array = blank_data;
    }

    ImGui::BeginChild("plot",ImVec2(data_width, plot_height));
    {
        if (ImPlot::BeginPlot("##scope traces", ImGui::GetContentRegionAvail())) {
            if(xy)
                ImPlot::SetupAxes("volts","volts");
            else
                ImPlot::SetupAxes("time (s)","volts");

            ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, x_constraint_min, x_constraint_max);
            ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, -20., 20.);
            ImPlot::SetupAxesLimits(xmin, xmax, ymin, ymax, ImPlotCond_Once);

            ImPlotSpec spec = ImPlotSpec();
            spec.LineWeight = 2;
            if(xy) {
                ImPlot::PlotLine("CH A", from_librador_chA->data(), from_librador_chB->data(), from_librador_chA->size(), spec);
            } else {
                if(chA_enabled)
                    ImPlot::PlotLine("CH A", time_array.data(), from_librador_chA->data(), from_librador_chA->size(), spec);
                if(chB_enabled)
                    ImPlot::PlotLine("CH B", time_array.data(), from_librador_chB->data(), from_librador_chB->size(), spec);
            }
            ImPlotRect axes_limits = ImPlot::GetPlotLimits();

            ymin = axes_limits.Y.Min;
            ymax = axes_limits.Y.Max;
            xmin = axes_limits.X.Min;
            xmax = axes_limits.X.Max;
            ImPlot::EndPlot();
        }
    }
    ImGui::EndChild();
}
