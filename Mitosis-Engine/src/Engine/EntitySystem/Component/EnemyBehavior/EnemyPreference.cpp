/**
 * @file EnemyPreference.cpp
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EnemyPreference.h"

namespace mts
{
    
    static std::unordered_map<EnemyGoal, float> DefaultGoalPreferences()
    {
        return {
            {EnemyGoal::PlayerElimination, 1.0f},
            {EnemyGoal::SelfPreservation, 0.25f},
            // add the rest of goals here
        };
    }


    EnemyPreference::EnemyPreference()
        : GoalPreferences(DefaultGoalPreferences()), Aggressiveness(0.0f), Practicalness(0.5f)
    {
        
    }

    
    EnemyPreference::EnemyPreference(std::unordered_map<EnemyGoal, float> map, float a, float p)
        : GoalPreferences(map), Aggressiveness(a), Practicalness(p)
    {
        if (GoalPreferences.empty())
        {
            GoalPreferences = {
                {EnemyGoal::PlayerElimination, 1.0f},
                {EnemyGoal::SelfPreservation, 0.25f},
                // add whatever goals
            };
            LOG_INFO("EnemyAI Initialized with default preferences");
        }
        GoalPreferences = map;
        Aggressiveness = a;
    }

    // This is a simple weighted sum heuristic evaluation, can be expanded to more complex functions
    // as needed
    float EnemyPreference::EvaluateHeuristic(std::unordered_map<EnemyGoal, float> heuristicMap, float estimatedCost) const
    {
        float result = 0.0f;
        for (EnemyGoal goal :
             {EnemyGoal::PlayerElimination, EnemyGoal::SelfPreservation})
        {
            result += GoalPreferences.at(goal) * heuristicMap.at(goal);
        }

        return result / std::pow(estimatedCost, Practicalness);

    }
}  // namespace mts