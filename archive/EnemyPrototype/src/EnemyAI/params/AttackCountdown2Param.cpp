/**
 * @file AttackCountdown2.cpp
 * @author cyclo
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "AttackCountdown2Param.h"

namespace mts
{
    void AttackCountdown2Param::Update(float dt, GameObject* owner)
    {
        float curr = std::get<float>(GetValue());
        if (curr > 0.0f)
        {
            SetValue(curr - dt);
        }

    }
}  // namespace mts