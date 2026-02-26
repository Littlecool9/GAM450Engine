/**
 * @file AIPlanner.h
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "EnemyState.h"

namespace mts
{
    class BaseEnemyAction;
    class EnemyPreference;
    class AIPlan;
    class EnemyState;

    class AIPlanner
    {
       public:
        AIPlanner() = default;
        AIPlanner(const AIPlanner&) = delete;
        AIPlanner& operator=(const AIPlanner&) = delete;

        AIPlanner(AIPlanner&&) noexcept = default;
        AIPlanner& operator=(AIPlanner&&) noexcept = default;

        AIPlan BuildPlan(const EnemyState& state, const EnemyPreference& preference, AIPlan& plan);

        static AIPlanner& GetPlannerById(const std::string& id);

        static std::string GetPlannerId(const AIPlanner& planner);

        // Action registration
        void AddAction(std::unique_ptr<BaseEnemyAction> action);  

        float EvaluateNodeUtility(const EnemyState& state,
                                  const EnemyPreference& preference,
                                  float estimatedTime,
                                  float currScore) const;
       private:

        struct AIPlanningNode
        {
            BaseEnemyAction* action = nullptr;    
            int parentClosedIndex = -1;  // index into closed list
            EnemyState estimatedState;                  
            float estimatedScore = 0.0f;
            float estimatedTime = 0.0f;
            int stepCounter = 0;
            float heuristic = 0.0f;

            float OverallUtility() const { return estimatedScore + heuristic; }
        };

        AIPlan SummarizePlanFromNodes(const AIPlanningNode& last,
                                      const EnemyPreference& preference,
                                      const std::vector<AIPlanningNode>& closed) const;
        static std::unique_ptr<AIPlanner> CreateInstanceById(const std::string& id);
        std::vector<std::unique_ptr<BaseEnemyAction>> mActions;
        // Planner cache
        static inline std::unordered_map<std::string, std::unique_ptr<AIPlanner>> mInstances;
    };

}  // namespace mts