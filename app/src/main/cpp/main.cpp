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
#include "ui_tile.h"
#include "sig_gen_ui.h"
#include "inputs_ui.h"
#include "trigger_ui.h"
#include "selector_ui.h"
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
    style.FontSizeBase = 19.f;
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

    config.GlyphOffset = { 0.f, 4.5f };
    const char* filename2 = "font/greek_delta.ttf";
    AAsset* asset2 = AAssetManager_open(mgr, filename2, AASSET_MODE_STREAMING);
    char buf2[2048];
    int nb_read2 = 0;
    nb_read2 = AAsset_read(asset2, buf2, 2048);
    ImFont* greek_delta_glyph;
    greek_delta_glyph = io.Fonts->AddFontFromMemoryTTF(buf2, nb_read2, 13.f, &config);
    AAsset_close(asset);

    jmethodID getStatusBarHeightID = env->GetMethodID(MainActivity, "getStatusBarHeight", "()I");
    jmethodID getNavigationBarHeightID = env->GetMethodID(MainActivity, "getNavigationBarHeight", "()I");
    jmethodID getScreenWidth = env->GetMethodID(MainActivity, "getScreenWidth", "()I");
    jmethodID getScreenHeight = env->GetMethodID(MainActivity, "getScreenHeight", "()I");
    
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
    bool iso_thread_active = false;
    bool need_board_init = true;
    while (!done)
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

        // will intentionally be zero sometimes; see MainActivity
        int statusBarHeight = (int) env->CallIntMethod(MainActivityObject,getStatusBarHeightID);
        int navigationBarHeight = (int) env->CallIntMethod(MainActivityObject,getNavigationBarHeightID);

        static bool collapse_settings = false;

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();


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

        static bool landscape = io.DisplaySize.y < io.DisplaySize.x;
        bool new_landscape = io.DisplaySize.y < io.DisplaySize.x;
        bool orientation_changed = (landscape != new_landscape);
        landscape = new_landscape;
        float settings_height_max;
        float font_scaling = 1.f;
        float tile_singlet_width_pixels;
        float settings_width;
