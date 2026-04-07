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
#include "EnemyStateParameter.h"
#include "Engine/EntitySystem/Component/AnimationSystem.h"
namespace mts
{
    using ParamID = uint32_t;

    constexpr ParamID HashAction(std::string_view str)
    {
        uint32_t hash = 2166136261u;  // FNV-1a
        for (char c : str)
        {
            hash ^= static_cast<uint8_t>(c);
            hash *= 16777619u;
        }
        return hash;
    }
    constexpr ParamID operator""_aid(const char* str, size_t len)
    {
        return HashAction({str, len});
    }

    class EnemyState
    {
       public:

        float GetDistanceToPlayer() const { return mDistanceToPlayer; }
        glm::vec2 GetVectorToPlayer() const { return mVectorToPlayer; }
        void SetDistanceToPlayer(float distance) { mDistanceToPlayer = distance; }
        void SetVectorToPlayer(const glm::vec2& vector) { mVectorToPlayer = vector; }
        int GetEnemyHealth() const { return mEnemyHealth; }
        void SetEnemyHealth(int health) { mEnemyHealth = health; }
        float GetPlayerHealthLeftRatio() const { return mPlayerHealthLeftRatio; }
        void SetPlayerHealthLeftRatio(float ratio) { mPlayerHealthLeftRatio = ratio; }
        bool GetInAction() const { return mInAction; }
        void SetInAction(bool inAction) { mInAction = inAction; }
        
        AnimationSystem::States GetCurrentPlayerState() const { return mCurrentPlayerState; }
        void SetCurrentPlayerState(AnimationSystem::States state) { mCurrentPlayerState = state; }
        float GetPlayerEliminationScore(EnemyPreference preference, float baseMultiplier) const;
        float GetSelfPreservationScore(EnemyPreference preference, float baseMultiplier) const;
        float GetScoreTowardsGoals(EnemyPreference preference) const;
        bool IsEndGoalState() const { return GetPlayerHealthLeftRatio() <= 0; }
        std::unordered_map<ParamID, EnemyStateParameter*> params;
       private:
        float mPlayerHealthLeftRatio = 0.0f;

        float mDistanceToPlayer = 0.0f;
        glm::vec2 mVectorToPlayer{0.0f, 0.0f};
        bool mInAction = false;
        int mEnemyHealth = 0;
        AnimationSystem::States mCurrentPlayerState = AnimationSystem::States::Idle;

    };
}  // namespace mts