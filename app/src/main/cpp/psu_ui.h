#ifndef PSUUI_H
#define PSUUI_H
class psuUI
{
    const std::chrono::milliseconds between_usb_sends_min{100};
    float psu = 4.5f;
    std::chrono::steady_clock::time_point last_usb_send;
    bool need_usb_send = false;
public:
    void usb_send_data();
    void draw();
};
#endif // PSUUI_H
