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
#include "BaseAction.h"
#include "Engine/Math/Vec3.h"

namespace mts
{
    class TranslateAction : public BaseAction
    {
        public:
        math::Vec3 startPosition;
        math::Vec3 targetPosition;

        TranslateAction(math::Vec3 Goal,
                        GameObject* target,
                        int id,
                        float lifeSpan,
                        float delay,
                        bool block,
                        EaseType ease);
        ActionResult Update(float dt) override;
    };
} 
