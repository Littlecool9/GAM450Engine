/**
 * @file EnemyState.h
 * @author Dong Ta
 * @brief // Contain information on current enemy state
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "EnemyPreference.h"

namespace mts
{
    class EnemyState
    {
       public:

        float GetDistanceToPlayer() const { return mDistanceToPlayer; }
        void SetDistanceToPlayer(float distance) { mDistanceToPlayer = distance; }
        bool IsMeleeAnimation() const { return mMeleeAnimation; }
        void SetMeleeAnimation(bool isMelee) { mMeleeAnimation = isMelee; }
        int GetEnemyHealth() const { return mEnemyHealth; }
        void SetEnemyHealth(int health) { mEnemyHealth = health; }
        float GetPlayerHealthLeftRatio() const { return mPlayerHealthLeftRatio; }
        void SetPlayerHealthLeftRatio(float ratio) { mPlayerHealthLeftRatio = ratio; }
        float GetPlayerEliminationScore(EnemyPreference preference, float baseMultiplier) const;
        float GetSelfPreservationScore(EnemyPreference preference, float baseMultiplier) const;
        float GetScoreTowardsGoals(EnemyPreference preference) const;
        bool IsEndGoalState() const { return mPlayerHealthLeftRatio <= 0; }
       private:
        float mPlayerHealthLeftRatio = 0.0f;
        float mMovementSpeed = 0.0f;
        float mDistanceToPlayer = 0.0f;
        bool mMeleeAnimation = false;
        int mEnemyHealth = 0;
    };
}  // namespace mts