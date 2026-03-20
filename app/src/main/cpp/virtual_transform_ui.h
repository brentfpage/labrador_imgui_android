#ifndef VIRTUALTRANSFORMUI_H
#define VIRTUALTRANSFORMUI_H
class virtualTransformUI
{
    static const int num_gain_options = 3;
    const int gains[num_gain_options] = {1, 5, 10};
    const char* gain_labels[num_gain_options] = {"1x", "5x", "10x"};
    struct ch_settings {
        float offset = 0.f;
        int gain_sel = 0;
        bool is_ac = false;
        bool is_paused = false;
    };

    int ch_sel = 1;
    ch_settings both_ch_settings[2];
    ch_settings* curr_ch_settings = both_ch_settings;
public:
    void draw();
};
#endif // VIRTUALTRANSFORMUI_H
