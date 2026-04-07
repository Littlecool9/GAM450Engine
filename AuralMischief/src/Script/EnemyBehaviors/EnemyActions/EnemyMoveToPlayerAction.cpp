/**
 * @file EnemyMoveToPlayerAction.cpp
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EnemyMoveToPlayerAction.h"

#include "../EnemyController.h"
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
        auto it = state.params.find(HashAction("AttackRange"));
        if (it == state.params.end())
            return false;
        float dis = std::get<float>(it->second->GetValue());

        if (state.GetInAction() || abs(state.GetVectorToPlayer().x) <= dis)
            return false;

        return true;
    }

    float EnemyMoveToPlayerAction::GetEstimatedTimeCost(
        const EnemyState& state,
        const EnemyPreference& /*preferences*/) const
    {
        // Simple heuristic: time = distance / speed, using the distance to player from state
        return state.GetDistanceToPlayer() / mMoveSpeed;
    }

    EnemyState EnemyMoveToPlayerAction::GetExpectedNextState(
        const EnemyState& state,
        const EnemyPreference& /*preferences*/) const
    {
        EnemyState newState = state;
        // We expect to be close to the player after moving towards them, so we can set distance to
        // a low value
        newState.SetDistanceToPlayer(0.5f);  // expected to be within attack range
        return newState;
    }

    void EnemyMoveToPlayerAction::OnBeginning(EnemyAgent& agent)
    {
        LOG_INFO("Chasing!");
        agent.GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
            AnimationSystem::States::Walking);
    }

    EnemyActionResult EnemyMoveToPlayerAction::Update(EnemyAgent* agent, float dt, float totalTime)
    {
        if (!agent)
            return Invalid;

        // Get positions

        const glm::vec2 enemyPos = agent->GetGameObject()->GetComponent<Transform>()->GetPosition();
        const glm::vec2 playerPos =
            agent->GetPlayerReference()->GetComponent<Transform>()->GetPosition();

        const glm::vec2 toPlayer = playerPos - enemyPos;
        const float xDist = std::abs(toPlayer.x);

        const float attackRange =
            agent->GetGameObject()->GetScript<EnemyController>()->GetAttackRange();

        // check if arrived
        if (abs(agent->GetState().GetVectorToPlayer().x) <= attackRange)
        {
            LOG_INFO("arrived, Distance x to player: {}", agent->GetState().GetVectorToPlayer().x);
            // Stop movement when reached
            agent->GetGameObject()->GetScript<EnemyController>()->SetDesiredVelocity(
                glm::vec2(0.0f));
            agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(
                glm::vec2(0));  // TODO: replace

            agent->GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                AnimationSystem::States::Idle);
            return Success;
        }

        // give up if too long
        if (totalTime >= mGiveUpTime)
        {
            // Stop movement on failure too
            agent->GetGameObject()->GetScript<EnemyController>()->SetDesiredVelocity(
                glm::vec2(0.0f));
            agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));

            agent->GetState().SetInAction(false);
            agent->GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                AnimationSystem::States::Idle);
            return Failed;
        }

        // Move toward player (simple steering)
        float fullDist = glm::length(toPlayer);
        glm::vec2 dir = (fullDist > 0.0001f) ? (toPlayer / fullDist) : glm::vec2{0.0f, 0.0f};
        if (dir.x < 0)
        {
            agent->GetGameObject()->GetComponent<SpriteRenderer>()->SetFlipX(false);
        }
        else
        {
            agent->GetGameObject()->GetComponent<SpriteRenderer>()->SetFlipX(true);
        }
        const glm::vec2 vel =
            dir * agent->GetGameObject()->GetScript<EnemyController>()->GetMoveSpeed();
        agent->GetGameObject()->GetScript<EnemyController>()->SetDesiredVelocity(
            glm::vec2{vel.x, 0.0f});
        agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(
            glm::vec2{vel.x, agent->GetGameObject()->GetComponent<RigidBody2D>()->GetVelocity().y});

        return OnGoing;
    }

    std::unordered_map<EnemyGoal, float> EnemyMoveToPlayerAction::InternalHeuristic(
        const EnemyState& state,
        const EnemyPreference& preference)
    {
        // Heuristic: moving towards the player increases player elimination and decreases self
        // preservation
        return {{EnemyGoal::PlayerElimination, 0.3f * preference[EnemyGoal::PlayerElimination]},
                {EnemyGoal::SelfPreservation, -0.5f * preference[EnemyGoal::SelfPreservation]}};
    }
    void EnemyMoveToPlayerAction::OnEnd(EnemyAgent& agent, EnemyActionResult /*result*/)
    {
    }
}  // namespace mts