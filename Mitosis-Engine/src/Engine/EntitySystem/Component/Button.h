/**
 * @file Button.h
 * @author Dong Ta
 * @brief  Component for Button functionality
 *         if the Button is clicked, it triggers a message that activates a function
 * @version 0.1
 * @date 1/15/2026 6:55:28 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include <glm/vec3.hpp>

#include "Component.h"

namespace mts
{
    class Button : public Component
    {
       public:
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;
        void Update(float dt) override;
        void Init() override;
        void TransformCheck();
        void MeshCheck();

       private:
        glm::vec3 ButtonBoundary{0.f};
        std::string OnClickEvent{""};
        glm::vec3 ButtonColor{1.f, 1.f, 1.f};
    };

}  // namespace mts
