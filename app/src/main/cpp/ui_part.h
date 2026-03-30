#ifndef UIPART_H
#define UIPART_H
class inputsUI;

class UI_part
{
    public:
        virtual ~UI_part() {};
        enum Width {single, duplex};
        UI_part(Width width, float aspect_ratio, int n_lines) : width(width), aspect_ratio(aspect_ratio), n_lines(n_lines) {};
        virtual int get_height() = 0;
        virtual void draw(float width, inputsUI* inputs_ui = nullptr) = 0;
        const Width width;
        const float aspect_ratio;
        const int n_lines;
};
#endif
