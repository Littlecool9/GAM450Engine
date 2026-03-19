/**
 * @file AggroRangeParam.h
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
    class AggroRangeParam: public EnemyStateParameter
    {
       public:
        AggroRangeParam() { SetValue(0.0f); }
       private:
    };

}  // namespace mts