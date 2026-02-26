/**
 * @file EnemyStep.h
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "EnemyActions/BaseEnemyAction.h"

namespace mts
{
    class EnemyStep
    {
       public:
        EnemyStep(BaseEnemyAction* action) { mAction = action; }


        BaseEnemyAction* GetAction() const { return mAction; }

        void SetAction(BaseEnemyAction* action) { mAction = action; }
       private:
        BaseEnemyAction* mAction;
    };

}  // namespace mts