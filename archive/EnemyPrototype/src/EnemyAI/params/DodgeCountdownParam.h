/**
 * @file DodgeCountdownParam.h
 * @author cyclo
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyStateParameter.h"

namespace mts
{
    class DodgeCountdownParam : public EnemyStateParameter
    {
    public:
        DodgeCountdownParam() { SetValue(0.0f); }
        void Update(float dt, GameObject* owner) override;
        float GetCooldownTime() { return CooldownTime; }

       private:
        float CooldownTime = 4.5f;
    };

}  // namespace mts