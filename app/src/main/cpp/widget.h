#ifndef WIDGETUI_H
#define WIDGETUI_H
class inputsUI;

class Widget
{
    public:
        virtual ~Widget() {};
        Widget(int n_lines) : n_lines(n_lines) {};
        virtual int get_height() = 0;
        virtual void draw(inputsUI* inputs_ui = nullptr) = 0;
        const int n_lines;
};
#endif
