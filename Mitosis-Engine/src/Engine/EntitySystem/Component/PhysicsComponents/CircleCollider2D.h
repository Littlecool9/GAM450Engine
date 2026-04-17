/**
 * @file CircleCollider2D.h
 * @author sumin.park
 * @brief  CircleCollider2D Component
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
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
        Component* Clone(GameObject* newOwner) const override;

        void DebugRender() override;
        ColliderType GetColliderType() const override { return ColliderType::Circle; }

        void Deserialize(const JsonValue& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

        void SetLocalRadius(float radius) { mRadius = radius; }

        glm::vec2 GetCenter() const;
        float GetLocalRadius() { return mRadius; }
        float GetRadius() const;

        glm::vec2 GetOffset() const { return mOffset; }
        void SetOffset(glm::vec2 o) { mOffset = o; }

       private:
        float mRadius = 1.0f;
        glm::vec2 mOffset;
    };

}  // namespace mts