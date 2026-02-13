/**
 * @file RotateAction.h
 * @author Dong Ta
 * @brief  Rotates an object to X degrees over y time
 * @version 0.1
 * @date 1/8/2026 7:06:27 AM
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once
#include "BaseAction.h"

namespace mts
{
    class RotateAction : public BaseAction
    {
       public:
        float startRotation;
        float targetRotation;
        RotateAction(float Goal,
                     GameObject* target,
                     int id,
                     float lifeSpan,
                     float delay,
                     bool block,
                     EaseType ease);
        ~RotateAction() override = default;
        ActionResult Update(float dt) override;
    };

}
