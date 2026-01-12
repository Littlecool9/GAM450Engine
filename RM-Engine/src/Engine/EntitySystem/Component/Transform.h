/**
 * @file Transform.h
 * @author sumin.park
 * @brief  Transform Component
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - RM Engine
 *
 */

#pragma once
#include "Component.h"
#include "Engine/Math/Mat3.h"
#include "Engine/Math/Vec3.h"

namespace rm {
    class Transform : public Component {
       public:
        Transform(GameObject* owner);
        ~Transform() override = default;

       public:
        void Init() override;
        void Update(float dt) override;
        void LateUpdate(float dt) override;
        void Render() override;

        math::Mat3 LocalMatrix() const;

       private:
        math::Vec3 position;
        math::Vec3 scale;
        float rotation;
    };

}  // namespace rm