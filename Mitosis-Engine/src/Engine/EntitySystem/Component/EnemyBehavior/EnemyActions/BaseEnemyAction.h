/**
 * @file BaseEnemyAction.h
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this header
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <span>

#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyState.h"

namespace mts
{
    class EnemyAgent;

    enum EnemyActionResult
    {
        Invalid = -1,
        OnGoing,
        Success,
        Failed
    };

    class BaseEnemyAction
    {
       public:
        BaseEnemyAction() = default;
        virtual ~BaseEnemyAction() = default;
        virtual EnemyState GetExpectedNextState(const EnemyState& state,
                                                const EnemyPreference& preferences) const = 0;
        ;
        virtual float GetEstimatedTimeCost(const EnemyState& state,
                                           const EnemyPreference& preferences) const = 0;
        ;

        // The function to be called when start executing the action
        // The input variable cannot be modified
        // If you need to apply changes to the agent, apply to
        // agent.GetComponent<AnyOtherComponent>()
        virtual void OnBeginning(EnemyAgent& agent) {}

        virtual bool IsAvailable(const EnemyState& state,
                                 const EnemyPreference& preferences) const = 0;
        ;

        // Override this if you don't want it be adaptive or want fields to be selectively adaptive,
        // such as movement action
        virtual const std::vector<EnemyGoal>& AdaptiveHeuristics() const
        {
            static const std::vector<EnemyGoal> goals = {EnemyGoal::PlayerElimination,
                                                         EnemyGoal::SelfPreservation};

            return goals;
        }

        void AdaptiveHeuristicUpdate(const EnemyPreference& pref,
                                     const EnemyState& savedState,
                                     const EnemyState& actualFinalState);
        virtual EnemyActionResult Update(EnemyAgent* agent, float dt, float totalTime) = 0;
        virtual void OnEnd(EnemyAgent& agent, EnemyActionResult result) = 0;



       private:
        std::unordered_map<EnemyGoal, float> mHeuristicAdaptiveDic;
        std::string Name;
        int mAdaptiveCounter = 0;
        const int mAdaptiveTimeSlice = 5;
        float mLastTweak = 1;
    protected:
        // Override this to get a different learning rate. Should be a small number. It's just a
        // magic number for now
        virtual float LearningRate() const { return 0.1f; }

        virtual std::unordered_map<EnemyGoal, float> InternalHeuristic(
            const EnemyState& state,
            const EnemyPreference& preference) = 0;
    public:
        std::unordered_map<EnemyGoal, float> UtilityHeuristic(const EnemyState& state,
                                                              const EnemyPreference& preference);
    };

}  // namespace mts