/**
 * @file AIPlan.cpp
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "AIPlan.h"

namespace mts
{
    AIPlan::AIPlan(std::vector<EnemyStep> steps, float practicalness)
    {
        mSteps = steps;
        mPracticalness = practicalness;
    }
}  // namespace mts