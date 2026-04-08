/**
 * @file EnemyAttackAction.cpp
 * @author cyclo
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "BossEnemyAttackAction.h"

#include <fmod_dsp_effects.h>

#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/Conductor/ConductorSystem.h"
#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/RigidBody2D.h"
#include "../EnemyParameters//AttackCountdownParam.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "../../AttackHitboxScript.h"
#include "../EnemyController.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"

namespace mts
{
    EnemyState BossEnemyAttackAction::GetExpectedNextState(const EnemyState& state,
                                                           const EnemyPreference& preferences) const
    {
        EnemyState newState = state;
        // TODO: update once player gameobject has stats
        newState.SetPlayerHealthLeftRatio(0.6f);
        return newState;
    }

    float BossEnemyAttackAction::GetEstimatedTimeCost(const EnemyState& state,
                                                      const EnemyPreference& preferences) const
    {
        return 1.28f;
    }

    void BossEnemyAttackAction::FireBullet(EnemyAgent& agent)
    {
        if (!agent.GetGameObject())
        {
            return;
        }
        agent.GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
            AnimationSystem::States::Attack1);
        ConductorSystem::Get().onBeat.Unsubscribe(mConductorSubId);
    }

    void BossEnemyAttackAction::OnBeginning(EnemyAgent& agent)
    {
        agent.GetState().SetInAction(true);
        mHasLunged = false;
        mHitboxSpawned = false;
        ConductorSystem& cs = ConductorSystem::Get();
        mConductorSubId = cs.onBeat.Subscribe(
            [this, &agent](mts::BeatInfo info)
            {
                if (info.isHalfBeat)
                    FireBullet(agent);
            });

        // debug
        //LOG_INFO("EnemyAttackAction: Attacking player!", );
    }

    bool BossEnemyAttackAction::IsAvailable(const EnemyState& state,
                                            const EnemyPreference& preferences) const
    {
        if (state.GetInAction() ||
            std::get<float>(state.params.at(HashAction("AttackCooldown"))->GetValue()) > 0 ||
            (state.GetVectorToPlayer().x) > 5.2f)
        {
            return false;
        }
        return true;
    }

    EnemyActionResult BossEnemyAttackAction::Update(EnemyAgent* agent, float dt, float totalTime)
    {
        if (!agent)
        {
            return Invalid;
        }

        // Lunge forward and spawn attack hitbox at frame 6
        auto* anim = agent->GetGameObject()->GetComponent<Animation>();
        if (!mHasLunged && totalTime >= anim->GetFrameDuration() * 6.f)
        {
            mHasLunged = true;
            bool flipX = agent->GetGameObject()->GetComponent<SpriteRenderer>()->GetFlipX();
            float dir = flipX ? -1.0f : 1.0f;
            float lungeDist = 1.5f;
            float frameDur = anim->GetFrameDuration();
            float lungeSpeed = lungeDist / (frameDur * 3.0f);
            agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(
                glm::vec2{dir * lungeSpeed, 0.0f});

            // Spawn attack hitbox in front of the boss
            if (!mHitboxSpawned)
            {
                mHitboxSpawned = true;
                auto* ctrl = agent->GetGameObject()->GetScript<EnemyController>();
                auto* oldHitbox = ctrl->GetAtk1Hitbox();
                if (oldHitbox && !oldHitbox->IsDead())
                    oldHitbox->Destroy();
                auto* mAttackHitbox =
                    GameObjectFactory::CreateFromArchetype("Data/AttackHitbox.prefab");
                ctrl->SetAtk1Hitbox(mAttackHitbox);
                if (mAttackHitbox)
                {
                    auto* bossTr = agent->GetGameObject()->GetComponent<Transform>();
                    glm::vec2 hitboxPos = bossTr->GetPosition();
                    float hitboxOffsetX = 3.0f;
                    hitboxPos.x += dir * hitboxOffsetX;
                    hitboxPos.y += -0.2f;
                    mAttackHitbox->GetComponent<Transform>()->SetPosition(hitboxPos);
                    mAttackHitbox->GetComponent<Transform>()->SetScale(
                        glm::vec3(3.25f, 2.5f, 1.0f));
                    mAttackHitbox->SetActive(true);

                    auto* hitboxScript = mAttackHitbox->GetScript<AttackHitboxScript>();
                    if (hitboxScript)
                    {
                        hitboxScript->SetDamage(2.0f);
                        hitboxScript->SetTeam(AttackHitboxScript::Team::Enemy);
                        glm::vec2 knockbackDir = glm::normalize(
                            glm::vec2(agent->GetPlayerReference()
                                          ->GetComponent<Transform>()
                                          ->GetPosition()) -
                            glm::vec2(
                                agent->GetGameObject()->GetComponent<Transform>()->GetPosition()));
                        hitboxScript->SetLifetime(0.7f);
                        hitboxScript->SetDirection(knockbackDir);
                    }
                }
            }
        }

        // Keep the hitbox following the boss during lunge
        auto* mAttackHitbox =
            agent->GetGameObject()->GetScript<EnemyController>()->GetAtk1Hitbox();
        if (mAttackHitbox && !mAttackHitbox->IsDead() && mAttackHitbox->IsActive())
        {
            auto* bossTr = agent->GetGameObject()->GetComponent<Transform>();
            bool flipX = agent->GetGameObject()->GetComponent<SpriteRenderer>()->GetFlipX();
            float dir = flipX ? -1.0f : 1.0f;
            glm::vec2 hitboxPos = bossTr->GetPosition();
            hitboxPos.x += dir * 3.0f;
            mAttackHitbox->GetComponent<Transform>()->SetPosition(hitboxPos);
        }

        if (anim->AnimationIsDone())
        {
            agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));
            // cooldown after attack
            agent->GetState()
                .params.at(HashAction("AttackCooldown"))
                ->SetValue(dynamic_cast<AttackCountdownParam*>(
                               agent->GetState().params.at(HashAction("AttackCooldown")))
                               ->GetCooldownTime());
            agent->GetState().SetInAction(false);
            agent->GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                AnimationSystem::States::Walking);
            LOG_INFO("Finished Firing");
            return Success;
        }
        else
        {
            return OnGoing;
        }
    }

    void BossEnemyAttackAction::OnEnd(EnemyAgent& agent, EnemyActionResult result)
    {
        if (result == Failed)
        {
            agent.GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));
            ConductorSystem::Get().onBeat.Unsubscribe(mConductorSubId);
            agent.GetState().SetInAction(false);
            LOG_INFO("Attack Failed, unsubscribing from beat");
        }
    }

    std::unordered_map<EnemyGoal, float> BossEnemyAttackAction::InternalHeuristic(
        const EnemyState& state,
        const EnemyPreference& preference)
    {
        return {
            {EnemyGoal::PlayerElimination, 1.0f * preference[EnemyGoal::PlayerElimination]},
            {EnemyGoal::SelfPreservation, 0.5f * preference[EnemyGoal::SelfPreservation]},
        };
    }

}  // namespace mts