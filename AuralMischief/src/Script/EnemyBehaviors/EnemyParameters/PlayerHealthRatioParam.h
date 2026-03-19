/**
 * @file PlayerHealthRatioParam.h
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
    class PlayerHealthRatioParam: public EnemyStateParameter
    {
       public:
        PlayerHealthRatioParam() { SetValue(0.5f); }
        void Update(float dt, GameObject* owner) override;
        float GetCooldownTime() { return CooldownTime; }
        void SetCooldownTime(float n) { CooldownTime = n; }

       private:
        float CooldownTime = 3.4f;
    };

}  // namespace mts