#include "settings_panel.h"

void do_settings_panel_layout(bool landscape, int x_size, int y_size, float dpi, float pixel_6a_dpi) {
    adjustment = 0.f;
    // should compute these values only once, but there's no way to get the navigation/status bar heights in landscape mode when in portrait mode or vice-versa; it's necessary to wait until the device actually enters a given orientation to access the heights
    // in landscape mode, allow scrolling the settings panel in the y direction, so don't vare about the height
    if(landscape) { 
        settings_height_max = y_size;
        tile_singlet_width_pixels = settings_height_max * pixel_6a_setting_panel_aspect / 3.f;
    } else {
        // if the current device's screen is smaller width-wise than the pixel 6a's screen, make sure the singlet-width tiles remain the same width in inches as on the pixel 6a.  do this by transfering space from the duplex-width tiles (which aren't as space-constrained)
        adjustment = ((pixel_6a_screen_width * dpi / pixel_6a_dpi) - static_cast<double>x_size)/3.; 
        adjustment = adjustment < 0 ? 0 : adjustment;
        tile_singlet_width_pixels = (io.DisplaySize.x - style.ItemSpacing.x - 2 * style.WindowPadding.x)/3.;
    };


}
