/**
 * @file BoxCollider2D.h
 * @author sumin.park
 * @brief  BoxCollider2D Component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <glm/vec2.hpp>

#include "Collider2D.h"
namespace mts
{
    class BoxCollider2D final : public Collider2D
    {
       public:
        ~BoxCollider2D() override;
        void DebugRender() override;

        ColliderType GetColliderType() const override { return ColliderType::Box; }
        bool IsColliding(const Collider2D& collider) override;

        void Deserialize(const JsonValue& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

       private:
        float mWidth = 1.0f;
        float mHeight = 1.0f;
        glm::vec2 mOffset;
    };
}  // namespace mts