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
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "../EnemyParameters//AttackCountdownParam.h"

namespace mts
{
    bool EnemyDodgeAction::IsAvailable(const EnemyState& state,
                                       const EnemyPreference& /*preferences*/) const
    {
        if (std::get<float>(state.params.at(HashAction("DodgeCooldown"))->GetValue()) > 0 ||
            state.GetDistanceToPlayer() > 6 ||
            (state.GetCurrentPlayerState() != AnimationSystem::States::Attack1 && state.GetCurrentPlayerState() != AnimationSystem::States::Attack2))
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
            // apply dodge cooldown
            agent->GetState()
                .params.at(HashAction("DodgeCooldown"))
                ->SetValue(dynamic_cast<AttackCountdownParam*>(
                               agent->GetState().params.at(HashAction("DodgeCooldown")))
                               ->GetCooldownTime());
            agent->GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                AnimationSystem::States::Walking);
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
        newState
            .params.at(HashAction("DodgeCooldown"))
            ->SetValue(dynamic_cast<AttackCountdownParam*>(
                           newState.params.at(HashAction("DodgeCooldown")))
                           ->GetCooldownTime());
        
        return newState;
    }

    void EnemyDodgeAction::OnEnd(EnemyAgent& agent, EnemyActionResult /*result*/)
    {
        // Set dodge cooldown
        agent.GetState()
            .params.at(HashAction("DodgeCooldown"))
            ->SetValue(dynamic_cast<AttackCountdownParam*>(
                           agent.GetState().params.at(HashAction("DodgeCooldown")))
                           ->GetCooldownTime());
    }

     std::unordered_map<EnemyGoal, float> EnemyDodgeAction::InternalHeuristic(const EnemyState& state,
         const EnemyPreference& /*preference*/)
     {
         std::unordered_map<EnemyGoal, float> heuristic;
         // Heuristic: if player is close, dodging is more favorable for self-preservation
         heuristic[EnemyGoal::SelfPreservation] =
             state.GetDistanceToPlayer() < 4.0f
                 ? 1.0f
                 : 0.5f;
         heuristic[EnemyGoal::PlayerElimination] = 0.2f;  // dodging doesn't directly contribute to offense
         return heuristic;
     }
}  // namespace mts