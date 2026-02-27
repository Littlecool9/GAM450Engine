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
    class Transform;

    class BoxCollider2D final : public Collider2D
    {
       public:
        ~BoxCollider2D() override;
        Component* Clone(GameObject* newOwner) const override;
        void DebugRender() override;

        ColliderType GetColliderType() const override { return ColliderType::Box; }

        void Deserialize(const JsonValue& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

        void SetLocalWidth(float width);
        void SetLocalHeight(float height);

        glm::vec2 GetCenter() const;
        glm::vec2 GetHalfExtents() const;
        glm::vec2 GetLocalSize() const;
        glm::vec2 GetUp() const;
        glm::vec2 GetRight() const;

        glm::vec2 GetOffset() const { return mOffset; }
        void SetOffset(glm::vec2 o) { mOffset = o; }

       private:
        float mWidth = 1.0f;
        float mHeight = 1.0f;
        float mHalfWidth = 0.5f;
        float mHalfHeight = 0.5f;
        glm::vec2 mOffset;
    };
}  // namespace mts