/**
 * @file SentryEnemyAttackAction.cpp
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "SentryEnemyAttackAction.h"

#include "../BulletBehavior.h"
#include "../EnemyParameters/AttackCountdownParam.h"
#include "../EnemyController.h"
#include "Engine/EntitySystem/Component/ActionList.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Rigidbody2D.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/Conductor/ConductorSystem.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
namespace mts
{

    EnemyState SentryEnemyAttackAction::GetExpectedNextState(
        const EnemyState& state,
        const EnemyPreference& preferences) const
    {
        EnemyState newState = state;
        // TODO: update once player gameobject has stats
        newState.SetPlayerHealthLeftRatio(0.3f);
        return newState;
    }

    float SentryEnemyAttackAction::GetEstimatedTimeCost(const EnemyState& state,
                                                        const EnemyPreference& preferences) const
    {
        return 2.f;
    }

    void SentryEnemyAttackAction::FireBullet(EnemyAgent& agent)
    {
        if (!agent.GetGameObject())
        {
            return;
        }

        GameObject* b = GameObjectFactory::CreateFromArchetype("Data/Bullet.prefab");
        if (b)
        {
            float mul =
                agent.GetGameObject()->GetComponent<SpriteRenderer>()->GetFlipX() ? -1.0f : 1.0f;
            b->GetComponent<Transform>()->SetPosition(
                agent.GetGameObject()->GetComponent<Transform>()->GetPosition() +
                glm::vec3{0.3f * mul, 0.3f, 0});

            b->GetComponent<SpriteRenderer>()->SetFlipX(true);
            // Fire upward at 45 degrees so the bullet arcs over toward the player
            b->GetScript<BulletBehavior>()->SetDirection(glm::normalize(glm::vec2(mul, 1.0f)));
            b->GetScript<BulletBehavior>()->SetHoming(true);
        }
        auto* ctrl = agent.GetGameObject()->GetScript<EnemyController>();
        uint32_t subId = ctrl->GetConductorSubId("SentryAttack");
        if (subId != 0)
        {
            ConductorSystem::Get().onBeat.Unsubscribe(subId);
            ctrl->ClearConductorSubId("SentryAttack");
        }
    }

    void SentryEnemyAttackAction::OnBeginning(EnemyAgent& agent)
    {
        agent.GetState().SetInAction(true);

        // Clean up any leftover subscription
        auto* ctrl = agent.GetGameObject()->GetScript<EnemyController>();
        uint32_t oldId = ctrl->GetConductorSubId("SentryAttack");
        if (oldId != 0)
        {
            ConductorSystem::Get().onBeat.Unsubscribe(oldId);
            ctrl->ClearConductorSubId("SentryAttack");
        }

        if (agent.GetGameObject()->GetComponent<AnimationSystem>()->GetCurrentState() ==
            AnimationSystem::States::Attack1)
        {
            agent.GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                AnimationSystem::States::Alerted);
        }
        agent.GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
            AnimationSystem::States::Attack1);

        LOG_INFO("EnemyAttackAction: Attacking player!", );
    }

    bool SentryEnemyAttackAction::IsAvailable(const EnemyState& state,
                                              const EnemyPreference& preferences) const
    {
        float cd = std::get<float>(state.params.at(HashAction("AttackCooldown"))->GetValue());
        if (state.GetInAction() || cd > 0 ||
            state.GetDistanceToPlayer() >
                std::get<float>(state.params.at(HashAction("AggroRange"))->GetValue()))
        {
            return false;
        }
        return true;
    }

    EnemyActionResult SentryEnemyAttackAction::Update(EnemyAgent* agent, float dt, float totalTime)
    {
        if (!agent)
        {
            return Invalid;
        }
        agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));
        const float aggroRange =
            std::get<float>(agent->GetState().params.at(HashAction("AggroRange"))->GetValue());
        const float distToPlayer = agent->GetState().GetDistanceToPlayer();
        if (distToPlayer > aggroRange)
        {
            LOG_INFO("out of range");
            agent->GetState().SetInAction(false);
            return Success;
        }

        auto* ctrl = agent->GetGameObject()->GetScript<EnemyController>();

        // Subscribe once per attack
        if (ctrl->GetConductorSubId("SentryAttack") == 0)
        {
            ConductorSystem& cs = ConductorSystem::Get();
            uint32_t subId = cs.onBeat.Subscribe(
                [this, agent](mts::BeatInfo info)
                {
                    if (info.isHalfBeat)
                        FireBullet(*agent);
                });
            ctrl->SetConductorSubId("SentryAttack", subId);
            return OnGoing;  // wait for beat to fire the bullet
        }

        // If FireBullet already cleared the sub ID, the bullet was fired — finish up
        // cooldown after attack
        agent->GetState()
            .params.at(HashAction("AttackCooldown"))
            ->SetValue(static_cast<AttackCountdownParam*>(
                           agent->GetState().params.at(HashAction("AttackCooldown")))
                           ->GetCooldownTime());
        agent->GetState().SetInAction(false);

        LOG_INFO("Finished Firing");
        return Success;
    }

    void SentryEnemyAttackAction::OnEnd(EnemyAgent& agent, EnemyActionResult result)
    {
        if (result == Failed)
        {
            auto* ctrl = agent.GetGameObject()->GetScript<EnemyController>();
            uint32_t subId = ctrl->GetConductorSubId("SentryAttack");
            if (subId != 0)
            {
                ConductorSystem::Get().onBeat.Unsubscribe(subId);
                ctrl->ClearConductorSubId("SentryAttack");
            }
        }
        agent.GetState().SetInAction(false);
    }

    std::unordered_map<EnemyGoal, float> SentryEnemyAttackAction::InternalHeuristic(
        const EnemyState& state,
        const EnemyPreference& preference)
    {
        return {
            {EnemyGoal::PlayerElimination, 1.0f * preference[EnemyGoal::PlayerElimination]},
            {EnemyGoal::SelfPreservation, 0.5f * preference[EnemyGoal::SelfPreservation]},
        };
    }
}  // namespace mts