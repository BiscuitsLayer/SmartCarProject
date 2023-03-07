#pragma once

// Sources
#include <model/model.hpp>
#include <transform/transform.hpp>

namespace App {

class CarModel : public Model {
public:
    CarModel(GL::Program program, std::string path, 
    float car_move_speed = APP_CAR_MOVE_SPEED, float car_rotate_speed = APP_CAR_ROTATE_SPEED,
    GL::Vec3 car_center_translation = APP_CAR_CENTER_TRANSLATION)
    : Model(program, path), 
    car_move_speed_(car_move_speed), car_rotate_speed_(car_rotate_speed),
    car_center_translation_(GL::Mat4{}.Translate(car_center_translation)) {
    }

    void MoveFront(float delta_time) {
        movement_transform_.Translate(GL::Vec3(0.0f, 0.0f, car_move_speed_ * delta_time));
    }

    void MoveFrontLeft(float delta_time) {
        movement_transform_.Rotate(GL::Vec3(0.0f, 1.0f, 0.0f), car_rotate_speed_ * delta_time);
        MoveFront(delta_time);
    }

    void MoveFrontRight(float delta_time) {
        movement_transform_.Rotate(GL::Vec3(0.0f, 1.0f, 0.0f), -1.0f * car_rotate_speed_ * delta_time);
        MoveFront(delta_time);
    }

    void MoveBack(float delta_time) {
        movement_transform_.Translate(GL::Vec3(0.0f, 0.0f, -1.0f * car_move_speed_ * delta_time));
    }

    void MoveBackLeft(float delta_time) {
        movement_transform_.Rotate(GL::Vec3(0.0f, 1.0f, 0.0f), -1.0f * car_rotate_speed_ * delta_time);
        MoveBack(delta_time);
    }

    void MoveBackRight(float delta_time) {
        movement_transform_.Rotate(GL::Vec3(0.0f, 1.0f, 0.0f), car_rotate_speed_ * delta_time);
        MoveBack(delta_time);
    }

    virtual GL::Mat4 GetModelMatrix() override {
        return static_cast<GL::Mat4>(transform_) * movement_transform_ * car_center_translation_;
    }

private:
    float car_move_speed_;
    float car_rotate_speed_;

    GL::Mat4 car_center_translation_;
    GL::Mat4 movement_transform_;

    friend class Gui;
};

} // namespace App