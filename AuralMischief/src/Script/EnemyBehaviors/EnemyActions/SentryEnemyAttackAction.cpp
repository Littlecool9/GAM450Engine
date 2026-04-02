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
#include "Engine/EntitySystem/Component/ActionList.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "../EnemyParameters/AttackCountdownParam.h"
#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Rigidbody2D.h"

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
                agent.GetGameObject()->GetComponent<SpriteRenderer>()->GetFlipX() ? -1 : 1;
            b->GetComponent<Transform>()->SetPosition(
                agent.GetGameObject()->GetComponent<Transform>()->GetPosition() + glm::vec3{0.3f * mul, -1.f,0});
            b->GetComponent<Transform>()->SetRotation(b->GetComponent<Transform>()->GetRotation() +
                                                      0.785398 * mul);

            b->GetComponent<SpriteRenderer>()->SetFlipX(
                agent.GetGameObject()->GetComponent<SpriteRenderer>()->GetFlipX());
            b->GetScript<BulletBehavior>()->SetDirection(glm::vec2(1.0f, 0.0f) * mul);
        }
        ConductorSystem::Get().onBeat.Unsubscribe(mConductorSubId);
    }

    void SentryEnemyAttackAction::OnBeginning(EnemyAgent& agent)
    {
        agent.GetState().SetInAction(true);
        glm::vec3 dir = agent.GetPlayerReference()->GetComponent<mts::Transform>()->GetPosition() -
                        agent.GetGameObject()->GetComponent<Transform>()->GetPosition();


        if (agent.GetGameObject()->GetComponent<AnimationSystem>()->GetCurrentState() == AnimationSystem::States::Attack1)
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
        if (state.GetInAction() || cd > 0 || state.GetDistanceToPlayer() > std::get<float>(state.params.at(HashAction("AggroRange"))->GetValue()))
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
        //auto a = agent->GetGameObject()->GetComponent<Animation>();
        //a->AnimationPlay(a->GetStartFrame(), 0, 0, false);
        //agent->GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
        //    AnimationSystem::States::Alerted);
        LOG_INFO("Finished Firing");
        return Success;


    }

    void SentryEnemyAttackAction::OnEnd(EnemyAgent& agent, EnemyActionResult result)
    {
        if (result == Failed)
        {
            //ConductorSystem::Get().onBeat.Unsubscribe(mConductorSubId);
            agent.GetState().SetInAction(false);
            LOG_INFO("Attack Failed, unsubscribing from beat");
        }
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