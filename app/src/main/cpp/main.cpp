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
// #include "implot_internal.h"
#include "imgui.h"
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
    float pixel_6a_main_scale = 2.625;
    SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL3+OpenGL3 example", (int)(1280 * main_scale), (int)(800 * main_scale), window_flags);
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

    style.FontSizeBase = 19.0f * pixel_6a_main_scale / main_scale;
    ImFont* defaultFont = io.Fonts->AddFontDefault();

    float fontsize = style.FontSizeBase * style.FontScaleDpi;

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
    config.FontDataOwnedByAtlas = false; // prevents imperciptible crash when the app is closed
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
    bool show_simple_window = false;
    bool show_demo_window = false;
    bool show_another_window = false;
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

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
//         ImPlot::ShowDemoWindow();

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
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);



        if (show_mainwindow) {
            plot_ui.recompute_x_bounds(inputs_ui.changed_since_last(), inputs_ui.mode, inputs_ui.xy);

            int statusBarHeight = (int) env->CallIntMethod(MainActivityObject,getStatusBarHeightID);
            int navigationBarHeight = (int) env->CallIntMethod(MainActivityObject,getNavigationBarHeightID);

            bool landscape = true;
            static bool collapse_settings = false;

            float settings_height;
            float fontsize;
            ImGui::SetNextWindowPos(ImVec2(0.f,statusBarHeight));
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x,io.DisplaySize.y - statusBarHeight - navigationBarHeight));
            if(io.DisplaySize.y < io.DisplaySize.x) {
                landscape = true;
                settings_height = portraitScreenWidth - statusBarHeight - navigationBarHeight - 2 * style.WindowPadding.y;
                fontsize = (settings_height - 18 * style.FramePadding.y - 10 * style.CellPadding.y)/15.;

            } else {
                landscape = false;
                settings_height = 15 * ImGui::GetFontSize() + 18 * style.FramePadding.y + 10 * style.CellPadding.y;
                fontsize = ImGui::GetFontSize();
            };
            ImGuiStyle& style = ImGui::GetStyle();

            float settings_width = portraitScreenWidth - 2 * style.WindowPadding.x; //in landscape mode, this value is specifically the settings width when the settings are not collapsed.


            ImGui::Begin("MainWindow",
                         &show_mainwindow,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);   

            float data_width;
            float data_height;
            if(landscape) {
                data_height = settings_height;
                if(collapse_settings) {
                    data_width = ImGui::GetContentRegionAvail().x - std::max(ImGui::GetFontSize(), ImGui::CalcTextSize(" < ").x) - 2 * style.FramePadding.x - style.ItemSpacing.x;
                } else {
                    data_width = ImGui::GetContentRegionAvail().x - settings_width - style.ItemSpacing.x;
                }
            } else {
                data_width = settings_width;
                if(collapse_settings) {
                    data_height = ImGui::GetContentRegionAvail().y - ImGui::GetFontSize() - 2 * style.FramePadding.y - style.ItemSpacing.y;
                } else {
                    data_height = ImGui::GetContentRegionAvail().y - settings_height  - style.ItemSpacing.y;
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
                plot_ui.draw(iso_thread_active, inputs_ui.mode, inputs_ui.ch_enabled(1), inputs_ui.ch_enabled(2), inputs_ui.xy, data_width, plot_height);

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
            ImGui::PushFont(NULL, style.FontSizeBase * fontsize/ImGui::GetFontSize());
            if(!collapse_settings) {
                ImGui::BeginChild("settings",ImVec2(settings_width, settings_height),0,ImGuiWindowFlags_NoScrollbar);
                {
                    ImGui::BeginChild("col1",ImVec2(settings_width*0.34 - style.ItemSpacing.x/2, settings_height),0, ImGuiWindowFlags_NoScrollbar);
                    {
                        inputs_ui.draw();
                        trigger_ui.draw(inputs_ui.scope_enable);
                    }
                    ImGui::EndChild();

                    ImGui::SameLine();
                    ImGui::BeginChild("col2",ImVec2(settings_width*0.66 - style.ItemSpacing.x/2, settings_height),0, ImGuiWindowFlags_NoScrollbar);
                    {
                        virtual_transform_ui.draw();
                        sig_gen_ui.draw(inputs_ui.logic_on());
                        psu_ui.draw();
                        logic_decode_ui.draw_settings(inputs_ui.logic_enable, inputs_ui.scopelogic_mode());
                    }
                    ImGui::EndChild();
                    ImVec2 settingsWindowPos = ImGui::GetWindowPos();
                    settingsWindowTopRight = settingsWindowPos + ImVec2(settings_width - (ImGui::CalcTextSize(" v ").x  + 2 * style.FramePadding.x), 0.);
                }
                ImGui::EndChild();
            }

            char label[36];

            ImGuiID collapse_id = ImGui::GetID("collapse");
            if(collapse_settings) {
                if(landscape) {
                    strcpy(label, " < ###collapse");
                } else {
                    ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(" ^ ").x - 2 * style.FramePadding.x,0.f));
                    strcpy(label, " ^ ###collapse");
                }
                if(ImGui::Button(label)) {
                    collapse_settings = !collapse_settings;
                }
            } else {
                if(landscape) {
                    strcpy(label, " > ###collapse");
                } else {
                    strcpy(label, " v ###collapse");
                }
                ImGui::BeginChild("settings");
                ImGui::BeginChild("col2");
                {
                    ImGui::SetCursorScreenPos(settingsWindowTopRight);
                    ImGui::PushOverrideID(collapse_id);
                    if(ImGui::Button(label)) {
                        collapse_settings = !collapse_settings;
                    }
                    ImGui::PopID();
                }
                ImGui::EndChild();
                ImGui::EndChild();
            }
            ImGui::PopFont();
            ImGui::End();
        }

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        if (show_simple_window)
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);
            ImGui::Checkbox("Labrador", &show_mainwindow);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

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
