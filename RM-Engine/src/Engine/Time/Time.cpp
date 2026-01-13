/**
 * @file Time.cpp
 * @author rahul
 * @brief Engine timekeeping (frame delta + fixed timestep accumulator) with optional frame-rate
 * limiting
 *
 * @copyright Copyright (c) 2026 - RM Engine
 *
 */

#include "pch.h"
#include "Time.h"

namespace rm
{
    void Time::Init()
    {
        s_initialized = true;
        s_paused = false;

        s_lastTick = Clock::now();
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
            s_timeSinceStart += s_frameDt;
            s_accumulator += s_frameDt;
        }
    }

    void Time::EndFrame()
    {
        EnsureInit();

        if (s_targetFps <= 0 || s_paused)
            return;

        const auto targetFrame = std::chrono::duration<double>(1.0 / double(s_targetFps));

        const auto now = Clock::now();
        auto elapsed = now - s_frameStart;

        if (elapsed < targetFrame)
        {
            auto remaining = targetFrame - elapsed;

            if (remaining > s_sleepMargin)
            {
                std::this_thread::sleep_for(remaining - s_sleepMargin);
            }

            while ((Clock::now() - s_frameStart) < targetFrame)
            {
                // optional: std::this_thread::yield();
            }
        }
    }

    // ---- Getters ----
    double Time::FrameDt()
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
}  // namespace rm
