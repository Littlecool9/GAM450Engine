/**
 * @file Transform.h
 * @author sumin.park
 * @brief  Transform Component
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
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
        Transform(GameObject* owner);
        Transform();
        ~Transform() override = default;

       public:
        void Init() override;
        void Update(float dt) override;
        void LateUpdate(float dt) override;
        void Render() override;

        const glm::vec3& GetPosition() const;
        void SetPosition(const glm::vec3& pos);

        const glm::vec3& GetScale() const;
        void SetScale(const glm::vec3& s);

        float GetRotation() const;
        void SetRotation(float r);

        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

        glm::mat4 LocalMatrix() const;

       private:
        glm::vec3 position;
        glm::vec3 scale;
        float rotation;
        bool isDirty;
    };

}  // namespace mts