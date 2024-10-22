#pragma once

// STL
#include <algorithm>
#include <exception>
#include <cmath>

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <accelerator/accelerator_fwd.hpp>

namespace App {

class Accelerator {
public:
    Accelerator(const float max_speed, const float acceleration);

    void IncreaseSpeed(const float delta_time, const bool move_front);
    void DecreaseSpeed(const float delta_time);
    void Stop();
    const float GetSpeed() const;
    const bool WasStopped() const;

private:
    const float max_speed_;
    const float acceleration_;

    float cur_speed_;

    // Check if car was stopped by collision
    bool was_stopped_; 

    friend class Gui; // access to private variables
};

} // namespace App