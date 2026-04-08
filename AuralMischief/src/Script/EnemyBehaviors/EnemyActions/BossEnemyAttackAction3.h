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
    class BossEnemyAttackAction3 : public BaseEnemyAction
    {
       public:
        BossEnemyAttackAction3() = default;
        ~BossEnemyAttackAction3() override = default;

        EnemyState GetExpectedNextState(const EnemyState& state,
                                        const EnemyPreference& preferences) const override;
        float GetEstimatedTimeCost(const EnemyState& state,
                                   const EnemyPreference& preferences) const override;
        void OnBeginning(EnemyAgent& agent) override;
        bool IsAvailable(const EnemyState& state,
                         const EnemyPreference& preferences) const override;
        EnemyActionResult Update(EnemyAgent* agent, float dt, float totalTime) override;
        void OnEnd(EnemyAgent& agent, EnemyActionResult result) override;

        void FireBullet(EnemyAgent& agent);

       private:
        std::unordered_map<EnemyGoal, float> InternalHeuristic(
            const EnemyState& state,
            const EnemyPreference& preference) override;
        bool mHasLunged = false;
        bool mHitboxSpawned = false;
        bool mHasPlunged = false;
        bool mHitbox2Spawned = false;
        uint32_t mConductorSubId = 0;  // for unsubscribing from conductor when action ends
    };

}  // namespace mts