// should compute these scalings only once, but there's no way to get the navigation/status bar heights in landscape mode when in portrait mode or vice-versa; it's necessary to wait until the device actually enters a given orientation to compute the scaling
// scale the font size so that the ui fits in width-wise in portrait mode and height-wise in landscape mode. only scale the font size b/c most of the other built-in lengths are 0-10 pixels so are only responsive to scalings more extreme than +- 10%
        if(landscape) { 
            int prescale_settings_height = inputs_ui.get_height() + trigger_ui.get_height(); // want to be able to fit these two ui_tiles in one column
            int text_height = (inputs_ui.n_lines + trigger_ui.n_lines) * ImGui::GetFontSize(); 
            int padding = prescale_settings_height - text_height; 
            settings_height_max = io.DisplaySize.y - statusBarHeight - navigationBarHeight - 2 * style.WindowPadding.y;
            int avail_for_text = settings_height_max - padding;
            font_scaling = static_cast<float>(avail_for_text)/text_height; 
            tile_singlet_width_pixels = settings_height_max * pixel_6a_setting_panel_aspect / 3.f;
        } else {
            float device_independent_x_padding = 2. * ImGuiStyle().WindowPadding.x + ImGuiStyle().ItemSpacing.x;
            // all lengths on a given device are scaled by main_scale, so can't compare pixels to pixels directly across devices.
            float screen_width_delta = static_cast<double>(io.DisplaySize.x / main_scale - device_independent_x_padding) - (pixel_6a_screen_width / pixel_6a_main_scale - device_independent_x_padding);
            font_scaling = screen_width_ratio / 1.02; //1.02: fudge factor to account for padding that's not scaled
            tile_singlet_width_pixels = (io.DisplaySize.x - style.ItemSpacing.x - 2 * style.WindowPadding.x)/3.;
            ImGui::PushFont(NULL,  style.FontSizeBase * font_scaling);
            settings_height_max = inputs_ui.get_height() + trigger_ui.get_height(); 
            ImGui::PopFont();
        };

        plot_ui.recompute_x_bounds(inputs_ui.changed_since_last(), inputs_ui.mode);

        const int n_tiles = 6;
        UI_tile* tiles[n_tiles] = {&inputs_ui, &trigger_ui, &virtual_transform_ui, &sig_gen_ui, &psu_ui, &logic_decode_ui};
        selectorUI selector_ui = selectorUI(tiles, n_tiles);

        // col1 and grp1 contain singlet-width tiles, col2 and grp2 have duplex-width tiles
        float tile_col_heights[2] = {0.f, 0.f};

        int n_singlet_tiles_visible = 0;
        float singlet_tile_height_when_row_col_tiling = 0.f; 
        ImGui::PushFont(NULL,  style.FontSizeBase * font_scaling);
        for(int i=0; i < n_tiles; i++) {
            if(tiles[i]->is_visible) {
                float height = tiles[i]->next_is_expanded ? tiles[i]->get_height() : tiles[i]->get_collapsed_height();
                tile_col_heights[static_cast<int>(tiles[i]->width)] += height;
                if(tiles[i]->width == UI_tile::Width::singlet) {
                    singlet_tile_height_when_row_col_tiling = fmax(singlet_tile_height_when_row_col_tiling, height);
                    n_singlet_tiles_visible++;
                }
            }
        }
        // these widths only relevent to two-col tiling
        float col1_width = (n_singlet_tiles_visible > 0) ? tile_singlet_width_pixels : 0;
        float col2_width = (tile_col_heights[1] > 0) ? 2 * tile_singlet_width_pixels : 0;

        bool row_col_tiling = (!landscape && (n_singlet_tiles_visible == 2) && ((tile_col_heights[1] + singlet_tile_height_when_row_col_tiling) < fmax(tile_col_heights[0], tile_col_heights[1]))) || \
            (landscape && (n_singlet_tiles_visible > 0) && (0 < tile_col_heights[1]) && ((tile_col_heights[1] + singlet_tile_height_when_row_col_tiling) < settings_height_max));

        ImGui::SetNextWindowPos(ImVec2(0.f,statusBarHeight));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x,io.DisplaySize.y - statusBarHeight - navigationBarHeight));

        ImGuiStyle& style = ImGui::GetStyle();

        ImGui::Begin("MainWindow",
                     &show_mainwindow,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);   

        float data_width;
        float data_height;
        float settings_height;
        if(landscape) {
            data_height = io.DisplaySize.y - statusBarHeight - navigationBarHeight - 2 * style.WindowPadding.y;
            if(collapse_settings) {
                data_width = ImGui::GetContentRegionAvail().x;
            } else {
                if(row_col_tiling) {
                    if (tile_col_heights[1] > 0.f) {
                        settings_width = 2 * tile_singlet_width_pixels + style.ItemSpacing.x;
                    } else if (n_singlet_tiles_visible > 0) {
                        settings_width = tile_singlet_width_pixels + (n_singlet_tiles_visible == 2) * (tile_singlet_width_pixels + style.ItemSpacing.x);
                    } else {
                        settings_width = 0.f;
                    }
                } else {
                    settings_width = col1_width + col2_width + ((col1_width>0)&&(col2_width>0)) * style.ItemSpacing.x;
                }
                settings_width = fmax(settings_width, ImGui::GetFontSize() + 2 * style.FramePadding.x);
                data_width = ImGui::GetContentRegionAvail().x - style.ItemSpacing.x - settings_width;
            }
        } else {
            settings_width = io.DisplaySize.x - 2 * style.WindowPadding.x;
            data_width = settings_width;
            if(collapse_settings) {
                data_height = ImGui::GetContentRegionAvail().y;
            } else {
                if(row_col_tiling) {
                    settings_height = singlet_tile_height_when_row_col_tiling + tile_col_heights[1];
                } else {
                    settings_height = fmax(tile_col_heights[0], tile_col_heights[1]);
                }
                settings_height = fmax(settings_height, ImGui::GetFontSize() + 2 * style.ItemSpacing.y);
                data_height = ImGui::GetContentRegionAvail().y - settings_height;
            }
        }

        ImGui::PopFont();
        ImGui::BeginChild("data",ImVec2(data_width, data_height));
        {
            float console_height;
            if(logic_decode_ui.decoding_on()) {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
                logic_decode_ui.draw_console(data_width);
            }
                
            plot_ui.draw(iso_thread_active, inputs_ui.mode, inputs_ui.ch_enabled(1), inputs_ui.ch_enabled(2), data_width, 0.);
            if(logic_decode_ui.decoding_on()) {
                ImGui::PopStyleVar();
            }

        }
        ImVec2 dataWindowBottomLeft = ImGui::GetWindowPos() + ImVec2(0.f,ImGui::GetWindowSize().y);
        ImVec2 dataWindowBottomRight = ImGui::GetWindowPos() + ImGui::GetWindowSize();
        ImGui::EndChild();
