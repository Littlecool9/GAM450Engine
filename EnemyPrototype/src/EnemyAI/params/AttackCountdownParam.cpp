/**
 * @file AttackCountdownParam.cpp
 * @author cyclo
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "AttackCountdownParam.h"


namespace mts
{
    void AttackCountdownParam::Update(float dt, GameObject* owner)
    {
        float curr = std::get<float>(GetValue());
        if (curr > 0.0f)
        {
            SetValue(curr - dt);
        }
    }
}  // namespace mts