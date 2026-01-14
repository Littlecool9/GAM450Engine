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
#include "Engine/Math/Mat3.h"
#include "Engine/Math/Vec3.h"

namespace mts
{
    class Transform : public Component
    {
       public:
        Transform(GameObject* owner);
        ~Transform() override = default;

       public:
        void Init() override;
        void Update(float dt) override;
        void LateUpdate(float dt) override;
        void Render() override;

        const math::Vec3& GetPosition() const;
        void SetPosition(const math::Vec3& pos);

        const math::Vec3& GetScale() const;
        void SetScale(const math::Vec3& s);

        const float GetRotation() const;
        void SetRotation(float r);


        void Deserialize(JsonValue const& jsonObj) override;
        math::Mat3 LocalMatrix() const;

       private:
        math::Vec3 position;
        math::Vec3 scale;
        float rotation;
        bool isDirty;
    };

}  // namespace mts