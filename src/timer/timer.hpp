#pragma once

// STL
#include <chrono>

namespace App {

class Timer {
public:
    using Seconds = std::ratio<1>;
    using Milliseconds = std::ratio<1, 1'000>;
    using Microseconds = std::ratio<1, 1'000'000>;
    using Nanoseconds = std::ratio<1, 1'000'000'000>;

    using Clock = std::chrono::steady_clock;
    using DefaultResolution = Seconds;

    template <typename U, typename V>
    using Duration = std::chrono::duration<U, V>;

    Timer(): start_time(Clock::now()), previous_tick(Clock::now()) {
    }

    void Start() {
        if (!running) {
            running = true;
            start_time = Clock::now();
        }
    }

    void Lap() {
        lapping = true;
        lap_time = Clock::now();
    }

    template <typename T = DefaultResolution>
    double Stop() {
        if (!running) {
            return 0;
        }

        running = false;
        lapping = false;
        auto duration = Duration<double, T>(Clock::now() - start_time);
        start_time = Clock::now();
        lap_time = Clock::now();

        return duration.count();
    }

    bool IsRunning() const {
        return running;
    }

    template <typename T = DefaultResolution>
    double Elapsed() {
        if (!running) {
            return 0;
        }

        Clock::time_point start = start_time;

        if (lapping) {
            start = lap_time;
        }

        return Duration<double, T>(Clock::now() - start).count();
    }

    template <typename T = DefaultResolution>
    double Tick() {
        auto now = Clock::now();
        auto duration = Duration<double, T>(now - previous_tick);
        previous_tick = now;
        return duration.count();
    }

private:
    bool running = false;
    bool lapping = false;

    Clock::time_point start_time;
    Clock::time_point lap_time;
    Clock::time_point previous_tick;
};

} // namespace App