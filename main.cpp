// Windows defines for PyTorch
#define NOMINMAX

// LibSmartCar
#include <helpers/helpers.hpp>
#include <transform/transform.hpp>
#include <camera/camera.hpp>
#include <gui/gui.hpp>
#include <timer/timer.hpp>
#include <window/window.hpp>
#include <config/config_handler.hpp>
#include <intersector/intersector.hpp>

// NN
#include <dqn/trainer.hpp>
#include <dqn/env.hpp>

// Model
#include <model/model.hpp>
#include <instanced_model/instanced_model.hpp>
#include <car_model/car_model.hpp>
#include <skybox/skybox.hpp>

// Configured by CMake
#include <config_application_out.hpp>

// Coordinate space matrices
// model: local space -> world space
// view: world space -> camera space
// projection: specified range of space in camera space (seen by camera) -> [-1.0, 1.0] (NDC), 
// clip coordinates out of range

int main(int argc, char** argv) try {
    auto& context = App::Context::Get();
    context.keyboard_mode = App::KeyboardMode::CAR_MOVEMENT;
    context.keyboard_status = App::KeyboardStatus{};
    context.keyboard_status->fill(false);

    if (argc != 2) {
        throw std::runtime_error("Wrong number of arguments!\nUsage: ./SmartCarMain.exe <config file path>");
    }
    App::ConfigHandler config_handler{argv[1], APP_CONFIG_DIR};

    auto window_config = config_handler.GetWindowConfig();
	auto collision_intersector_config = config_handler.GetCollisionIntersectorConfig();
	auto ray_intersector_config = config_handler.GetRayIntersectorConfig();
    auto camera_config = config_handler.GetCameraConfig();

    context.projection_matrix = GL::Mat4::Perspective(
        GL::Rad(camera_config.projection.FOV),
        static_cast<float>(window_config.params.width) / static_cast<float>(window_config.params.height),
        camera_config.projection.near_plane, camera_config.projection.far_plane
    );

    auto& gl = context.gl->get();

    gl.Enable(GL::Capability::DepthTest);
    gl.Enable(GL::Capability::Blend);
    gl.Enable(GL::Capability::CullFace);
    gl.Enable(GL::Capability::Multisample);

    gl.BlendFunc(GL::BlendingFactor::SourceAlpha, GL::BlendingFactor::OneMinusSourceAlpha);

    context.camera = App::Camera(camera_config);
    context.car_model->SetCollisionIntersector(collision_intersector_config);
    context.car_model->SetRayIntersector(ray_intersector_config);
    App::Gui gui(window_config);

    App::Timer main_timer;
    main_timer.Start();

    // NN stuff
    AppNN::Trainer nn_trainer;

    bool space_was_pressed = false;
    bool draw_gui = true;

    GL::Event ev;
    while (context.window->IsOpen()) {
        auto delta_time = static_cast<float>(main_timer.Tick<App::Timer::Seconds>());

        while (context.window->GetEvent(ev)) {
            if (ev.Type == GL::Event::KeyDown) {
                context.keyboard_status.value()[ev.Key.Code] = true;
            } else if (ev.Type == GL::Event::KeyUp) {
                context.keyboard_status.value()[ev.Key.Code] = false;
            }

            if (context.keyboard_status.value()[GL::Key::Space]) {
                if (!space_was_pressed) {
                    draw_gui = !draw_gui;
                    space_was_pressed = true;
                }
            } else {
                space_was_pressed = false;
            }

            if (context.keyboard_status.value()[GL::Key::Escape]) {
                context.window->Close();
            }
        }

        // FIX THIS PART
        auto car_collision_intersector = context.car_model->GetCollisionIntersector();
        auto car_ray_intersector = context.car_model->GetRayIntersector();

        if (car_collision_intersector) {
            car_collision_intersector->ClearObstacles();

            for (auto obstacle : context.obstacles) {
                car_collision_intersector->AddObstacles(obstacle.get());
            }
        }
        if (car_ray_intersector) {
            car_ray_intersector->ClearObstacles();

            for (auto obstacle : context.obstacles) {
                car_ray_intersector->AddObstacles(obstacle.get());
            }
        }
        // FIX THIS PART

        context.camera->Move(delta_time);
        if (context.keyboard_mode.value() == App::KeyboardMode::NN_LEARNING) {
            nn_trainer.TrainingStep(delta_time);
        } else if (context.keyboard_mode.value() == App::KeyboardMode::NN_TEST) {
            nn_trainer.TrainingStep(delta_time);
        } else {
            context.car_model->Move(delta_time);
        }

        // for (int model_index = 0; model_index < context.obstacles.size(); ++model_index) {
        //     auto intersection_result = car_collision_intersector->GetIntersectedObstacleMeshIndices(model_index);
        //     for (auto mesh_index : intersection_result) {
        //         context.obstacles[model_index]->DrawBBoxOnCollision(mesh_index);
        //     }
        // }

        gl.Clear(GL::Buffer::Color | GL::Buffer::Depth);
        if (draw_gui) {
            gui.Prepare();
        }

        context.car_model->Draw();
        context.skybox->Draw();
        for (auto env_object : context.env) {
            env_object->Draw();
        }
        for (auto obstacle : context.obstacles) {
            obstacle->Draw();
        }

        if (draw_gui) {
            gui.Draw();
        }

        context.camera->UpdateMatrix();

        context.window->Present();
        App::LimitMaxFps(window_config);
    }

    if (draw_gui) {
        gui.Cleanup();
    }
    nn_trainer.SaveModel();

    return 0;
}
catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 0;
}