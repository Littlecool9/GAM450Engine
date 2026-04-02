/**
 * @file UITransform.h
 * @author sumin.park
 * @brief  Transform component for screen-space (HUD/UI) GameObjects.
 *         Position is in normalized [0,1] screen coordinates; scale in UI units.
 *         Independent of Transform.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Component.h"
#include "Engine/Renderer.h"
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/vec3.hpp>

namespace mts
{
    class UITransform : public Component
    {
       public:
        UITransform() = default;
        UITransform(GameObject* owner) : Component(owner) {}
        ~UITransform() override = default;

        Component* Clone(GameObject* newOwner) const override;
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

        const glm::vec3& GetPosition() const;
        void SetPosition(const glm::vec3& pos);
        void SetPosition(const glm::vec2& pos);

        const glm::vec3& GetScale() const;
        void SetScale(const glm::vec3& s);

        float GetRotation() const;
        void SetRotation(float r);

        glm::mat4 GetLocalMatrix();

       private:
        glm::mat4 mLocalMatrix = glm::identity<glm::mat4>();
        glm::vec3 mPosition    = {0.0f, 0.0f, 0.0f};
        glm::vec3 mScale       = {1.0f, 1.0f, 1.0f};
        float     mRotation    = 0.0f;
    };

}  // namespace mts
