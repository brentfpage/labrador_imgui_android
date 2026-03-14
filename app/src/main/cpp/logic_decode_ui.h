#ifndef LOGICDECODEUI_H
#define LOGICDECODEUI_H

#include "uartstyledecoder.h"

class logicDecodeUI
{
    enum class Protocol {None, UART, I2C};
    Protocol protocol_sel = Protocol::None;

    static const int num_baud_options = 12;
    const int baud_rates[num_baud_options] = {      
      300,
      600,
      1200,
      2400,
      4800,
      9600,
      14400,
      19200,
      28800,
      38400,
      57600,
      115200
    };
    const char* baud_rate_labels[num_baud_options] = {
      "300",
      "600",
      "1200",
      "2400",
      "4800",
      "9600",
      "14400",
      "19200",
      "28800",
      "38400",
      "57600",
      "115200"};

    static const int num_parity_options = 3;
    const UartParity parities[num_parity_options] = {UartParity::None, UartParity::Even, UartParity::Odd};
    const char* parity_labels[num_parity_options] = {"None", "Even", "Odd"};

    struct uart_settings {
        bool decode_on = false;
        int baud_idx_sel = 0;
        int parity_idx_sel = 0;
    };

    int ch_sel = 1;
    uart_settings both_ch_uart_settings[2];
    uart_settings* curr_ch_uart_settings = both_ch_uart_settings;
    float console_height = 0.f;
    float ch_console_height[2] = {0.f, 0.f};
    float init_console_height_per_ch = 300.f;
    float grabber_height = 60.f;
    void draw_separator(const char * label, float *item_below_height);
    void print_stream(int id, const char * text, bool *at_bottom, float window_content_width, float ch_console_height);
    bool uart_ch_console_at_bottom[2] = {true, true};
    bool i2c_console_at_bottom = true;
public:
    bool decoding_on();
    void draw_settings(const bool logic_enable[2], bool scopelogic_mode);
    void draw_console(float window_content_width);//const char * from_librador_1, const char * from_librador_2 = nullptr);
    float get_console_height(float avail_y);
};
#endif // LOGICDECODEUI_H
