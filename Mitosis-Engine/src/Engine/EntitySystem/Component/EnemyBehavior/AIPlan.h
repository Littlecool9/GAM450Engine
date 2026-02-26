/**
 * @file AIPlan.h
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "EnemyStep.h"

namespace mts
{
    class AIPlan
    {
       public:

        AIPlan(std::vector<EnemyStep> steps, float practicalness);
        float GetPracticalness() const { return mPracticalness; }
        void SetPracticalness(float practicalness) { mPracticalness = practicalness; }
        std::vector<EnemyStep> GetSteps() const { return mSteps; }
        void SetSteps(const std::vector<EnemyStep>& steps) { mSteps = steps; }
        bool Empty() const { return mSteps.empty(); }
       private:
        std::vector<EnemyStep> mSteps;
        float mPracticalness = 0.0f;  // how good this plan is in practice, can be used for learning
                                      // and tweaking the planner
    };

}  // namespace mts