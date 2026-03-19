/**
 * @file MovespeedParam.h
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
    class MoveSpeed : public EnemyStateParameter
    {
       public:
        MoveSpeed() { SetValue(0.0f); }
        float GetMoveSpeed() { return mMoveSpeed; }

       private:
        float mMoveSpeed = 3.f;
    };

}  // namespace mts