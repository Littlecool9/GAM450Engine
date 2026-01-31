/**
 * @file CircleCollider2D.h
 * @author sumin.park
 * @brief  CircleCollider2D Component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

#include <glm/vec2.hpp>

#include "Collider2D.h"
namespace mts
{
    class CircleCollider2D final : public Collider2D
    {
       public:
        ~CircleCollider2D() override;

        ColliderType GetColliderType() const override { return ColliderType::Circle; }
        bool IsColliding(const Collider2D& collider) override;

       private:
        float mRadius = 1.0f;
        glm::vec2 mOffset;
    };

}  // namespace mts