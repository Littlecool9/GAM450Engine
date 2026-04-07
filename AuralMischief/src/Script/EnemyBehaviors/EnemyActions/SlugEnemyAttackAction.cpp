/**
 * @file SlugEnemyAttackAction.cpp
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "SlugEnemyAttackAction.h"

#include "../BulletBehavior.h"
#include "../EnemyController.h"
#include "Engine/EntitySystem/Component/ActionList.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "../EnemyParameters/AttackCountdownParam.h"

#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Rigidbody2D.h"

#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
namespace mts
{

    EnemyState SlugEnemyAttackAction::GetExpectedNextState(const EnemyState& state,
                                                           const EnemyPreference& preferences) const
    {
        EnemyState newState = state;

        newState.SetPlayerHealthLeftRatio(0.3f);
        return newState;
    }

    float SlugEnemyAttackAction::GetEstimatedTimeCost(const EnemyState& state,
                                                      const EnemyPreference& preferences) const
    {
        return 2.f;
    }

    void SlugEnemyAttackAction::OnBeginning(EnemyAgent& agent)
    {
        agent.GetState().SetInAction(true);

        LOG_INFO("EnemyAttackAction: slug Attacking player!", );
    }

    bool SlugEnemyAttackAction::IsAvailable(const EnemyState& state,
                                            const EnemyPreference& preferences) const
    {
        float dis = std::get<float>(state.params.at(HashAction("AggroRange"))->GetValue());
        float toPlayer = state.GetDistanceToPlayer();

        if (state.GetInAction() || state.GetDistanceToPlayer() >= dis)
        {
            LOG_INFO("not available currently in action: {}; distance to player: {}",
                     state.GetInAction(),
                     state.GetDistanceToPlayer());
            return false;
        }

        LOG_INFO("Attack available");
        return true;
    }

    EnemyActionResult SlugEnemyAttackAction::Update(EnemyAgent* agent, float dt, float totalTime)
    {
        // LOG_INFO("Updating SlugEnemyAttackAction, time in action: {}", totalTime);
        if (!agent)
        {
            return Invalid;
        }

        auto o = agent->GetGameObject();
        if (!o)
        {
            return Invalid;
        }

        auto anim = o->GetComponent<AnimationSystem>();
        if (anim && anim->GetCurrentState() == AnimationSystem::States::Alerted && !anim->IsDone())
        {
            LOG_INFO("Alerting");
            return OnGoing;  // wait until alerted animation is done before moving
        }

        agent->GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
            AnimationSystem::States::Attack1);

        auto sprite = o->GetComponent<SpriteRenderer>();
        auto rb = o->GetComponent<RigidBody2D>();
        if (!sprite || !rb)
        {
            LOG_INFO("Missing components");
            agent->GetState().SetInAction(false);
            return Invalid;
        }

        const bool flipX = sprite->GetFlipX();  // true = left, false = right
        const float moveDir = flipX ? -1.0f : 1.0f;
        const float spd = o->GetScript<EnemyController>()->GetMoveSpeed() * 1.5f;
        // constantly move in facing direction
        rb->SetVelocity({moveDir * spd, rb->GetVelocity().y});

        const float distToPlayer = agent->GetState().GetDistanceToPlayer();
        const float aggroRange =
            std::get<float>(agent->GetState().params.at(HashAction("AggroRange"))->GetValue());
        if (distToPlayer <= 1.5f)
        {
            o->GetScript<EnemyController>()->Explode();
            return Success;
        }
        const glm::vec2 myPos = o->GetComponent<Transform>()->GetPosition();
        const glm::vec2 playerPos =
            agent->GetPlayerReference()->GetComponent<Transform>()->GetPosition();

        const float toPlayerX = playerPos.x - myPos.x;

        const bool facingAwayFromPlayer =
            (flipX && toPlayerX > 0.0f) || (!flipX && toPlayerX < 0.0f);

        if (distToPlayer > aggroRange && facingAwayFromPlayer)
        {
            LOG_INFO("slug Done");
            agent->GetState().SetInAction(false);
            return Success;
        }

        return OnGoing;
    }

    void SlugEnemyAttackAction::OnEnd(EnemyAgent& agent, EnemyActionResult result)
    {
        if (result == Failed)
        {
            // ConductorSystem::Get().onBeat.Unsubscribe(mConductorSubId);
            agent.GetState().SetInAction(false);
            LOG_INFO("Attack Failed, unsubscribing from beat");
        }
    }

    std::unordered_map<EnemyGoal, float> SlugEnemyAttackAction::InternalHeuristic(
        const EnemyState& state,
        const EnemyPreference& preference)
    {
        return {
            {EnemyGoal::PlayerElimination, 1.0f * preference[EnemyGoal::PlayerElimination]},
            {EnemyGoal::SelfPreservation, 0.5f * preference[EnemyGoal::SelfPreservation]},
        };
    }
}  // namespace mts