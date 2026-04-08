/**
 * @file EnemyAttackAction.cpp
 * @author cyclo
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "BossEnemyAttackAction2.h"

#include <fmod_dsp_effects.h>

#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/Conductor/ConductorSystem.h"
#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/RigidBody2D.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "../../AttackHitboxScript.h"
#include "../EnemyController.h"
#include "../EnemyParameters/AttackCountdownParam.h"

namespace mts
{

    EnemyState BossEnemyAttackAction2::GetExpectedNextState(
        const EnemyState& state,
        const EnemyPreference& preferences) const
    {
        EnemyState newState = state;
        // TODO: update once player gameobject has stats
        newState.SetPlayerHealthLeftRatio(0.4f);
        return newState;
    }

    float BossEnemyAttackAction2::GetEstimatedTimeCost(const EnemyState& state,
                                                       const EnemyPreference& preferences) const
    {
        return 1.72f;
    }

    void BossEnemyAttackAction2::FireBullet(EnemyAgent& agent)
    {
        agent.GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
            AnimationSystem::States::Attack2);
        ConductorSystem::Get().onBeat.Unsubscribe(mConductorSubId);
    }

    void BossEnemyAttackAction2::OnBeginning(EnemyAgent& agent)
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
    }

    bool BossEnemyAttackAction2::IsAvailable(const EnemyState& state,
                                             const EnemyPreference& preferences) const
    {
        if (state.GetInAction() ||
            std::get<float>(state.params.at(HashAction("Attack2Cooldown"))->GetValue()) > 0 ||
            abs(state.GetVectorToPlayer().x) > 7)
        {
            return false;
        }
        return true;
    }

    EnemyActionResult BossEnemyAttackAction2::Update(EnemyAgent* agent, float dt, float totalTime)
    {
        if (!agent)
        {
            return Invalid;
        }

        // Lunge forward and spawn attack hitbox at frame 4
        auto* anim = agent->GetGameObject()->GetComponent<Animation>();
        if (!mHasLunged && totalTime >= anim->GetFrameDuration() * 4.f)
        {
            mHasLunged = true;
            agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(
                glm::vec2(0));
            bool flipX = agent->GetGameObject()->GetComponent<SpriteRenderer>()->GetFlipX();
            float dir = flipX ? -1.0f : 1.0f;
            float lungeDist = 3.0f;
            float frameDur = anim->GetFrameDuration();
            float lungeSpeed = lungeDist / (frameDur * 3.0f);
            agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(
                glm::vec2{dir * lungeSpeed, 0.0f});

            // Spawn attack hitbox in front of the boss
            if (!mHitboxSpawned)
            {
                mHitboxSpawned = true;
                auto* ctrl = agent->GetGameObject()->GetScript<EnemyController>();
                auto* oldHitbox = ctrl->GetAtk2Hitbox();
                if (oldHitbox && !oldHitbox->IsDead())
                    oldHitbox->Destroy();
                auto* mAttackHitbox =
                    GameObjectFactory::CreateFromArchetype("Data/AttackHitbox.prefab");
                ctrl->SetAtk2Hitbox(mAttackHitbox);
                if (mAttackHitbox)
                {
                    auto* bossTr = agent->GetGameObject()->GetComponent<Transform>();
                    glm::vec2 hitboxPos = bossTr->GetPosition();
                    float hitboxOffsetX = 3.0f;
                    hitboxPos.x += dir * hitboxOffsetX;
                    hitboxPos.y += -0.7f;
                    mAttackHitbox->GetComponent<Transform>()->SetPosition(hitboxPos);
                    mAttackHitbox->GetComponent<Transform>()->SetScale(
                        glm::vec3(3.25f, 2.f, 1.0f));

                    mAttackHitbox->SetActive(true);

                    auto* hitboxScript = mAttackHitbox->GetScript<AttackHitboxScript>();
                    if (hitboxScript)
                    {
                        hitboxScript->SetDamage(1.0f);
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
            agent->GetGameObject()->GetScript<EnemyController>()->GetAtk2Hitbox();
        if (mAttackHitbox && !mAttackHitbox->IsDead() && mAttackHitbox->IsActive())
        {
            auto* bossTr = agent->GetGameObject()->GetComponent<Transform>();
            bool flipX = agent->GetGameObject()->GetComponent<SpriteRenderer>()->GetFlipX();
            float dir = flipX ? -1.0f : 1.0f;
            glm::vec2 hitboxPos = bossTr->GetPosition();
            hitboxPos.x += dir * 3.0f;
            hitboxPos.y += -0.7f;
            mAttackHitbox->GetComponent<Transform>()->SetPosition(hitboxPos);
        }

        if (anim->AnimationIsDone())
        {
            agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));
            // cooldown after attack
            agent->GetState()
                .params.at(HashAction("Attack2Cooldown"))
                ->SetValue(dynamic_cast<AttackCountdownParam*>(
                               agent->GetState().params.at(HashAction("Attack2Cooldown")))
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

    void BossEnemyAttackAction2::OnEnd(EnemyAgent& agent, EnemyActionResult result)
    {
        if (result == Failed)
        {
            ConductorSystem::Get().onBeat.Unsubscribe(mConductorSubId);
            // agent.GetState().SetInAction(false);
            agent.GetState().SetInAction(false);
            LOG_INFO("Attack Failed, unsubscribing from beat");
        }
    }

    std::unordered_map<EnemyGoal, float> BossEnemyAttackAction2::InternalHeuristic(
        const EnemyState& state,
        const EnemyPreference& preference)
    {
        return {
            {EnemyGoal::PlayerElimination, 1.0f * preference[EnemyGoal::PlayerElimination]},
            {EnemyGoal::SelfPreservation, 0.5f * preference[EnemyGoal::SelfPreservation]},
        };
    }

}  // namespace mts