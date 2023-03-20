#include "gui.hpp"

// ImGui Implementation
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <imgui.h>

namespace App {

Gui::Gui(App::Config::WindowConfig window_config) {
    auto raw_window_handle = FindWindowA("OOGL_WINDOW", window_config.params.title.c_str());

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

void Gui::Cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Gui::Prepare() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Gui::Draw() {
    auto& context = App::Context::Get();

    // DEMO WINDOW TOOLS

    if (show_imgui_demo_window) {
        ImGui::ShowDemoWindow(&show_imgui_demo_window);
    }

    ImGui::Begin("Parameters");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("Sample text");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &show_imgui_demo_window);      // Edit bools storing our window open/close state

    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    // MODELS

    ImGui::SeparatorText("Models");

    for (int model_idx = 0; model_idx < context.models.size(); ++model_idx) {
        ImGui::PushID(model_idx);
        if (ImGui::TreeNode(context.models[model_idx]->name_.c_str())) {
            ImGui::Text("All BBoxes");
            ImGui::SameLine();
            if (ImGui::SmallButton("Enable")) {
                context.models[model_idx]->SetDrawBBoxes(true);
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("Disable")) {
                context.models[model_idx]->SetDrawBBoxes(false);
            }

            if (model_idx == 0) {
                auto car_model_ptr = std::dynamic_pointer_cast<App::CarModel>(context.models[model_idx]);

                // WARNING: be careful with ImGui ID system: without PushID
                // both "All BBoxes" enable and disable buttons and
                // "Wheels BBoxes" enable and disable buttons will have
                // the same ID and therefore won't work properly
                ImGui::PushID("wheels");

                ImGui::Text("Wheels BBoxes");
                ImGui::SameLine();
                if (ImGui::SmallButton("Enable")) {
                    car_model_ptr->SetDrawWheelsBBoxes(true);
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("Disable")) {
                    car_model_ptr->SetDrawWheelsBBoxes(false);
                }

                ImGui::PopID();

            }

            if (ImGui::TreeNode("Meshes")) {
                for (int mesh_idx = 0; mesh_idx < context.models[model_idx]->meshes_.size(); ++mesh_idx) {
                    ImGui::PushID(mesh_idx);

                    auto& mesh = context.models[model_idx]->meshes_[mesh_idx];

                    ImGui::Text(mesh.name_.c_str());
                    ImGui::SameLine();
                    ImGui::Checkbox("BBox", &mesh.draw_bbox_);

                    ImGui::PopID();
                }
                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    // CAMERA PARAMETERS

    auto camera_ptr = context.camera;

    ImGui::SeparatorText("Camera parameters");

    ImGui::DragFloat3("Position", (float*)&camera_ptr->camera_position_, 0.1f, -100.0f, 100.0f);
    ImGui::Checkbox("Position fixed behind car", &camera_ptr->camera_position_fixed_behind_car_);
    ImGui::DragFloat3("Target", (float*)&camera_ptr->camera_target_, 0.1f, -100.0f, 100.0f);
    ImGui::Checkbox("Target fixed on car", &camera_ptr->camera_target_fixed_on_car_);
    ImGui::Text("Length to target: %.3f", camera_ptr->cur_length_to_target_);

    ImGui::SliderFloat("Move speed", &camera_ptr->camera_move_speed_, 1.0f, 50.0f);
    ImGui::SliderFloat("Rotate speed", &camera_ptr->camera_rotate_speed_, 1.0f, 50.0f);

    ImGui::DragFloat("Min length to target", &camera_ptr->camera_min_length_to_target_, 0.2f, 1.0f, std::min(camera_ptr->camera_max_length_to_target_, 999.8f));
    ImGui::DragFloat("Max length to target", &camera_ptr->camera_max_length_to_target_, 0.2f, std::max(camera_ptr->camera_min_length_to_target_, 1.2f), 1000.0f);

    camera_ptr->UpdateMatrix();

    // KEYBOARD MODE

    ImGui::SeparatorText("Keyboard mode");

    ImGui::PushItemWidth(160);
    const char* keyboard_modes[] = { "ORBIT CAMERA", "FIRST PERSON CAMERA", "CAR MOVEMENT" };

    ImGui::PushID(0);
    ImGui::ListBox("", (int*)&context.keyboard_mode.value(), keyboard_modes, IM_ARRAYSIZE(keyboard_modes));
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

} // namespace App