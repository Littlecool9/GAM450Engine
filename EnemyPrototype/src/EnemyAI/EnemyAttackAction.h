/**
 * @file EnemyAttackAction.h
 * @author cyclo
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyActions/BaseEnemyAction.h"

namespace mts
{
    class EnemyAttackAction : public BaseEnemyAction
    {
       public:
        EnemyAttackAction() = default;
        ~EnemyAttackAction() override = default;

        EnemyState GetExpectedNextState(const EnemyState& state,
                                        const EnemyPreference& preferences) const override;
        float GetEstimatedTimeCost(const EnemyState& state,
                                   const EnemyPreference& preferences) const override;
        void OnBeginning(const EnemyAgent& agent) override;
        bool IsAvailable(const EnemyState& state,
                         const EnemyPreference& preferences) const override;
        EnemyActionResult Update(const EnemyAgent agent, float dt, float totalTime) const override;
        void OnEnd(const EnemyAgent& agent, EnemyActionResult result) override;

       private:
        std::unordered_map<EnemyGoal, float> InternalHeuristic(const EnemyState& state,
            const EnemyPreference& preference) override;
    };

}  // namespace mts