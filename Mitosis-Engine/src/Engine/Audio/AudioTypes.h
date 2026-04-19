/**
 * @file AudioTypes.h
 * @author rahul
 * @brief Audio types and associated configurations
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace mts
{
    // TODO: Needs to be refactored once the new Event ID system is created
    using AudioEventId = std::string;

    enum class StopMode : uint8_t
    {
        AllowFadeOut,
        Immediate
    };

    // Opaque handle used by gameplay/engine.
    // Never expose FMOD pointers outside the backend.
    struct AudioHandle
    {
        uint32_t value = 0;  // 0 = invalid
        explicit operator bool() const { return value != 0; }
    };

    // In 2D: set z = 0, forward/up can be default.
    struct SpatialAttributes
    {
        glm::vec3 pos{};
        glm::vec3 vel{};                   // optional; can be zero if need be
        glm::vec3 forward{0.f, 0.f, 1.f};  // arbitrary but consistent
        glm::vec3 up{0.f, 1.f, 0.f};
    };

    // Definition of how "2D world sound" is mapped to stereo output
    struct Spatial2DSettings
    {
        float panRange = 10.f;     // world units corresponding to full L/R
        float minDistance = 1.f;   // inside this distance -> full volume
        float maxDistance = 25.f;  // beyond -> inaudible
        float rolloff = 1.f;       // exponent for attenuation curve (>1 steeper)
        bool enable = true;
    };

    struct AudioConfig
    {
        int maxVirtualVoices = 1024;  // FMOD virtual voices
        int maxRealVoices = 64;       // "real" mixing voices
        bool liveUpdate = false;      // FMOD live update opens TCP 9264 (firewall prompt)
    };

    // Convenience 2D helper
    inline SpatialAttributes Make2DAttributes(glm::vec2 pos, glm::vec2 vel = {})
    {
        SpatialAttributes a;
        a.pos = {pos.x, pos.y, 0.f};
        a.vel = {vel.x, vel.y, 0.f};
        return a;
    }

}  // namespace mts