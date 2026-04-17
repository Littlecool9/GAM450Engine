/**
 * @file UITransform.h
 * @author sumin.park
 * @brief  transform component for screen space GameObjects
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Component.h"
#include "Engine/Renderer.h"
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace mts
{
    enum class UIAnchor
    {
        TopLeft,
        TopCenter,
        TopRight,
        MiddleLeft,
        Center,
        MiddleRight,
        BottomLeft,
        BottomCenter,
        BottomRight,
    };

    class UITransform : public Component
    {
       public:
        UITransform() = default;
        UITransform(GameObject* owner) : Component(owner) {}
        ~UITransform() override = default;

        Component* Clone(GameObject* newOwner) const override;
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

        UIAnchor GetAnchor() const;
        void SetAnchor(UIAnchor a);

        // position = anchor preset + pixel offset (Y-down)
        // (0,0) places the element center exactly on the anchor point
        const glm::vec2& GetOffset() const;
        void SetOffset(const glm::vec2& off);

        float GetDepth() const;
        void SetDepth(float d);

        // scaling is proportional to the reference resolution
        const glm::vec3& GetScale() const;
        void SetScale(const glm::vec3& s);

        float GetRotation() const;
        void SetRotation(float r);

        // returns actually rendered scale
        // use this when computing bounds or hit areas
        glm::vec3 GetEffectiveScale() const;

        // returns the element center in normalized viewport space
        // matching the coordinate space returned by Renderer::MouseToUISpace()
        glm::vec2 GetNormalizedCenter() const;

        glm::mat4 GetLocalMatrix();

       private:
        static glm::vec2 AnchorToNorm(UIAnchor a);
        static UIAnchor AnchorFromString(const char* s);
        static const char* AnchorToString(UIAnchor a);

        glm::mat4 mLocalMatrix = glm::identity<glm::mat4>();
        UIAnchor mAnchor = UIAnchor::Center;
        glm::vec2 mOffset = {0.0f, 0.0f};  // Y+ = down
        float mDepth = 0.0f;
        glm::vec3 mScale = {1.0f, 1.0f, 1.0f};
        float mRotation = 0.0f;
    };

}  // namespace mts
