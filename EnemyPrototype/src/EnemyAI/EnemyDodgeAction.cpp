/**
 * @file EnemyDodgeAction.cpp
 * @author cyclo
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EnemyDodgeAction.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/RigidBody2D.h"

namespace mts
{
    bool EnemyDodgeAction::IsAvailable(const EnemyState& state,
                                       const EnemyPreference& /*preferences*/) const
    {
        if (state.GetDodgeCountdown() > 0 || state.GetDistanceToPlayer() > 5 || state.GetCurrentPlayerState() != AnimationSystem::States::Attack1)
        {
            return false;
        }
        return true;
    }

    void EnemyDodgeAction::OnBeginning(EnemyAgent& agent)
    {
        agent.GetState().SetInAction(true);

        // Determine opposite facing direction
        const glm::vec2 enemyPos = agent.GetGameObject()->GetComponent<Transform>()->GetPosition();
        const glm::vec2 playerPos =
            agent.GetPlayerReference()->GetComponent<Transform>()->GetPosition();

        const float deltaX = playerPos.x - enemyPos.x;
        const float backwardDir = (deltaX >= 0.0f) ? -1.0f : 1.0f;  // opposite of player directionqqq

        agent.GetGameObject()->GetComponent<RigidBody2D>()->AddImpulse({backwardDir * mDodgeForceX, mDodgeForceY});
        agent.GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
            AnimationSystem::States::Falling);
    }

    EnemyActionResult EnemyDodgeAction::Update(EnemyAgent* agent,
                                               float dt,
                                               float totalTime)
    {
        if (!agent)
            return Invalid;

        if (totalTime >= mDuration)
        {
            agent->GetState().SetInAction(false);
            agent->GetState().SetDodgeCountdown(agent->GetState().GetDodgeCooldownTime());  // apply dodge cooldown
            agent->GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                AnimationSystem::States::Idle);
            return Success;
        }

        return OnGoing;
    }

    float EnemyDodgeAction::GetEstimatedTimeCost(const EnemyState& state,
                                       const EnemyPreference& /*preferences*/) const
    {

        return 0.4f;
    }
    
    EnemyState EnemyDodgeAction::GetExpectedNextState(const EnemyState& state,
                                            const EnemyPreference& /*preferences*/) const
    {
        EnemyState newState = state;
        newState.SetDodgeCountdown(state.GetDodgeCooldownTime());
        return newState;
    }

    void EnemyDodgeAction::OnEnd(EnemyAgent& agent, EnemyActionResult /*result*/)
    {
        // Set dodge cooldown
        agent.GetState().SetDodgeCountdown(agent.GetState().GetDodgeCooldownTime());
    }

     std::unordered_map<EnemyGoal, float> EnemyDodgeAction::InternalHeuristic(const EnemyState& state,
         const EnemyPreference& /*preference*/)
     {
         std::unordered_map<EnemyGoal, float> heuristic;
         // Heuristic: if player is close, dodging is more favorable for self-preservation
         heuristic[EnemyGoal::SelfPreservation] = state.GetDistanceToPlayer() < 4.0f ? 1.0f : 0.5f;
         heuristic[EnemyGoal::PlayerElimination] = 0.2f;  // dodging doesn't directly contribute to offense
         return heuristic;
     }
}  // namespace mts