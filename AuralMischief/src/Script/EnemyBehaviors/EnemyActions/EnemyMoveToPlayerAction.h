/**
 * @file EnemyMoveToPlayerAction.h
 * @author
 *
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyActions/BaseEnemyAction.h"

namespace mts
{
    class EnemyMoveToPlayerAction : public BaseEnemyAction
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
        std::unordered_map<EnemyGoal, float> InternalHeuristic(
            const EnemyState& state,
            const EnemyPreference& preference) override;

        float mGiveUpTime = 3.0f;  // fail if cannot reach in time
        float mMoveSpeed = 1.50f;  // units/sec
    };

}  // namespace mts