/**
 * @file ScaleAction.h
 * @author Dong Ta
 * @brief  Scales an object to x value over y time
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */
#pragma once
#include <glm/vec3.hpp>

#include "BaseAction.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"


namespace mts
{
    class ScaleAction : public BaseAction
    {
       public:
        glm::vec3 startScale;
        glm::vec3 targetScale; 
        ScaleAction(glm::vec3 Goal,
                    GameObject* target,
                    int id,
                    float lifeSpan,
                    float delay,
                    bool block,
                    EaseType ease);
        ~ScaleAction() override = default;
        ActionResult Update(float dt) override;
    };

}
