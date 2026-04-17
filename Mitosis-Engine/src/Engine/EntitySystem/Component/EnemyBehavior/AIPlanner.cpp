/**
 * @file AIPlanner.cpp
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "AIPlanner.h"

#include <random>

#include "AIPlan.h"
#include "EnemyActionList.h"
#include "EnemyStep.h"

namespace mts
{
    // Static planner cache: plannerId -> planner instance
    AIPlanner& AIPlanner::GetPlannerById(const std::string& id)
    {
        auto it = mInstances.find(id);
        if (it == mInstances.end())
        {
            auto planner = CreateInstanceById(id);
            auto [newIt, inserted] = mInstances.emplace(id, std::move(planner));
            return *newIt->second;  
        }   
        return *it->second;  
    }

    std::string AIPlanner::GetPlannerId(const AIPlanner& planner)
    {
        for (const auto& [id, instance] : mInstances)
        {
            if (instance && instance.get() == &planner)
                return id;
        }
        return "";  // not found
    }

    // Action registration: adds an action to this planner's action list. Takes ownership of the
    // action.
    void AIPlanner::AddAction(std::unique_ptr<BaseEnemyAction> action)
    {
        if (!action)
            return;
        mActions.emplace_back(std::move(action)); 
    }

    // Creates a new planner instance based on the given plannerId. 
    std::unique_ptr<AIPlanner> AIPlanner::CreateInstanceById(const std::string& plannerId)
    {
        auto planner = std::make_unique<AIPlanner>();

        const ActionRegistry& registry = mts::EnemyActionList::Registry();

        auto it = registry.find(plannerId);
        if (it != registry.end())
        {
            for (const auto& makeAction : it->second)
            {
                planner->AddAction(makeAction());  //transfers ownership
            }
        }

        return planner;
    }

    // Summarizes a plan from the last node and the closed list. Walks backwards from `last` through
    AIPlan AIPlanner::SummarizePlanFromNodes(const AIPlanningNode& last,
                                             const EnemyPreference& preference,
                                             const std::vector<AIPlanningNode>& closed) const
    {
        std::vector<EnemyStep> steps;

        // Walk backwards from `last` through closed list indices
        const AIPlanningNode* node = &last;
        while (node && node->action != nullptr)
        {
            steps.emplace_back(node->action /*, any stored per-step metadata */);

            if (node->parentClosedIndex < 0)
                break;

            node = &closed[node->parentClosedIndex];
        }

        std::reverse(steps.begin(), steps.end());

        // Plan "utility" value: use bestNode.OverallUtility() or just estimatedScore
        const float planUtility = last.OverallUtility();
        return AIPlan(std::move(steps), planUtility);
    }

    // Higher utility means better plan. 
    float AIPlanner::EvaluateNodeUtility(const EnemyState& state,
                                         const EnemyPreference& preference,
                                         float estimatedTime,
                                         float currScore) const
    {
        // If this is an end goal state, return a very high utility to prioritize it
        if (state.IsEndGoalState())
            return 1'000'000.0f;

        const float denom = std::pow(std::max(estimatedTime, 0.0001f), preference.Practicalness);
        const float res = (state.GetScoreTowardsGoals(preference) - currScore) / denom;
        return res;
    }

    // Main planning function: returns the best plan found within limits. Can also return early if
    // we find an end goal state.
    AIPlan AIPlanner::BuildPlan(const EnemyState& state,
                                   const EnemyPreference& preference,
                                   AIPlan& plan)
    {
        // Safety caps
        constexpr int kMaxDepth = 6;
        constexpr int kMaxExpanded = 500;  // prevents worst-case blowups

        const float currScore = state.GetScoreTowardsGoals(preference);

        std::vector<AIPlanningNode> open;
        std::vector<AIPlanningNode> closed;
        open.reserve(128);
        closed.reserve(128);

        // root
        open.push_back(AIPlanningNode{
            .action = nullptr,
            .parentClosedIndex = -1,
            .estimatedState = state,
            .estimatedScore = 0.0f,
            .estimatedTime = 0.0f,
            .stepCounter = 0,
            .heuristic = 0.0f,
        });

        // Best plan found so far
        bool hasBest = false;
        AIPlanningNode bestNode{};
        float bestUtility = -std::numeric_limits<float>::infinity();

        int expanded = 0;

        while (!open.empty() && expanded < kMaxExpanded)
        {
            //Pop the best OverallUtility node (max)
            auto itMax = std::max_element(open.begin(),
                                          open.end(),
                                          [](const AIPlanningNode& a, const AIPlanningNode& b)
                                          { return a.OverallUtility() < b.OverallUtility(); });

            AIPlanningNode pop = *itMax;
            open.erase(itMax);

            // push into closed; remember its closed index for children
            const int parentClosedIndex = static_cast<int>(closed.size());
            closed.push_back(pop);
            ++expanded;

            // If too deep, don't expand further (but it can still be a candidate)
            if (pop.stepCounter >= kMaxDepth)
                continue;

            // Expand actions available from THIS node's estimatedState
            const EnemyState& s = pop.estimatedState;

            for (const auto& actionPtr : mActions)
            {
                BaseEnemyAction* action = actionPtr.get();
                if (!action || !action->IsAvailable(s, preference))
                    continue;

                const float stepTime = action->GetEstimatedTimeCost(s, preference);
                if (stepTime < 0.01f)  // avoid degenerate time
                    continue;

                EnemyState nextState = action->GetExpectedNextState(s, preference);

                const float totalTime = pop.estimatedTime + stepTime;
                const float score =
                    EvaluateNodeUtility(nextState, preference, totalTime, currScore);

                // heuristic exactly like before
                auto hMap = action->UtilityHeuristic(nextState, preference);
                const float h = preference.EvaluateHeuristic(hMap, stepTime);

                AIPlanningNode child;
                child.action = action;
                child.parentClosedIndex = parentClosedIndex;
                child.estimatedState = std::move(nextState);
                child.estimatedTime = totalTime;
                child.estimatedScore = score;
                child.heuristic = h;
                child.stepCounter = pop.stepCounter + 1;

                const float util = child.OverallUtility();

                // Track best candidate (either end-goal OR just best utility)
                if (!hasBest || util > bestUtility)
                {
                    hasBest = true;
                    bestUtility = util;
                    bestNode = child;
                }

                // Optional early exit for if we want "end goal wins immediately"
                if (child.estimatedState.IsEndGoalState())
                {
                    plan = SummarizePlanFromNodes(child, preference, closed);
                    return plan;
                }

                open.push_back(std::move(child));
            }
        }

        // If we never found anything useful, return empty plan
        if (!hasBest || bestNode.action == nullptr)
        {
            plan = AIPlan({}, 0.0f);
            return plan;
        }

        plan = SummarizePlanFromNodes(bestNode, preference, closed);
        return plan;
    }


}  // namespace mts