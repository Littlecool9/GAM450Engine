/**
 * @file Camera.h
 * @author Dong Ta
 * @brief Component for camera functionality
 *        
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */


#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include "Engine/EntitySystem/Component/Component.h"



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
        void Render() override;

        bool isActive;

        // Near/far for ortho 
        float nearPlane;
        float farPlane;

       private:
        glm::mat4 BuildView2D();
        glm::mat4 BuildProjFromViewport() const;
        
    };

}
