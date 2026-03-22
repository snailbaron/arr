#pragma once

#include <chrono>

class FrameTimer {
public:
    explicit FrameTimer(int fps);

    double delta() const;

    void reset();
    int operator()();

    void relax() const;

private:
    using Clock = std::chrono::steady_clock;

    double _delta = 0.0;
    Clock::duration _frameDuration;
    Clock::time_point _start = Clock::now();
    size_t _lastFrame = 0;
};
