/**
 * @file TranslateAction.h
 * @author Dong Ta
 * @brief  Moves an object to x position over y time
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include <glm/vec3.hpp>

#include "BaseAction.h"

namespace mts
{
    class TranslateAction : public BaseAction
    {
        public:
        glm::vec3 startPosition;
        glm::vec3 targetPosition;

        TranslateAction(glm::vec3 Goal,
                        GameObject* target,
                        int id,
                        float lifeSpan,
                        float delay,
                        bool block,
                        EaseType ease);
        ~TranslateAction() override = default;
        ActionResult Update(float dt) override;
    };
} 
