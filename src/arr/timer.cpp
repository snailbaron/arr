#include "timer.hpp"

#include <thread>

FrameTimer::FrameTimer(int fps)
    : _delta{1.0 / fps}
    , _frameDuration{std::chrono::duration_cast<Clock::duration>(
        std::chrono::duration<double>{_delta})}
{ }

double FrameTimer::delta() const
{
    return _delta;
}

void FrameTimer::reset()
{
    _start = Clock::now();
    _lastFrame = 0;
}

int FrameTimer::operator()()
{
    auto currentFrame = (Clock::now() - _start) / _frameDuration;
    auto framesPassed = static_cast<int>(currentFrame - _lastFrame);
    _lastFrame = currentFrame;
    return framesPassed;
}

void FrameTimer::relax() const
{
    std::this_thread::sleep_until(_start + _frameDuration * (_lastFrame + 1));
}
