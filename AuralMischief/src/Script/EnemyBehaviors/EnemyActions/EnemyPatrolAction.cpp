/**
 * @file EnemyPatrolAction.cpp
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EnemyPatrolAction.h"

#include <imgui_internal.h>

#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include <Engine/EntitySystem/Component/Transform.h>
#include <Engine/EntitySystem/GameObject/GameObject.h>
#include <Engine/EntitySystem/\Component/SpriteRenderer.h>
#include <fmt/base.h>

#include "../EnemyController.h"
#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Rigidbody2D.h"
#include "Engine/Helpers/Random.h"

namespace mts
{
    bool EnemyPatrolAction::IsAvailable(const EnemyState& state,
                                        const EnemyPreference& preferences) const
    {
        // stop patrolling if player in aggro range
        auto it = state.params.find(HashAction("AggroRange"));
        if (it != state.params.end() && it->second != nullptr)
        {
            auto* p = it->second;

            if (state.GetDistanceToPlayer() <= std::get<float>(p->GetValue()))
            {
                return false;
            }
        }
        return true;
    }

    void EnemyPatrolAction::OnBeginning(EnemyAgent& agent)
    {


        LOG_INFO("Patrolling!");

    }

    float EnemyPatrolAction::GetEstimatedTimeCost(const EnemyState& state,
                                                  const EnemyPreference& preferences) const
    {
        return 5.f;
    }

    EnemyState EnemyPatrolAction::GetExpectedNextState(const EnemyState& state,
                                                       const EnemyPreference& preferences) const
    {
        EnemyState ns = state;
        return ns;
    }

    EnemyActionResult EnemyPatrolAction::Update(EnemyAgent* agent, float dt, float totalTime)
    {
        if (!agent)
            return Invalid;
        auto o = agent->GetGameObject();
        if (o->GetComponent<AnimationSystem>()->GetCurrentState() == AnimationSystem::States::Alerted && !o->GetComponent<Animation>()->GetIsDone())
        {
            return OnGoing;  // wait until alerted animation is done before moving
        }
        else
        {
            agent->GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                AnimationSystem::States::Walking);
        }
        auto* go = agent->GetGameObject();
        if (!go)
            return Invalid;

        auto* controller = go->GetScript<EnemyController>();
        auto* transform = go->GetComponent<Transform>();
        auto* sprite = go->GetComponent<SpriteRenderer>();
        auto* rb = go->GetComponent<RigidBody2D>();

        if (!controller || !transform || !sprite || !rb)
            return Invalid;

        EnemyState s = agent->GetState();

        // Stop patrolling once player is close enough
        auto it = s.params.find(HashAction("AggroRange"));
        if (it != s.params.end() && it->second != nullptr)
        {
            const float aggroRange = std::get<float>(it->second->GetValue());
            if (s.GetDistanceToPlayer() <= aggroRange)
            {
                rb->SetVelocity({0.0f, rb->GetVelocity().y});
                return Success;
            }
        }

        const glm::vec3 origin3 = controller->GetOriginPos();
        const glm::vec3 pos3 = transform->GetPosition();

        const float originX = origin3.x;
        const float x = pos3.x;
        const float spd = controller->GetMoveSpeed();
        const float patrolRadius = controller->GetPatrolRadius();

        // true = facing left, false = facing right
        bool flipX = sprite->GetFlipX();
        int facing = flipX ? -1 : 1;

        const float distFromOrigin = x - originX;

        float desiredVelX = 0.0f;

        // If outside patrol range, walk back toward actual origin first
        if (std::abs(distFromOrigin) > patrolRadius)
        {
            if (distFromOrigin > 0.0f)
            {
                // too far right -> go left toward origin
                facing = -1;
                sprite->SetFlipX(true);
            }
            else
            {
                // too far left -> go right toward origin
                facing = 1;
                sprite->SetFlipX(false);
            }

            desiredVelX = static_cast<float>(facing) * spd;

            // stop once close enough to origin to avoid oscillation
            const float stopThreshold = 0.05f;
            if (std::abs(distFromOrigin) <= stopThreshold)
            {
                desiredVelX = 0.0f;
            }
        }
        else
        {
            // Normal patrol inside patrol zone
            desiredVelX = static_cast<float>(facing) * spd;

            // Predict next x using velocity * dt so we can flip before crossing too far
            const float nextX = x + desiredVelX * dt;
            const float nextDist = nextX - originX;

            if (nextDist > patrolRadius)
            {
                facing = -1;
                sprite->SetFlipX(true);
                desiredVelX = -spd;
            }
            else if (nextDist < -patrolRadius)
            {
                facing = 1;
                sprite->SetFlipX(false);
                desiredVelX = spd;
            }
        }

        // Preserve Y velocity if using gravity / vertical motion
        rb->SetVelocity({desiredVelX, rb->GetVelocity().y});

        // For flying enemies, keep them on their patrol Y
        if (controller->GetEnemyType() == EnemyType::FlyEnemy)
        {
            glm::vec3 correctedPos = transform->GetPosition();
            correctedPos.y = origin3.y;
            transform->SetPosition(correctedPos);
        }

        return OnGoing;
    }

    void EnemyPatrolAction::OnEnd(EnemyAgent& agent, EnemyActionResult result)
    {
    }

    std::unordered_map<EnemyGoal, float> EnemyPatrolAction::InternalHeuristic(
        const EnemyState& state,
        const EnemyPreference& preference)
    {
        // Heuristic: unemployed behavior, only patrol when there's nothing to do, should be bottom
        // of food chain
        return {{EnemyGoal::PlayerElimination, 0.1f}, {EnemyGoal::SelfPreservation, 0.1f}};
    }
}  // namespace mts