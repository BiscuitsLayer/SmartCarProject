#pragma once

// ImGui
// #include <imgui_internal.h>
#include <imgui.h>

// Sources
#include <car_model/car_model.hpp>
#include <model/model.hpp>
#include <helpers/helpers.hpp>
#include <config/config_handler.hpp>

namespace App {

struct Gui {
    Gui(App::Config::WindowConfig window_config);

    void Cleanup();
    void Prepare();
    void Draw();

    ImVec4 clear_color;
    bool show_imgui_demo_window;
};

} // namespace App