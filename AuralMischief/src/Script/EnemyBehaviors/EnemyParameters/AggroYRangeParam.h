/**
 * @file AggroYRangeParam.h
 * @author Dong Ta
 * @brief Y-axis aggro range parameter for enemy state
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyStateParameter.h"

namespace mts
{
    class AggroYRangeParam: public EnemyStateParameter
    {
       public:
        AggroYRangeParam() { SetValue(0.0f); }
       private:
    };

}  // namespace mts
