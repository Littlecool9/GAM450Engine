/**
 * @file BeatMath.h
 * @author rahul
 * @brief Pure beat/bar arithmetic utilities with no engine dependencies.
 *        Used by ConductorSystem, BeatBarController, and unit tests.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 */
#pragma once
#include <cmath>
#include <utility>

namespace mts::BeatMath
{
    // Given song time (seconds), BPM, and beat-0 offset, return {beatIndex, beatPhase 0..1}.
    // songTimeSec < offsetSec is treated as pre-roll; returns {0, 0.f}.
    inline std::pair<int, float> BeatFromTime(float songTimeSec, float bpm, float offsetSec)
    {
        const float t          = songTimeSec - offsetSec;
        const float beatDurSec = 60.f / bpm;
        const float raw        = (t >= 0.f) ? (t / beatDurSec) : 0.f;
        const int   beat       = static_cast<int>(raw);
        return {beat, raw - static_cast<float>(beat)};
    }

    // Bar-level phase (0..1 over one measure), accelerated by speedMultiplier and wrapped.
    // With multiplier = 1.0, phase reaches 1.0 exactly at the bar boundary.
    // Guarded against beatsPerBar <= 0.
    inline float BarPhase(int beatInBar, float beatPhase, int beatsPerBar, float speedMultiplier)
    {
        if (beatsPerBar <= 0)
            return 0.f;
        const float base = (beatInBar + beatPhase) / static_cast<float>(beatsPerBar);
        return std::fmod(base * speedMultiplier, 1.0f);
    }
}  // namespace mts::BeatMath
