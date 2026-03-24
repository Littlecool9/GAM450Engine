/**
 * @file Time.cpp
 * @author rahul
 * @brief Engine timekeeping (frame delta + fixed timestep accumulator) with optional frame-rate
 * limiting
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "Time.h"
namespace mts
{
    void Time::Init()
    {
        s_initialized = true;
        s_paused = false;

        s_lastTick = Clock::now();
        s_nextFrame = Clock::now();
        s_frameStart = s_lastTick;

        s_frameDt = 0.0;
        s_unclampedFrameDt = 0.0;
        s_timeSinceStart = 0.0;

        s_accumulator = 0.0;
    }

    void Time::EnsureInit()
    {
        if (!s_initialized)
            Init();
    }

    void Time::BeginFrame()
    {
        EnsureInit();

        s_frameStart = Clock::now();

        const auto now = s_frameStart;
        const std::chrono::duration<double> delta = now - s_lastTick;
        s_lastTick = now;

        s_unclampedFrameDt = std::max(0.0, delta.count());
        s_frameDt = std::clamp(s_unclampedFrameDt, 0.0, s_maxFrameDt);

        if (!s_paused)
        {
            const double scaled = s_frameDt * static_cast<double>(s_timeScale);
            s_timeSinceStart += scaled;
            s_accumulator += scaled;
        }
    }

    void Time::EndFrame()
    {
        EnsureInit();

        if (s_targetFps <= 0)
        {
            // unlimited: reset scheduler anchor so it doesn't "catch up" later
            s_nextFrame = Clock::now();
            return;
        }

        const auto frameStep =
            std::chrono::nanoseconds((long long) (1'000'000'000LL / s_targetFps));

        // Initialize nextFrame if this is first time or we fell behind a lot
        auto now = Clock::now();
        if (s_nextFrame < now - frameStep)
            s_nextFrame = now;

        s_nextFrame += frameStep;

        // Sleep until close to deadline (sleep_until is better than sleep_for)
        // Leave a small margin for scheduler jitter
        constexpr auto margin = std::chrono::microseconds(500);  // 0.5ms
        now = Clock::now();

        if (now + margin < s_nextFrame)
            std::this_thread::sleep_until(s_nextFrame - margin);

        // Finish with a short spin/yield for precision
        while (Clock::now() < s_nextFrame)
            std::this_thread::yield();
    }

    // ---- Getters ----
    double Time::FrameDt()
    {
        return s_frameDt * static_cast<double>(s_timeScale);
    }
    double Time::UnscaledDt()
    {
        return s_frameDt;
    }
    double Time::UnclampedFrameDt()
    {
        return s_unclampedFrameDt;
    }
    double Time::TimeSinceStart()
    {
        return s_timeSinceStart;
    }
    double Time::FixedDt()
    {
        return s_fixedDt;
    }

    double Time::Alpha()
    {
        if (s_paused || s_fixedDt <= 0.0)
            return 0.0;

        return std::clamp(s_accumulator / s_fixedDt, 0.0, 1.0);
    }

    // ---- Controls ----
    void Time::SetPaused(bool paused)
    {
        if (s_paused && !paused)
        {
            s_lastTick = Clock::now();
            s_frameStart = s_lastTick;
        }
        s_paused = paused;
    }

    bool Time::IsPaused()
    {
        return s_paused;
    }

    void Time::SetTimeScale(float scale)
    {
        const float clamped = std::max(0.0f, scale);
        if (s_timeScale == 0.0f && clamped != 0.0f)
        {
            // Reset clock anchor to avoid a dt spike on resume
            s_lastTick = Clock::now();
        }
        s_timeScale = clamped;
    }

    float Time::GetTimeScale()
    {
        return s_timeScale;
    }

    void Time::SetFixedDt(double seconds)
    {
        s_fixedDt = std::max(0.0, seconds);
        s_accumulator = std::min(s_accumulator, s_fixedDt);
    }

    void Time::SetMaxFrameDt(double seconds)
    {
        s_maxFrameDt = std::max(0.0, seconds);
    }

    void Time::SetMaxSubSteps(int steps)
    {
        s_maxSubSteps = std::max(1, steps);
    }

    void Time::SetTargetFps(int fps)
    {
        s_targetFps = fps;
    }

    int Time::TargetFps()
    {
        return s_targetFps;
    }

    void Time::SetSleepMarginMicroseconds(int us)
    {
        s_sleepMargin = std::chrono::microseconds(std::max(0, us));
    }

    void Time::ClearAccumulator()
    {
        s_accumulator = 0.0;
    }
}  // namespace mts
