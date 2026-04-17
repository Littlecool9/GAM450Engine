/**
 * @file AggroRangeParam.h
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
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