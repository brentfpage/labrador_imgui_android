#ifndef WIDGETUI_H
#define WIDGETUI_H
class Widget
{
    public:
        virtual ~Widget() {};
        Widget(int n_lines) : n_lines(n_lines) {};
        virtual int get_height() = 0;
        virtual void draw(bool* ctrls = nullptr, int n_ctrls = 0) = 0;
        const int n_lines;
};
#endif
