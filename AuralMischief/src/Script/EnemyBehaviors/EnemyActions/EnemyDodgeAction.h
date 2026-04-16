/**
 * @file EnemyDodgeAction.h
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyActions/BaseEnemyAction.h"

namespace mts
{
    class EnemyDodgeAction : public BaseEnemyAction
    {
       public:
        bool IsAvailable(const EnemyState& state,
                         const EnemyPreference& preferences) const override;
        float GetEstimatedTimeCost(const EnemyState& state,
                                      const EnemyPreference& preferences) const override;
        EnemyState GetExpectedNextState(const EnemyState& state,
                                           const EnemyPreference& preferences) const override;
        void OnBeginning(EnemyAgent& agent) override;
        EnemyActionResult Update(EnemyAgent* agent, float dt, float totalTime) override;
        void OnEnd(EnemyAgent& agent, EnemyActionResult result) override;


       private:
        std::unordered_map<EnemyGoal, float> InternalHeuristic(const EnemyState& state,
            const EnemyPreference& preference) override;

        float mDodgeForceX = 10.0f;  // horizontal impulse applied when dodging
        float mDodgeForceY = 8.0f;   // vertical impulse applied when dodging
        float mDuration = 0.5f;
    };

}  // namespace mts