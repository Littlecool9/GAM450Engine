/**
 * @file PatrolRadius.h
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
    class PatrolRadius: public EnemyStateParameter
    {
       public:
        PatrolRadius() { SetValue(0.0f); }

       private:
    };

}  // namespace mts