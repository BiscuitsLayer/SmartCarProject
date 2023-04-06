#include "window.hpp"

// Incomplete type resolve
#include <config/config_handler.hpp>

namespace App {

// Extern variables
/* empty */

CustomWindow::CustomWindow(GL::uint width, GL::uint height, const std::string& title, GL::WindowStyle::window_style_t style)
    : Window(width, height, title, style, WindowEventHandlerWithImGui) {}

CustomWindow::CustomWindow(const Config::WindowConfig& config)
    : CustomWindow(config.params.width, config.params.height, config.params.title,
        config.params.fullscreen ? GL::WindowStyle::Fullscreen : GL::WindowStyle::Close) {}

LRESULT CALLBACK WindowEventHandlerWithImGui(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
        return true;
    }

    CustomWindow* window;

    if (msg == WM_NCCREATE)
    {
        // Store pointer to associated Window class as userdata in Win32 window
        window = reinterpret_cast<CustomWindow*>(((LPCREATESTRUCT)lParam)->lpCreateParams);
        window->window = hwnd;

        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));

        return DefWindowProc(hwnd, msg, wParam, lParam);
    } else {
        window = reinterpret_cast<CustomWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        if (window != nullptr)
            return window->WindowEvent(msg, wParam, lParam);
        else
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

void LimitMaxFps(const Config::WindowConfig& window_config) {
    if (window_config.max_fps.enabled) {
        static std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
        static std::chrono::system_clock::time_point b = std::chrono::system_clock::now();

        const float max_elapsed_time = 1000.0f / window_config.max_fps.value;

        a = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> work_time = a - b;
        if (work_time.count() < max_elapsed_time) {
            std::chrono::duration<double, std::milli> delta_ms(max_elapsed_time - work_time.count());
            auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
            std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
        }
        b = std::chrono::system_clock::now();
    }
}

} // namespace App