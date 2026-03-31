#ifndef UIPART_H
#define UIPART_H
class inputsUI;

class UI_part
{
    public:
        virtual ~UI_part() {};
        enum Width {single, duplex};
        UI_part(const char* name, const char* short_name, Width width, int n_lines) : name(name), short_name(short_name), width(width), n_lines(n_lines) {};
        virtual int get_height() = 0;
        int get_collapsed_height();
        virtual void draw(float width_pixels, inputsUI* inputs_ui = nullptr) = 0;
        void standard_header(float width_pixels);
        const Width width;
        int n_lines;
        const char* name;
        const char* short_name;
        bool is_expanded = true;
        bool is_visible = true;
};
#endif
