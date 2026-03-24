/**
 * @file Time.h
 * @author rahul
 * @brief Engine timekeeping (frame delta + fixed timestep accumulator) with optional frame-rate
 * limiting
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once

#include <algorithm>
#include <chrono>
#include <thread>

namespace mts
{
    class Time
    {
       public:
        using Clock = std::chrono::steady_clock;

        // ---- Lifecycle ----
        static void Init();
        static void BeginFrame();
        static void EndFrame();

        // ---- Variable timestep ----
        static double FrameDt();          // clamped seconds, scaled by TimeScale
        static double UnscaledDt();       // clamped seconds, unaffected by TimeScale
        static double UnclampedFrameDt();
        static double TimeSinceStart();  // paused-aware

        // ---- Fixed timestep ----
        template <typename Fn>
        static void StepFixed(Fn&& simulateFixedStep)
        {
            if (s_paused)
                return;

            int steps = 0;

            while (s_accumulator >= s_fixedDt && steps < s_maxSubSteps)
            {
                simulateFixedStep(s_fixedDt);
                s_accumulator -= s_fixedDt;
                ++steps;
            }

            if (steps >= s_maxSubSteps)
            {
                s_accumulator = 0.0;
            }
        }

        static double FixedDt();
        static double Alpha();

        // ---- Controls ----
        static void SetPaused(bool paused);
        static bool IsPaused();

        static void  SetTimeScale(float scale);  // 0.0 = freeze, 1.0 = normal
        static float GetTimeScale();

        static void SetFixedDt(double seconds);
        static void SetMaxFrameDt(double seconds);
        static void SetMaxSubSteps(int steps);

        static void SetTargetFps(int fps);  // 0 = uncapped
        static int TargetFps();

        static void SetSleepMarginMicroseconds(int us);

        static void ClearAccumulator();

       private:
        static void EnsureInit();

       private:
        static inline bool  s_initialized = false;
        static inline bool  s_paused = false;
        static inline float s_timeScale = 1.0f;

        // Clock
        static inline Clock::time_point s_lastTick{};
        static inline Clock::time_point s_frameStart{};
        static inline Clock::time_point s_nextFrame{};

        // Frame timing
        static inline double s_unclampedFrameDt = 0.0;
        static inline double s_frameDt = 0.0;
        static inline double s_timeSinceStart = 0.0;

        // Fixed timestep
        static inline double s_fixedDt = 1.0 / 60.0;
        static inline double s_accumulator = 0.0;
        static inline int s_maxSubSteps = 5;

        // Clamp
        static inline double s_maxFrameDt = 0.1;

        // FPS cap
        static inline int s_targetFps = 0;
        static inline std::chrono::microseconds s_sleepMargin{1000};
    };
}  // namespace mts