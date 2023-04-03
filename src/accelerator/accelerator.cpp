#include "accelerator.hpp"

namespace App {

// Extern variables
extern const float APP_CAR_SPEED_EPS;

Accelerator::Accelerator(const float max_speed, const float acceleration)
    : max_speed_(max_speed), acceleration_(acceleration), cur_speed_(0.0f) {
    if (acceleration <= 0) {
        throw std::runtime_error("Wrong acceleration value given!");
    }
}

void Accelerator::IncreaseSpeed(const float delta_time, const bool move_front) {
    float new_speed = cur_speed_ + (move_front ? 1.0f : -1.0f) * acceleration_ * delta_time;
    if ((new_speed > -1.0f * max_speed_) && (new_speed < max_speed_)) {
        cur_speed_ = new_speed;
    }
    was_stopped_ = false;
}

void Accelerator::DecreaseSpeed(const float delta_time) {
    if (std::fabs(cur_speed_) <= APP_CAR_SPEED_EPS) {
        cur_speed_ = 0.0f;
        return;
    }
    if (cur_speed_ > 0.0) {
        cur_speed_ -= acceleration_ * delta_time;
    } else {
        cur_speed_ += acceleration_ * delta_time;
    }
}

void Accelerator::Stop() {
    cur_speed_ = 0.0f;
    was_stopped_ = true;
}

const float Accelerator::GetSpeed() const {
    return cur_speed_;
}

const bool Accelerator::WasStopped() const {
    return was_stopped_;
}

} // namespace App