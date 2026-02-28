/**
 * @file Collision2D.h
 * @author sumin.park
 * @brief  collision info struct passed onto collision callbacks
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <glm/vec2.hpp>

namespace mts
{
    class Collider2D;

    struct Collision2D
    {
        Collider2D* other = nullptr;  // the other object's collider
        Collider2D* self = nullptr;   // self collider
        glm::vec2 normal{};           // contact normal self -> other
        glm::vec2 contactPoint{};     // the contact point in world space
        float penetration = 0.0f;     // penetration depth (0 on exit events)
    };
}  // namespace mts
