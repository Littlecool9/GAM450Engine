/**
 * @file Transform.h
 * @author sumin.park
 * @brief  Transform Component
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Component.h"
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/vec3.hpp>

namespace mts
{
    class Transform : public Component
    {
       public:
        Transform() = default;
        ~Transform() override = default;

       public:
        Component* Clone(GameObject* newOwner) const override;
        void DebugRender() override;

        const glm::vec3& GetPosition() const;
        void SetPosition(const glm::vec3& pos);
        void SetPosition(const glm::vec2& pos);

        const glm::vec3& GetScale() const;
        void SetScale(const glm::vec3& s);

        float GetRotation() const;
        void SetRotation(float r);

        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

        glm::mat4 GetLocalMatrix();
        glm::vec3 GetRight();
        glm::vec3 GetUp();

       private:
        glm::mat4 mLocalMatrix = glm::identity<glm::mat4>();
        glm::vec3 mPosition = {0.0, 0.0, 0.0};
        glm::vec3 mScale = {1.0, 1.0, 1.0};
        float mRotation = 0.0;
        bool mIsDirty = false;
    };

}  // namespace mts