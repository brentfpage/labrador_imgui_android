#ifndef WIDGETUI_H
#define WIDGETUI_H
class inputsUI;

class Widget
{
    public:
        virtual ~Widget() {};
        enum Width {one_third, two_thirds};
        Widget(Width width) : width(width) {};
        virtual int get_height() = 0;
        virtual void draw(inputsUI* inputs_ui = nullptr) = 0;
        const Width width;
};
#endif
