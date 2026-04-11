This is an Android app/GUI for the [EspoTek Labrador](https://espotek.com/labrador/) board.
At present, the layout is designed for phone screens, although tablet support is planned for the future.

<img src="https://github.com/brentfpage/labrador_imgui_android/blob/dc53f1887a877ead76aa8ec110342cc85a142dfe/.assets/labrador_imgui_android.png" width=240>

A compiled version of the app is located in this repo's root directory at labrador\_imgui\_android\_v0.2.apk .
It can be uploaded to your Android device using the procedure outlined in [this guide](https://www.thecustomdroid.com/how-to-install-apk-on-android/) or using Android Studio.

UI tricks and tips:
- Settings panel:
  - tap on a tile header to collapse/expand the tile
  - after collapsing a tile, tap on the 'x' button that appears to remove the tile completely
  - tap on an empty space in the settings panel to open a tile-selector pop-up
  - collapse the settings panel by clicking the button in the upper-left (profile) or bottom-right (landscape)
  - long-press on a slider label to start manual input mode
  - short-press on the sig. gen. freq. slider label to change the range of the slider (e.g., from 0 -> 100 Hz to 0-> 1000 Hz)
- Plot:
  - short-press or drag on an axis to plot reference lines for that axis
    - double tap an axis to clear the reference lines (useful, e.g., if one of the lines is far outside the visible range so can't be easily dragged into place)
  - long-press on an axis to enter manual entry mode for the axis's limits
  - tap on a legend entry to deactivate the plotting of the line
- Other:
  - if the parity label for a logic debugging console turns red, that means that the bit stream does not have the specified parity

Build instructions (only relevant if you want to make edits to the app):
- clone this repository using the `--recurse-submodules` option. The `--recurse-submodules` option ensures that prerequisite submodules (my forks of SDL, imgui, and implot) are cloned as well.
   - navigate to app/src/main/cpp/deps/implot and run `git checkout for_labrador_android_app`
   - navigate to app/src/main/cpp/deps/imgui and run `git checkout for_labrador_android_app`
   - navigate to app/src/main/cpp/deps/SDL and run `git checkout for_labrador_app`
- download the Android sdk at API level 36
- create a file `local.properties` in the root directory of the repo clone and add the line `sdk.dir=path/to/sdk`, where `path/to/sdk` is the path to the Android sdk on your machine
- download the Android ndk, version 28.2.13676358
   - I used Android command line tools to download the ndk, which placed it in a subdirectory of the sdk directory.  Other directory structures may produce minor headaches.
- on linux, download OpenJDK 17
- on mac, download Oracle corp. JDK 25
- navigate to the root directory of the repo clone and run `./gradlew assembleDebug`, which will build a debug version of the app