#define INDENTUP ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() - ImVec2(0.f,style.ItemSpacing.y)); // remove gaps between ui_tile groups in order to avoid unwanted presses on the background that open the ui_tile selector popup.  this ItemSpacing is added back in by the tiles within their BeginGroup()/EndGroup() wrappings.  ImGuiContext.DebugShowGroupRects is very handy for debugging the groups
        if(landscape) {
            ImGui::SameLine();
        } else {
            INDENTUP
        }

        ImGuiID col2_id;
        ImGui::PushFont(NULL,  style.FontSizeBase * font_scaling);
        bool maybe_clicked_background = false;
        bool screen_keyboard_shown = SDL_ScreenKeyboardShown(window);
        ImVec2 settings_window_center;
        if(!collapse_settings) {
            ImGui::BeginChild("settings",ImVec2(0.f, 0.f),0 );
            settings_window_center = ImGui::GetWindowPos() + ImGui::GetWindowSize()/2.;
            ImGuiContext& g = *GImGui;
            ImVec2 settings_start_pos = ImGui::GetCursorScreenPos();
            ImGui::SetNextItemAllowOverlap();
            if(!screen_keyboard_shown && ImGui::InvisibleButton("open ui_tile selector", {0.f, 0.f})) {
                    maybe_clicked_background = true;
            }
            ImGui::SetCursorScreenPos(settings_start_pos);

            if(row_col_tiling) {
                for(int grp : {0,1}) {
                    if(grp==1)
                        INDENTUP
                    ImGui::BeginGroup();
                    bool first = true;
                    for(int i=0; i<n_tiles; i++) {
                        if (tiles[i]->is_visible && (static_cast<int>(tiles[i]->width) == grp)) {
                            if((grp==1)&&(!first)) {
                                INDENTUP
                            }
                            first = false;
                            tiles[i]->draw(tile_singlet_width_pixels + grp * (tile_singlet_width_pixels + style.ItemSpacing.x), &inputs_ui);
                            maybe_clicked_background &= !ImGui::IsItemHovered();
                            // items in group 0 are stacked side-by-side; those in group 1 are stacked vertically
                            if(grp==0) {
                                ImGui::SameLine(); // seems to be invalidated after endgroup, which is convenient here
                            }
                        }
                    }
                    ImGui::EndGroup();
                }
            } else {
                for(int col : {0,1}) {
                    if(tile_col_heights[col] > 0)
                    {
                        ImGui::BeginGroup();
                        bool first = true;
                        for(int i=0; i<n_tiles; i++) {
                            if (tiles[i]->is_visible && (static_cast<int>(tiles[i]->width) == col)) {
                                if(!first)
                                    INDENTUP
                                first=false;
                                float adjustment = 0.1;
    // "singlet"-width tiles are (tile_singlet_width_pixels + adjustment) wide
    // "duplex"-width tiles are (tile_singlet_width_pixels - adjustment) wide
                                tiles[i]->draw((static_cast<int>(tiles[i]->width) + 1) * tile_singlet_width_pixels + (-2*static_cast<int>(tiles[i]->width) + 1) * adjustment, &inputs_ui);
                                maybe_clicked_background &= !ImGui::IsItemHovered();
                            }
                        }
                        ImGui::EndGroup();
                        ImGui::SameLine();
                    }
                }
                ImGui::NewLine();
            }
            ImGui::EndChild();

        }
        ImGui::PopFont();

        if(maybe_clicked_background) {
            LOGW("reached");
        }
        char label[36];

        style = ImGui::GetStyle();
        ImGuiID collapse_id = ImGui::GetID("collapse");
        ImVec2 collapse_button_pos;
        if(landscape) {
            if(collapse_settings) {
                strcpy(label, " < ");
            } else {
                strcpy(label, " > ");
            }
            collapse_button_pos = dataWindowBottomRight - ImGui::CalcTextSize(" < ") - style.FramePadding * 2;
        } else {
            if(collapse_settings) {
                strcpy(label, " ^ ");
            } else {
                strcpy(label, " v ");
            }
            collapse_button_pos = dataWindowBottomLeft - ImVec2(0.f,ImGui::CalcTextSize(" ^ ").y + style.FramePadding.y * 2);
        }
        ImGui::BeginChild("data");
        ImGui::BeginChild("plot");
        ImGui::SetCursorScreenPos(collapse_button_pos);
        if(ImGui::custom_ButtonEx(label)) {
            collapse_settings = !collapse_settings;
        }
        ImGui::EndChild();
        ImGui::EndChild();

        // maybe_clicked_background = clicked_background at this point
        if(maybe_clicked_background || (orientation_changed && ImGui::IsPopupOpen("config_settings"))) {
            ImVec2 main_window_bottom_right = ImGui::GetWindowPos() + ImGui::GetWindowSize();
            ImVec2 centered_selector_window_bottom_right = settings_window_center + ImVec2(selector_ui.get_width(), selector_ui.get_height())/2.;
            if(\
                (landscape && (centered_selector_window_bottom_right.x > main_window_bottom_right.x)) || 
                (!landscape && (centered_selector_window_bottom_right.y > main_window_bottom_right.y))) {
                ImVec2 edge_selector_window_pos = ImGui::GetWindowPos() + \
                                         ImVec2((ImGui::GetWindowSize().x - selector_ui.get_width()) * (landscape ? 1. : 0.5), (ImGui::GetWindowSize().y - selector_ui.get_height()) * (landscape ? 0.5 : 1));
                ImGui::SetNextWindowPos(edge_selector_window_pos,0,ImVec2(0.f,0.f));
            } else {
                ImGui::SetNextWindowPos(settings_window_center,0,ImVec2(0.5f,0.5f));
            }
            ImGui::OpenPopup("config_settings");
            maybe_clicked_background = false;
        }
        selector_ui.draw_popup();

        ImGui::End();

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

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
