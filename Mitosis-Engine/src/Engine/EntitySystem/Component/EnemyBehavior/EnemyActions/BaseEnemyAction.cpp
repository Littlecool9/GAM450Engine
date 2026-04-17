/**
 * @file BaseEnemyAction.cpp
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "BaseEnemyAction.h"

namespace mts
{

    void BaseEnemyAction::AdaptiveHeuristicUpdate(const EnemyPreference& pref,
                                                 const EnemyState& savedState,
                                                 const EnemyState& actualFinalState)
    {
        if (!AdaptiveHeuristics().empty())
        {
            auto h = UtilityHeuristic(savedState, pref);
            float actualUtility = actualFinalState.GetScoreTowardsGoals(pref);
            float expectedUtility =
                GetExpectedNextState(savedState, pref).GetScoreTowardsGoals(pref);
            float heuristicsUtility = pref.EvaluateHeuristic(h, 1);
            
            auto goal = AdaptiveHeuristics()[(mAdaptiveCounter / mAdaptiveTimeSlice) % AdaptiveHeuristics().size()];
            float diff = actualUtility - (expectedUtility + heuristicsUtility);

            mLastTweak = diff * LearningRate() * pref[goal];

            if (mHeuristicAdaptiveDic.contains(goal))
                mHeuristicAdaptiveDic[goal] += mLastTweak;
            else mHeuristicAdaptiveDic[goal] = mLastTweak;
            ++mAdaptiveCounter;
        }
    }

    std::unordered_map<EnemyGoal, float> BaseEnemyAction::UtilityHeuristic(const EnemyState& state, const EnemyPreference& preference)
    {
        auto heuristicDic = InternalHeuristic(state, preference);
        for (EnemyGoal goal : AdaptiveHeuristics())
        {
            if (heuristicDic.find(goal) != heuristicDic.end())
            {
                heuristicDic[goal] *= mHeuristicAdaptiveDic[goal];
            }
        }
        return heuristicDic;
    }
}  // namespace mts