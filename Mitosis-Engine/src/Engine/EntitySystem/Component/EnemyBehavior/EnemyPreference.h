/**
 * @file EnemyPreference.h
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "EnemyGoals.h"

namespace mts
{
    class EnemyPreference
    {
       public:
        std::unordered_map<EnemyGoal, float> GoalPreferences;
        float Aggressiveness;
        float Practicalness;
        // add more preference variables as needed

        //
        EnemyPreference();
        EnemyPreference(std::unordered_map<EnemyGoal, float> map, float a, float p);
        float operator[](EnemyGoal goal) const
        {
            return GoalPreferences.at(goal);  // or preferences[goal]
        }

        float EvaluateHeuristic(std::unordered_map<EnemyGoal, float> HeuristicMap,
                                float estimatedCost) const;


       private:
    };

}  // namespace mts