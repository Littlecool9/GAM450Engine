/**
 * @file Camera.h
 * @author Dong Ta
 * @brief Component for camera functionality
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include "Engine/EntitySystem/Component/Component.h"
#include "Engine/Renderer/PostProcessStack.h"

namespace mts
{
    class CameraComponent : public Component
    {
       public:
        CameraComponent();
        CameraComponent(GameObject* owner);
        ~CameraComponent() override = default;

        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

        void Init() override;
        void Update(float dt) override;
        Component* Clone(GameObject* newOwner) const override;

        bool isActive;

        // Near/far for ortho
        float nearPlane;
        float farPlane;

        // Half-height in world units. Controls how much of the world is visible.
        // Smaller = zoomed in, larger = zoomed out.
        float orthoSize;

        // Post-processing effect stack
        PostProcessStack& GetPostProcessStack() { return mPostProcessStack; }
        const PostProcessStack& GetPostProcessStack() const { return mPostProcessStack; }

       private:
        glm::mat4 BuildView2D();
        glm::mat4 BuildProjFromViewport() const;

        PostProcessStack mPostProcessStack;
    };

}  // namespace mts
