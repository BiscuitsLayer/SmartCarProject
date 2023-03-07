#pragma once

// ImGui
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <imgui.h>

// Sources
#include <helpers/helpers.hpp>

namespace App {

struct Gui {

    Gui(std::shared_ptr<App::Camera> camera_ptr, std::shared_ptr<KeyboardMode> keyboard_mode_ptr)
    : camera_ptr_(camera_ptr), keyboard_mode_ptr_(keyboard_mode_ptr) {
        auto raw_window_handle = FindWindowA("OOGL_WINDOW", APP_INIT_WINDOW_TITLE.c_str());

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui::StyleColorsClassic();
        //ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        ImGui_ImplWin32_Init(raw_window_handle);
        ImGui_ImplOpenGL3_Init();

        clear_color = ImVec4(1.000F, 1.000F, 1.000F, 1.0F);
        show_imgui_demo_window = false;
    }

    void Cleanup() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    void Prepare() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void Draw() {

        // DEMO WINDOW TOOLS

        if (show_imgui_demo_window) {
			ImGui::ShowDemoWindow(&show_imgui_demo_window);
		}

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_imgui_demo_window);      // Edit bools storing our window open/close state

        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        // CAMERA PARAMETERS

        ImGui::SeparatorText("Camera parameters");

        ImGui::DragFloat3("Position", (float*)&camera_ptr_->camera_position_, 0.1f, -100.0f, 100.0f);
        ImGui::DragFloat3("Target", (float*)&camera_ptr_->camera_target_, 0.1f, -100.0f, 100.0f);
        ImGui::Text("Length to target: %.3f", camera_ptr_->cur_length_to_target_);

        ImGui::SliderFloat("Move speed", &camera_ptr_->camera_move_speed_, 1.0f, 50.0f);
        ImGui::SliderFloat("Rotate speed", &camera_ptr_->camera_rotate_speed_, 1.0f, 50.0f);

        ImGui::DragFloat("Min length to target", &camera_ptr_->camera_min_length_to_target_, 0.2f, 1.0f, std::min(camera_ptr_->camera_max_length_to_target_, 999.8f));
        ImGui::DragFloat("Max length to target", &camera_ptr_->camera_max_length_to_target_, 0.2f, std::max(camera_ptr_->camera_min_length_to_target_, 1.2f), 1000.0f);

        camera_ptr_->UpdateMatrix();

        // KEYBOARD MODE

        ImGui::SeparatorText("Keyboard mode");

        ImGui::PushItemWidth(160);
        const char* keyboard_modes[] = { "ORBIT CAMERA", "FIRST PERSON CAMERA", "CAR MOVEMENT" };
        int* keyboard_mode_ptr_raw = reinterpret_cast<int *>(keyboard_mode_ptr_.get());

        ImGui::PushID(0);
        ImGui::ListBox("", keyboard_mode_ptr_raw, keyboard_modes, IM_ARRAYSIZE(keyboard_modes));
        ImGui::PopID();

        ImGui::PopItemWidth();

        // FPS COUNTER

        ImGui::SeparatorText("FPS counter");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

		ImGui::EndFrame();
        ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    bool show_imgui_demo_window;
    ImVec4 clear_color;

    std::shared_ptr<App::Camera> camera_ptr_;
    std::shared_ptr<KeyboardMode> keyboard_mode_ptr_;
};

} // namespace App