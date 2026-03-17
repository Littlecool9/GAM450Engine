/**
 * @file EnemyState.cpp
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EnemyState.h"

namespace mts
{

    // The scoring functions are designed to evaluate how well the current state of the enemy aligns
    // with its goals, based on its preferences. The baseMultiplier can be used to scale the scores
    // as needed, and additional modifiers can be added to make the scoring more nuanced.
    float EnemyState::GetPlayerEliminationScore(EnemyPreference preference,
                                                float baseMultiplier) const
    {
        float score = 0.f;
        // If player is going to be eliminated, this is the end goal state, which should have the
        // highest score. We can also add a modifier based on how quickly we eliminate the player,
        // but for now we just give it a flat high score.
        if (GetPlayerHealthLeftRatio()<= 0)
        {
            score = baseMultiplier * baseMultiplier;
        }
        else
        {
            score = preference[EnemyGoal::PlayerElimination] *
                    (1.0f - GetDistanceToPlayer()) /
                    (1.0f - static_cast<float>(GetEnemyHealth()));
        }

        // can add more modifiers as we go on
        return score;
    }

    float EnemyState::GetSelfPreservationScore(EnemyPreference preference,
                                               float baseMultiplier) const
    {
        float score = 0.f;
        score = preference[EnemyGoal::SelfPreservation] *
                (GetDistanceToPlayer() / 100.0f) *
                (static_cast<float>(GetEnemyHealth()) /
             100.0f);
        //can add more modifiers as we go on
        return score;
    }

    float EnemyState::GetScoreTowardsGoals(EnemyPreference preference) const
    {
        float baseMultiplier = 10;
        float result = 0.f;
        float PEScore = GetPlayerEliminationScore(preference, baseMultiplier);
        float SPScore = GetSelfPreservationScore(preference, baseMultiplier);
        result += PEScore * preference[EnemyGoal::PlayerElimination];
        result += SPScore * preference[EnemyGoal::SelfPreservation];
        return result;
    }
}  // namespace mts