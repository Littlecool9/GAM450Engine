/**
 * @file EnemyAttackAction.cpp
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "FlyingEnemyAttackAction.h"
#include <Engine/EntitySystem/Component/Actions/TranslateAction.h>
#include <Engine/EntitySystem/Component/Actions/RotateAction.h>
#include <fmod_dsp_effects.h>

#include "../BulletBehavior.h"
#include "../EnemyParameters/AttackCountdownParam.h"

#include "../EnemyController.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/Conductor/ConductorSystem.h"

#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Rigidbody2D.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"

namespace mts
{

    EnemyState FlyingEnemyAttackAction::GetExpectedNextState(
        const EnemyState& state,
        const EnemyPreference& preferences) const
    {
        EnemyState newState = state;
        // TODO: update once player gameobject has stats
        newState.SetPlayerHealthLeftRatio(0.3f);
        return newState;
    }

    float FlyingEnemyAttackAction::GetEstimatedTimeCost(const EnemyState& state,
                                                        const EnemyPreference& preferences) const
    {
        return 2.f;
    }

    void FlyingEnemyAttackAction::FireBullet(EnemyAgent& agent)
    {
        if (!agent.GetGameObject())
        {
            return;
        }

        GameObject* b = GameObjectFactory::CreateFromArchetype("Data/Bullet.prefab");

        if (b)
        {
            b->GetComponent<Transform>()->SetPosition(
                agent.GetGameObject()->GetComponent<Transform>()->GetPosition());
            // Point downward
            b->GetComponent<SpriteRenderer>()->SetFlipX(true);
            b->GetComponent<Transform>()->SetRotation(glm::half_pi<float>());
            b->GetScript<BulletBehavior>()->SetDirection(glm::vec2(0.0f, -1.0f));
            b->GetScript<BulletBehavior>()->SetHoming(false);
        }
        ConductorSystem::Get().onBeat.Unsubscribe(mConductorSubId);
    }

    void FlyingEnemyAttackAction::OnBeginning(EnemyAgent& agent)
    {
        agent.GetState().SetInAction(true);

        LOG_INFO("EnemyAttackAction: Attacking player!", );
    }

    bool FlyingEnemyAttackAction::IsAvailable(const EnemyState& state,
                                              const EnemyPreference& preferences) const
    {
        auto rangeIt = state.params.find(HashAction("AttackRange"));
        auto cdIt = state.params.find(HashAction("AttackCooldown"));
        if (rangeIt == state.params.end() || cdIt == state.params.end())
            return false;
        float dis = std::get<float>(rangeIt->second->GetValue());
        float cd = std::get<float>(cdIt->second->GetValue());
        if (state.GetInAction() || cd > 0 || abs(state.GetVectorToPlayer().x) > dis)
        {
            return false;
        }
        return true;
    }

    EnemyActionResult FlyingEnemyAttackAction::Update(EnemyAgent* agent, float dt, float totalTime)
    {
        if (!agent)
        {
            return Invalid;
        }
        agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));

        auto* playerGO = agent->GetPlayerReference();
        if (playerGO)
        {

            const float attackRange =
                agent->GetGameObject()->GetScript<EnemyController>()->GetAttackRange();
            if (abs(agent->GetState().GetVectorToPlayer().x) > attackRange)
            {
                LOG_INFO("out of x range");
                agent->GetState().SetInAction(false);
                return Success;
            }
        }

        ConductorSystem& cs = ConductorSystem::Get();
        mConductorSubId = cs.onBeat.Subscribe(
            [this, agent](mts::BeatInfo info)
            {
                if (info.isHalfBeat)
                    FireBullet(*agent);
            });
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

    void FlyingEnemyAttackAction::OnEnd(EnemyAgent& agent, EnemyActionResult result)
    {
        if (result == Failed)
        {
            ConductorSystem::Get().onBeat.Unsubscribe(mConductorSubId);
            agent.GetState().SetInAction(false);
            LOG_INFO("Attack Failed, unsubscribing from beat");
        }
    }

    std::unordered_map<EnemyGoal, float> FlyingEnemyAttackAction::InternalHeuristic(
        const EnemyState& state,
        const EnemyPreference& preference)
    {
        return {
            {EnemyGoal::PlayerElimination, 1.0f * preference[EnemyGoal::PlayerElimination]},
            {EnemyGoal::SelfPreservation, 0.5f * preference[EnemyGoal::SelfPreservation]},
        };
    }

}  // namespace mts