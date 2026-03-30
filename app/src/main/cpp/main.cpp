// Dear ImGui: standalone example application for SDL3 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "librador.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "implot.h"
#include "imgui.h"
#include "widget.h"
#include "sig_gen_ui.h"
#include "inputs_ui.h"
#include "trigger_ui.h"
#include "virtual_transform_ui.h"
#include "psu_ui.h"
#include "logic_decode_ui.h"
#include "plot_ui.h"
#include "custom_imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
// #include "SDL_android.h"
#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif
#include "imgui_internal.h"
#include <stdlib.h>
#include <chrono>
#include <SDL3/SDL_events.h>

#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif


// Main code
int main(int, char**)
{
    // Setup SDL
    // [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts would likely be your SDL_AppInit() function]
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return 1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_Rect bounds;
    SDL_zero(bounds);
    SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &bounds);

    float pixel_6a_main_scale = 2.625;
    float pixel_6a_screen_width = 1080.f;
    float pixel_6a_single_width = 1038.f/3;
    float pixel_6a_setting_panel_aspect = 1.13; // width to height
//     float pixel_6a_profile_aspect_ratio = 0.49; // relevant to profile b/c it excludes navigation, status bars
    SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window* window = SDL_CreateWindow("main window", (int)bounds.w, (int)bounds.h, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return 1;
    }
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr)
    {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    SDL_PropertiesID propsIme = SDL_CreateProperties(); // for allowing specification of keyboard type (numeric, alpha, ...)
    SDL_SetNumberProperty(propsIme, SDL_PROP_TEXTINPUT_ANDROID_INPUTTYPE_NUMBER, 2);
    io.UserData = &propsIme;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
    style.FontSizeBase = 18.5f;
    style.ItemSpacing = ImVec2(style.ItemSpacing.x, style.ItemSpacing.y/2.);
    style.WindowPadding = ImVec2(style.WindowPadding.x/2,style.WindowPadding.y/2);

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If fonts are not explicitly loaded, Dear ImGui will call AddFontDefault() to select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
    //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a small threshold.
    // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefaultVector();
    //io.Fonts->AddFontDefaultBitmap();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    ImFont* defaultFont = io.Fonts->AddFontDefault();


    // for accessing android app resources
    JNIEnv *env = (JNIEnv *) SDL_GetAndroidJNIEnv();
    jobject MainActivityObject = (jobject) SDL_GetAndroidActivity();
    jclass MainActivity(env->GetObjectClass(MainActivityObject));

    jfieldID asset_manager_id = env->GetFieldID(MainActivity, "mgr", "Landroid/content/res/AssetManager;");
    jobject mgr_java = (jobject)env->GetObjectField(MainActivityObject, asset_manager_id);
    AAssetManager * mgr = AAssetManager_fromJava(env, mgr_java);

    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphOffset = { 0.f, 3.f };
    config.FontDataOwnedByAtlas = false; // prevents imperceptible crash when the app is closed
//     https://stackoverflow.com/a/13317651/3474552
    const char* filename = "font/waveform-glyphs3.ttf";
    AAsset* asset = AAssetManager_open(mgr, filename, AASSET_MODE_STREAMING);
    char buf[2048];
    int nb_read = 0;
    nb_read = AAsset_read(asset, buf, 2048);
    ImFont* waveform_glyph_font;
    waveform_glyph_font = io.Fonts->AddFontFromMemoryTTF(buf, nb_read, 13.f, &config);
    AAsset_close(asset);

    jmethodID getStatusBarHeightID = env->GetMethodID(MainActivity, "getStatusBarHeight", "()I");
    jmethodID getNavigationBarHeightID = env->GetMethodID(MainActivity, "getNavigationBarHeight", "()I");
    jmethodID getScreenWidth = env->GetMethodID(MainActivity, "getScreenWidth", "()I");
    jmethodID getScreenHeight = env->GetMethodID(MainActivity, "getScreenHeight", "()I");
    

    int portraitScreenHeight = (int) env->CallIntMethod(MainActivityObject,getScreenHeight);
    int portraitScreenWidth = (int) env->CallIntMethod(MainActivityObject,getScreenWidth);
    if(portraitScreenWidth > portraitScreenHeight) {
        int temp = portraitScreenWidth;
        portraitScreenWidth = portraitScreenHeight;
        portraitScreenHeight = temp;
    }

    // Our state
    bool show_mainwindow = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    
    inputsUI inputs_ui = inputsUI();
    triggerUI trigger_ui = triggerUI();
    virtualTransformUI virtual_transform_ui = virtualTransformUI();
    sigGenUI sig_gen_ui = sigGenUI();
    psuUI psu_ui = psuUI();
    logicDecodeUI logic_decode_ui = logicDecodeUI();
    plotUI plot_ui = plotUI();

    // Main loop
    bool done = false;
    bool iso_thread_active;
    bool need_board_init = true;
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!done)
#endif
    {

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        // [If using SDL_MAIN_USE_CALLBACKS: call ImGui_ImplSDL3_ProcessEvent() from your SDL_AppEvent() function]
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        int statusBarHeight = (int) env->CallIntMethod(MainActivityObject,getStatusBarHeightID);
        int navigationBarHeight = (int) env->CallIntMethod(MainActivityObject,getNavigationBarHeightID);

        static bool collapse_settings = false;

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

// should compute these scalings only once, but there's no way to get the navigation/status bar heights in landscape mode when in portrait mode
        bool landscape = io.DisplaySize.y < io.DisplaySize.x;
        float settings_height_max;
        float font_scaling = 1.f;
        float single_width_pixels;
        float settings_width;
        // scale the font size so that the ui fits in width-wise (portrait mode) or height-wise (landscape mode). only scale the font size b/c most of the other built-in lengths are 0-10 pixels so are only responsive to scalings more extreme than +- 10%
        if(landscape) { 
            int prescale_settings_height = inputs_ui.get_height() + trigger_ui.get_height() + style.ItemSpacing.y; // want to be able to fit these two widgets in one column
            int text_height = (inputs_ui.n_lines + trigger_ui.n_lines) * ImGui::GetFontSize(); 
            int padding = prescale_settings_height - text_height; 
            settings_height_max = portraitScreenWidth - statusBarHeight - navigationBarHeight - 2 * style.WindowPadding.y;
            int avail_for_text = settings_height_max - padding;
            font_scaling = static_cast<float>(avail_for_text)/text_height; 
            single_width_pixels = settings_height_max * pixel_6a_setting_panel_aspect / 3.f;
        } else {
            float device_independent_x_padding = 2. * ImGuiStyle().WindowPadding.x + ImGuiStyle().ItemSpacing.x;
            // all lengths on a given device are scaled by main_scale, so can't compare pixels to pixels directly across devices.
            float screen_width_ratio = static_cast<double>(portraitScreenWidth / main_scale - device_independent_x_padding)/(pixel_6a_screen_width / pixel_6a_main_scale - device_independent_x_padding);
            font_scaling = screen_width_ratio;
            single_width_pixels = (portraitScreenWidth - style.ItemSpacing.x - 2 * style.WindowPadding.x)/3.;
            ImGui::PushFont(NULL,  style.FontSizeBase * font_scaling);
            settings_height_max = inputs_ui.get_height() + trigger_ui.get_height() + style.ItemSpacing.y; 
            ImGui::PopFont();
        };
//         LOGW("lui height: %d", inputs_ui.get_height());
//         LOGW("tui height: %d", trigger_ui.get_height());
//         LOGW("vtui height: %d", virtual_transform_ui.get_height());
//         LOGW("sg height: %d", sig_gen_ui.get_height());
//         LOGW("psu height: %d", psu_ui.get_height());
//         LOGW("ld height: %d", logic_decode_ui.get_height());
//         LOGW("width: %.1f", io.DisplaySize.x);
//         LOGW("fontsize: %.1f", ImGui::GetFontSize());

// important to have this iso_thread_active check after the new frame starts.  Otherwise, (board connected -> user puts phone to sleep -> user unplugs board -> user wakes phone) leads to a crash.  The crash arises from the librador_get_(analog/digital)_data block below thinking iso_thread_active=true when it's not.
        iso_thread_active = librador_iso_thread_is_active();
        if(!iso_thread_active) {
            need_board_init = true;
        }
        if(need_board_init && iso_thread_active) {
            inputs_ui.update_device_mode();
            sig_gen_ui.usb_send_data(1);
            sig_gen_ui.usb_send_data(2);
            psu_ui.usb_send_data();
            librador_set_oscilloscope_gain(8.);
            need_board_init = false;
        }

        ImGuiIO& io = ImGui::GetIO();

        plot_ui.recompute_x_bounds(inputs_ui.changed_since_last(), inputs_ui.mode);

        enum Widgets {Inputs,Trigger,VirtTrans,SigGen,LogDec};
        const int n_widgets = 6;
        static bool widgets_enable[n_widgets] = {true, true, true, true, true, true};
        const char * widget_names[n_widgets] = {"Inputs","Trigger","Virtual Transforms", "Signal Generator", "PSU", "Logic Decoding"};
        Widget *widgets[n_widgets] = {&inputs_ui, &trigger_ui, &virtual_transform_ui, &sig_gen_ui, &psu_ui, &logic_decode_ui};
        int n_1_3 = 0;
        int n_enabled = 0;
        float widget_height_sum = 0.f;
        float widget_2_3_height_sum = 0.f;
        int which_enabled[n_widgets];
        for(int i=0; i < n_widgets; i++) {
            if(widgets_enable[i]) {
                widget_height_sum += widgets[i]->get_height();
                which_enabled[n_enabled] = i;
                n_enabled++;
                if(widgets[i]->width == Widget::Width::single) {
                    n_1_3++;
                } else {
                    widget_2_3_height_sum += widgets[i]->get_height();
                }
            }
        }

        static int widget_col[n_widgets];
        float widget_col_heights[2] = {0.f, 0.f};
        float widget_row_heights[2] = {0.f, 0.f};

        const int widget_row[n_widgets] = {0,0,1,1,1,1};

        const int widget_col_or_row_standard[n_widgets] = {0,0,1,1,1,1};
        bool go_by_rows = ((!landscape && (n_1_3 == 2)) && (widget_2_3_height_sum < settings_height_max/2.)) || \
            ((landscape && (n_1_3 > 0)) && ((0 < widget_2_3_height_sum) && (widget_2_3_height_sum < settings_height_max/2.)));

        if(go_by_rows) {
            for(int i=0; i< n_widgets; i++) {
                if(widgets_enable[i]) {
                    if(widget_row[i]==0)
                        widget_row_heights[widget_row[i]] = fmax(widget_row_heights[widget_row[i]], widgets[i]->get_height() + style.ItemSpacing.y);
                    else
                        widget_row_heights[widget_row[i]] += widgets[i]->get_height() + style.ItemSpacing.y;
                }
            }
        } else {
            if(landscape && (widget_height_sum < settings_height_max)) {
                memset(widget_col, 0, sizeof(int) * n_widgets);
                widget_col_heights[0] = widget_height_sum;
                widget_col_heights[1] = 0.f;
            } else {
                memcpy(widget_col, widget_col_or_row_standard, sizeof(int) * n_widgets);
                for(int i=0; i< n_widgets; i++) {
                    if(widgets_enable[i]) {
                        widget_col_heights[widget_col[i]] += widgets[i]->get_height() + style.ItemSpacing.y;
                    }
                }
            }
        }

        ImGui::SetNextWindowPos(ImVec2(0.f,statusBarHeight));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x,io.DisplaySize.y - statusBarHeight - navigationBarHeight));

        ImGuiStyle& style = ImGui::GetStyle();

        ImGui::Begin("MainWindow",
                     &show_mainwindow,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);   

        float data_width;
        float data_height;
        float settings_height;
        if(landscape) {
            data_height = portraitScreenWidth - statusBarHeight - navigationBarHeight - 2 * style.WindowPadding.y;
            if(collapse_settings) {
                data_width = ImGui::GetContentRegionAvail().x - std::max(ImGui::GetFontSize(), ImGui::CalcTextSize(" < ").x) - 2 * style.FramePadding.x - style.ItemSpacing.x;
            } else {
                if(go_by_rows) {
                    if (widget_2_3_height_sum > 0.f) {
                        settings_width = 2 * single_width_pixels + (n_1_3 == 2) * style.ItemSpacing.x;
                    } else if (n_1_3 > 0) {
                        settings_width = single_width_pixels + (n_1_3 == 2) * (single_width_pixels + style.ItemSpacing.x);
                    } else {
                        settings_width = ImGui::CalcTextSize(" < ").x + 2 * style.FramePadding.x + style.ItemSpacing.x;
                    }
                } else {
                    float col1_width = 0.f;
                    float col2_width = 0.f;
                    for(int i = 0; i < n_widgets; i++) {
                        if(widgets_enable[i] && (widget_col[i]==0)) {
                            col1_width = fmax(col1_width, (static_cast<int>(widgets[i]->width) + 1) * single_width_pixels);
                        }
                        if(widgets_enable[i] && (widget_col[i]==1)) {
                            col2_width = fmax(col1_width, (static_cast<int>(widgets[i]->width) + 1) * single_width_pixels);
                        }
                        settings_width = col1_width + col2_width + ((col1_width>0)&&(col2_width>0)) * style.ItemSpacing.x;
                    }

                }
                data_width = ImGui::GetContentRegionAvail().x - style.ItemSpacing.x - settings_width;
            }
        } else {
            settings_width = portraitScreenWidth - 2 * style.WindowPadding.x;
            data_width = settings_width;
            if(go_by_rows) {
                settings_height = widget_row_heights[0] + widget_row_heights[1];
            } else {
                settings_height = fmax(fmax(widget_col_heights[0], widget_col_heights[1]), ImGui::GetFontSize() + 2 * style.FramePadding.y + style.ItemSpacing.y);
            }

            if(collapse_settings) {
                data_height = ImGui::GetContentRegionAvail().y - ImGui::GetFontSize() - 2 * style.FramePadding.y - style.ItemSpacing.y;
            } else {
                data_height = ImGui::GetContentRegionAvail().y - settings_height;
            }
        }

        ImGui::BeginChild("data",ImVec2(data_width, data_height));
        {
            float plot_height;
            if(logic_decode_ui.decoding_on()) {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
                plot_height = ImGui::GetContentRegionAvail().y - logic_decode_ui.get_console_height(ImGui::GetContentRegionAvail().y);
            } else {
                plot_height = ImGui::GetContentRegionAvail().y;
            }
            plot_ui.draw(iso_thread_active, inputs_ui.mode, inputs_ui.ch_enabled(1), inputs_ui.ch_enabled(2), data_width, plot_height);

            if(logic_decode_ui.decoding_on()) {
                logic_decode_ui.draw_console(data_width);
                ImGui::PopStyleVar();
            }
        }
        ImGui::EndChild();
        if(landscape) {
            ImGui::SameLine();
        }
        ImGuiID col2_id;
        ImVec2 settingsWindowTopRight;
        ImGui::PushFont(NULL,  style.FontSizeBase * font_scaling);
        if(!collapse_settings) {
            ImGui::BeginChild("settings",ImVec2(0.f, 0.f),0 );
            if(go_by_rows) {
                for(int row : {0,1}) {
                    for(int i=0; i<n_widgets; i++) {
                        if (widgets_enable[i] && (widget_row[i] == row)) {
                            widgets[i]->draw((static_cast<int>(widgets[i]->width) + 1) * single_width_pixels, &inputs_ui);
                            // items in row 0 are stacked side-by-side; those in row 1 are stacked vertically
                            if(row==0) {
                                ImGui::SameLine();
                            }
                        }
                    }
                    if(row==0)
                        ImGui::NewLine(); // overrides previous sameline
                }
            } else {
                float crax = ImGui::GetContentRegionAvail().x;
                for(int col : {0,1}) {
                    if(widget_col_heights[col] > 0)
                    {
                        ImGui::BeginGroup();
                        for(int i=0; i<n_widgets; i++) {
                            if (widgets_enable[i] && (widget_col[i] == col)) {
                                widgets[i]->draw((static_cast<int>(widgets[i]->width) + 1) * single_width_pixels,  &inputs_ui);
                            }
                        }
                        ImGui::EndGroup();
                        ImGui::SameLine();
                    }
                }
                ImGui::NewLine();
            }
            settingsWindowTopRight = ImGui::GetWindowPos() + ImVec2(ImGui::GetWindowSize().x, 0.f);
            ImGui::EndChild();
        }
        ImGui::PopFont();

        char label[36];

        style = ImGui::GetStyle();
        bool open_widget_sel = false;
        ImGuiID collapse_id = ImGui::GetID("collapse");
        if(collapse_settings) {
            if(landscape) {
                strcpy(label, " < ");
            } else {
                ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(" ^ ").x - 2 * style.FramePadding.x,0.f));
                strcpy(label, " ^ ");
            }
            ImGui::PushOverrideID(collapse_id);
            if(ImGui::Button(label)) {
                collapse_settings = !collapse_settings;
            }
            ImGui::PopID();
        } else {
            if(landscape) {
                strcpy(label, " > ");
            } else {
                strcpy(label, " v ");
            }
            ImGui::BeginChild("settings");
            ImGui::SetCursorScreenPos(settingsWindowTopRight - ImVec2(ImGui::CalcTextSize("\xee\xa4\x83 v ").x   + 4 * style.FramePadding.x + style.ItemSpacing.x, 0.));
            if(ImGui::Button("\xee\xa4\x83##start_widget_sel")) {
                open_widget_sel = true;
            }
            ImGui::PushOverrideID(collapse_id);
            ImGui::SameLine();
            if(ImGui::Button(label)) {
                collapse_settings = !collapse_settings;
            }
            ImGui::PopID();
            ImGui::EndChild();
        }


        if(open_widget_sel) {
            ImGui::OpenPopup("config_settings");
        }
        if(ImGui::BeginPopup("config_settings")) {
            ImGui::Text("Select widgets");
            ImGui::Separator();
            for (int i=0; i< sizeof(widgets_enable); i++) {
                ImGui::Checkbox(widget_names[i], &widgets_enable[i]);
            }
            ImGui::EndPopup();
        }

        ImGui::End();

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyProperties(propsIme);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
