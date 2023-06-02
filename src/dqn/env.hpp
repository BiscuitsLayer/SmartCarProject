#pragma once

// Torch
#include <torch/torch.h>

// Constants
#include <constants/constants.hpp>

// LibSmartCar
#include <helpers/helpers.hpp>

namespace AppNN {

class Environment {
public:
    void Step(float delta_time) {
        auto& context = App::Context::Get();
        context.car_model->Move(delta_time);

        for (int i = 0; i < App::APP_RAY_INTERSECTOR_RAYS_COUNT; ++i) {
            context.state[i] = isinf(context.distances_from_rays[i]) ? 100.0 : context.distances_from_rays[i];
        }

        GL::Vec3 cur_position = context.car_model->GetPosition();
        context.state[App::APP_RAY_INTERSECTOR_RAYS_COUNT + 0] = cur_position.X;
        context.state[App::APP_RAY_INTERSECTOR_RAYS_COUNT + 1] = cur_position.Y;
        context.state[App::APP_RAY_INTERSECTOR_RAYS_COUNT + 2] = cur_position.Z;

        float cur_speed = context.car_model->GetSpeed();
        context.state[App::APP_RAY_INTERSECTOR_RAYS_COUNT + 3] = cur_speed;
    }

    Reward GetReward() {
        auto& context = App::Context::Get();
        Reward ans = -1;

        static const GL::Vec3 final_destination = GL::Vec3(56.0, 0.0, 0.0);
        static GL::Vec3 prev_position = GL::Vec3(0.0, 0.0, 0.0);
        GL::Vec3 cur_position = context.car_model->GetPosition();

        float prev_distance = (prev_position - final_destination).Length();
        float cur_distance = (cur_position - final_destination).Length();

        float cur_speed = context.car_model->GetSpeed();
        if (std::fabs(cur_speed) < 2.0) {
            ans -= 1000;
        }

        if (std::fabs(cur_speed) > 2.0) {
            ans += 100;
        }

        // DO NOT RIDE TOO FAST
        // if (std::fabs(cur_speed) > 9.0) {
        //     ans -= 50;
        // }

        if (cur_distance > prev_distance) {
            ans -= 100;
        }

        if (cur_distance < prev_distance) {
            ans += 50;
        }

        return ans;
    }

    bool IsDone() const {
        auto& context = App::Context::Get();

        GL::Vec3 cur_position = context.car_model->GetPosition();
        static const GL::Vec3 final_destination = GL::Vec3(56.0, 0.0, 0.0);

        float cur_distance = (cur_position - final_destination).Length();

        if (cur_distance < 2.0) {
            return true;
        }
        return false;
    }
};

} // namespace AppNN