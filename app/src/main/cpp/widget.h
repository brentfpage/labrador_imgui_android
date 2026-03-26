#ifndef WIDGETUI_H
#define WIDGETUI_H
class Widget
{
    public:
        virtual ~Widget() {};
        virtual int get_height() = 0;
};
#endif
