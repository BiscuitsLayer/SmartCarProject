#include "gui.hpp"

// ImGui Implementation
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <imgui.h>

namespace App {

// Extern variables
extern const char* keyboard_modes[static_cast<size_t>(KeyboardMode::SIZE)];

Gui::Gui(const App::Config::WindowConfig& window_config) {
    auto raw_window_handle = FindWindowA("OOGL_WINDOW", window_config.params.title.c_str());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsClassic();
    // TODO: select ImGui style button
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImGui_ImplWin32_Init(raw_window_handle);
    ImGui_ImplOpenGL3_Init();

    show_imgui_demo_window = false;
}

void Gui::Cleanup() const {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Gui::Prepare() const {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Gui::Draw() {
    auto& context = App::Context::Get();

    ///// LEFT MENU /////

    const float displayWidth = ImGui::GetIO().DisplaySize.x;
    const float displayHeight = ImGui::GetIO().DisplaySize.y;

    float mainAreaTop = 0.0f;

    // Quick Hack: Added some padding to hide the window resize controls from right/bottom/top edge of the
    // window. When proper window docking is available in imgui use that instead.
    constexpr float HACK_PADDING = 0.0f; //9.0f;
    const float mainAreaHeight = displayHeight - mainAreaTop;
    const float windowHeight = mainAreaHeight + HACK_PADDING * 2u;
    const float windowPosY = mainAreaTop - HACK_PADDING;

    constexpr float MENU_DEFAULT_WIDTH_FACTOR = 30.0f;
    const float menuWidth = MENU_DEFAULT_WIDTH_FACTOR * ImGui::GetFontSize();

    ImGui::SetNextWindowSize(ImVec2(menuWidth, mainAreaHeight), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(0, -1), ImVec2(FLT_MAX, -1)); // Horizontal resize only

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("Right Menu", nullptr,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | /* ImGuiWindowFlags_NoScrollbar | */
        ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImGui::PopStyleVar(2u); // Undo ImGui::PushStyleVar

    const float windowWidth = ImGui::GetWindowWidth();
    const float windowPosX = 0; //displayWidth - windowWidth + HACK_PADDING;

    // Keep the window glued to the right edge of the screen.
    ImGui::SetWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::SetWindowPos(ImVec2(windowPosX, windowPosY));

    const float menuContentWidth = ImGui::GetContentRegionAvail().x - HACK_PADDING;
    const float menuContentHeight = ImGui::GetContentRegionAvail().y - HACK_PADDING * 2u;

    ImGui::Dummy(ImVec2(0.0f, HACK_PADDING));

    ////////////////////////////////////////////////////////////////////////////////////////

    // DEMO WINDOW TOOLS

    if (show_imgui_demo_window) {
        ImGui::ShowDemoWindow(&show_imgui_demo_window);
    }

    //ImGui::Begin("Parameters");

    if (ImGui::Button("Clear car transform")) {
        // Update car movement transform
        context.car_model->precomputed_movement_transform_ = Transform{};
        context.car_model->UpdateMovementTransform();
        context.car_model->accelerator_.Stop();

        // Update camera target and position
        context.camera->reached_final_position_ = false;
    }
    ImGui::SameLine();
    ImGui::Checkbox("Demo Window", &show_imgui_demo_window);

    // MODELS

    ImGui::SeparatorText("Car model");

    // Car model
    ImGui::PushID("car_model");
    if (ImGui::TreeNode(context.car_model->name_.c_str())) {

        // WARNING: be careful with ImGui ID system: without PushID
        // both "All BBoxes" enable and disable buttons and
        // "Wheels BBoxes" enable and disable buttons will have
        // the same ID and therefore won't work properly
        ImGui::PushID("wheels");

        ImGui::Text("Wheels BBoxes");
        ImGui::SameLine();
        if (ImGui::SmallButton("Enable")) {
            context.car_model->SetDrawWheelsBBoxes(true);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Disable")) {
            context.car_model->SetDrawWheelsBBoxes(false);
        }
        ModelMeshBBoxSelector(context.car_model);

        ImGui::PopID(); // wheels
    }
    ImGui::PopID(); // car model

    ImGui::SeparatorText("Environment");

    for (int model_idx = 0; model_idx < context.env.size(); ++model_idx) {
        ImGui::PushID(model_idx);
        if (ImGui::TreeNode(context.env[model_idx]->name_.c_str())) {
            ModelMeshBBoxSelector(context.env[model_idx]);
        }
        ImGui::PopID();
    }

    ImGui::SeparatorText("Obstacles");

    for (int model_idx = 0; model_idx < context.obstacles.size(); ++model_idx) {
        ImGui::PushID(model_idx);
        if (ImGui::TreeNode(context.obstacles[model_idx]->name_.c_str())) {
            ModelMeshBBoxSelector(context.obstacles[model_idx]);
        }
        ImGui::PopID();
    }

    // CAMERA PARAMETERS

    ImGui::SeparatorText("Camera parameters");

    ImGui::DragFloat3("Position", (float*)&context.camera->position_, 0.1f, -100.0f, 100.0f);
    ImGui::Checkbox("Position fixed behind car", &context.camera->is_position_fixed_behind_car_);
    ImGui::DragFloat3("Target", (float*)&context.camera->target_, 0.1f, -100.0f, 100.0f);
    ImGui::Checkbox("Target fixed on car", &context.camera->is_target_fixed_on_car_);
    ImGui::Text("Length to target: %.3f", context.camera->cur_length_to_target_);

    ImGui::SliderFloat("Move speed", &context.camera->move_speed_, 1.0f, 50.0f);
    ImGui::SliderFloat("Rotate speed", &context.camera->rotate_speed_, 1.0f, 50.0f);

    ImGui::DragFloat("Min length to target", &context.camera->min_length_to_target_, 0.2f, 1.0f, (std::min)(context.camera->max_length_to_target_, 999.8f));
    ImGui::DragFloat("Max length to target", &context.camera->max_length_to_target_, 0.2f, (std::max)(context.camera->min_length_to_target_, 1.2f), 1000.0f);

    // KEYBOARD MODE

    ImGui::SeparatorText("Keyboard mode");

    ImGui::PushItemWidth(160);

    ImGui::PushID(0);
    ImGui::ListBox("", (int*)&context.keyboard_mode.value(), keyboard_modes, IM_ARRAYSIZE(keyboard_modes));
    ImGui::PopID();

    ImGui::PopItemWidth();

    // FPS COUNTER

    ImGui::SeparatorText("FPS counter");

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    //ImGui::End();

////////////////////////////////////////////////////////////////////////////////////////

    ImGui::End(); // Right Menu

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::ModelMeshBBoxSelector(std::shared_ptr<App::Model> model) const {
    ImGui::Text("All BBoxes");
    ImGui::SameLine();
    if (ImGui::SmallButton("Enable")) {
        model->SetDrawBBoxes(true);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Disable")) {
        model->SetDrawBBoxes(false);
    }

    if (ImGui::TreeNode("Meshes")) {
        for (int mesh_idx = 0; mesh_idx < model->meshes_.size(); ++mesh_idx) {
            ImGui::PushID(mesh_idx);

            auto& mesh = model->meshes_[mesh_idx];

            ImGui::Text(mesh.name_.c_str());
            ImGui::SameLine();
            ImGui::Checkbox("BBox", &mesh.bbox_.is_enabled_);

            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    ImGui::TreePop();
}


} // namespace App