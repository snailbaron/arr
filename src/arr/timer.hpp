#pragma once

#include <chrono>
#include <cstddef>

class FrameTimer {
public:
    explicit FrameTimer(int fps);

    [[nodiscard]] double delta() const;

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

class Ticker {
public:
    explicit Ticker(int fps);
    void tick(double delta);
    [[nodiscard]] size_t frame() const;

private:
    double _secsPerFrame = 0.0;
    size_t _fullFrames = 0;
    double _lastFrameTime = 0.0;
};
