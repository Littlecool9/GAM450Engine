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
#include "Engine/EntitySystem/Component/AnimationSystem.h"
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
        float GetAttackCooldownTime() const { return mAttackCooldownTime; }
        float GetAttackCountdown() const { return mAttackCountdown; }
        void SetAttackCountdown(float time) { mAttackCountdown = time; }
        float GetAttack2CooldownTime() const { return mAttack2CooldownTime; }
        void SetAttack2Countdown(float time) { mAttack2Countdown = time; }
        float GetAttack2Countdown() const { return mAttack2Countdown; }
        float GetMovementSpeed() const { return mMovementSpeed; }
        void SetMovementSpeed(float speed) { mMovementSpeed = speed; }
        bool GetInAction() const { return mInAction; }
        void SetInAction(bool inAction) { mInAction = inAction; }
        float GetDodgeCooldownTime() const { return mDodgeCooldownTime; }
        float GetDodgeCountdown() const { return mDodgeCountdown; }
        void SetDodgeCountdown(float time) { mDodgeCountdown = time; }
        AnimationSystem::States GetCurrentPlayerState() const { return mCurrentPlayerState; }
        void SetCurrentPlayerState(AnimationSystem::States state) { mCurrentPlayerState = state; }
        float GetPlayerEliminationScore(EnemyPreference preference, float baseMultiplier) const;
        float GetSelfPreservationScore(EnemyPreference preference, float baseMultiplier) const;
        float GetScoreTowardsGoals(EnemyPreference preference) const;
        bool IsEndGoalState() const { return mPlayerHealthLeftRatio <= 0; }
       private:
        float mPlayerHealthLeftRatio = 0.0f;
        float mMovementSpeed = 0.0f;
        float mDistanceToPlayer = 0.0f;
        float mAttackCooldownTime = 3.4f;
        float mAttackCountdown = 0.0f;
        float mAttack2CooldownTime = 4.0f;
        float mAttack2Countdown = 0.0f;
        float mDodgeCooldownTime = 4.5f;
        float mDodgeCountdown = 0.0f;
        bool mMeleeAnimation = false;
        bool mInAction = false;
        int mEnemyHealth = 0;
        AnimationSystem::States mCurrentPlayerState = AnimationSystem::States::Idle;
    };
}  // namespace mts