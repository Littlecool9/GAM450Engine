/**
 * @file EnemyMoveToPlayerAction.cpp
 * @author cyclo
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EnemyMoveToPlayerAction.h"

#include "Engine/EntitySystem/Component/AnimationSystem.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/RigidBody2D.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
namespace mts
{
    bool EnemyMoveToPlayerAction::IsAvailable(const EnemyState& state,
                                              const EnemyPreference& /*preferences*/) const
    {
        if (state.GetInAction() || state.GetDistanceToPlayer() <= mStopDistance)
            return false;

        return true;
    }

    float EnemyMoveToPlayerAction::GetEstimatedTimeCost(const EnemyState& state,
                                                        const EnemyPreference& /*preferences*/) const
    {
        // Simple heuristic: time = distance / speed, using the distance to player from state
        return state.GetDistanceToPlayer() / mMoveSpeed;
    }

    EnemyState EnemyMoveToPlayerAction::GetExpectedNextState(const EnemyState& state,
                                                      const EnemyPreference& /*preferences*/) const
    {
        EnemyState newState = state;
        // We expect to be close to the player after moving towards them, so we can set distance to a low value
        newState.SetDistanceToPlayer(mStopDistance * 0.5f);  // expected to be within stop distance
        return newState;
    }

    void EnemyMoveToPlayerAction::OnBeginning(EnemyAgent& agent)
    {
        agent.GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(AnimationSystem::States::Walking);
    }

    EnemyActionResult EnemyMoveToPlayerAction::Update(EnemyAgent* agent,
                                                      float dt,
                                                      float totalTime) 
    {
        if (!agent)
            return Invalid;

        //Get positions

        const glm::vec2 enemyPos = agent->GetGameObject()->GetComponent<Transform>()->GetPosition();         
        const glm::vec2 playerPos = agent->GetPlayerReference()->GetComponent<Transform>()->GetPosition();  

        const glm::vec2 toPlayer = playerPos - enemyPos;
        const float dist = glm::length(toPlayer);

        //check if arrived
        if (dist <= mStopDistance)
        {
            // Stop movement when reached
            agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));  // TODO: replace

            agent->GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                AnimationSystem::States::Idle);
            return Success;
        }

        //give up if too long
        if (totalTime >= mGiveUpTime)
        {
            // Stop movement on failure too
            agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));

            agent->GetState().SetInAction(false);
            agent->GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                AnimationSystem::States::Idle);
            return Failed;
        }

        //Move toward player (simple steering)
        glm::vec2 dir = (dist > 0.0001f) ? (toPlayer / dist) : glm::vec2{0.0f, 0.0f};
        if (dir.x < 0)
        {
            agent->GetGameObject()->GetComponent<SpriteRenderer>()->SetFlipX(true);
        }
        else
        {
            agent->GetGameObject()->GetComponent<SpriteRenderer>()->SetFlipX(false);
        }
        const glm::vec2 vel = dir * mMoveSpeed;
        agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(
            glm::vec2{vel.x, agent->GetGameObject()->GetComponent<RigidBody2D>()->GetVelocity().y});  

        return OnGoing;
    }

    std::unordered_map<EnemyGoal, float> EnemyMoveToPlayerAction::InternalHeuristic(
        const EnemyState& state,
        const EnemyPreference& preference)
    {
        // Heuristic: moving towards the player increases player elimination and decreases self preservation
        return {
            {EnemyGoal::PlayerElimination, 1.0f * preference[EnemyGoal::PlayerElimination]},
            {EnemyGoal::SelfPreservation, -0.5f * preference[EnemyGoal::SelfPreservation]}
        };
    }
    void EnemyMoveToPlayerAction::OnEnd(EnemyAgent& agent, EnemyActionResult /*result*/)
    {
    }
}  // namespace mts