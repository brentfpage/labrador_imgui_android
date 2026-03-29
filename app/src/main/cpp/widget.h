#ifndef WIDGETUI_H
#define WIDGETUI_H
class inputsUI;

class Widget
{
    public:
        virtual ~Widget() {};
        enum Width {single, duplex};
        Widget(Width width, float aspect_ratio, int n_lines) : width(width), aspect_ratio(aspect_ratio), n_lines(n_lines) {};
        virtual int get_height() = 0;
        virtual void draw(float width, inputsUI* inputs_ui = nullptr) = 0;
        const Width width;
        const float aspect_ratio;
        const int n_lines;
};
#endif
