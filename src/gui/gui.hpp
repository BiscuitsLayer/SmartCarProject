#pragma once

// STL
#include <algorithm>

// ImGui
#include <imgui.h>

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <gui/gui_fwd.hpp>

// LibSmartCar
#include <car_model/car_model.hpp>
#include <model/model.hpp>
#include <helpers/helpers.hpp>
#include <config/config_handler.hpp>

namespace App {

class Gui {
public:
    Gui(const App::Config::WindowConfig& window_config);

    void Cleanup() const;
    void Prepare() const;

    // This function changes context variables, so it is not const
    void Draw();

private:
    bool show_imgui_demo_window;
};

} // namespace App