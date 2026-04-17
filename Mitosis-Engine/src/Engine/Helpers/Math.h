/**
 * @file   Math.h
 * @author sumin.park
 * @brief  Math helpers
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace mts::math
{
    inline glm::vec3 ToVec3(const glm::vec2& v, float z = 0.0f)
    {
        return {v.x, v.y, z};
    }

    inline glm::vec2 ToVec2(const glm::vec3& v)
    {
        return {v.x, v.y};
    }

    static inline glm::vec2 AbsVec2(glm::vec2 v)
    {
        return {std::abs(v.x), std::abs(v.y)};
    }
}  // namespace mts::math