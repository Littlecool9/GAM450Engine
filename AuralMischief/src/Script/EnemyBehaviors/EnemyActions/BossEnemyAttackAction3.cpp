/**
 * @file EnemyAttackAction.cpp
 * @author cyclo
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "BossEnemyAttackAction3.h"

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

    EnemyState BossEnemyAttackAction3::GetExpectedNextState(
        const EnemyState& state,
        const EnemyPreference& preferences) const
    {
        EnemyState newState = state;
        // TODO: update once player gameobject has stats
        newState.SetPlayerHealthLeftRatio(0.4f);
        return newState;
    }

    float BossEnemyAttackAction3::GetEstimatedTimeCost(const EnemyState& state,
                                                       const EnemyPreference& preferences) const
    {
        return 1.72f;
    }

    void BossEnemyAttackAction3::FireBullet(EnemyAgent& agent)
    {
        agent.GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
            AnimationSystem::States::Attack3);
        auto* ctrl = agent.GetGameObject()->GetScript<EnemyController>();
        uint32_t subId = ctrl->GetConductorSubId("BossAttack3");
        if (subId != 0)
        {
            ConductorSystem::Get().onBeat.Unsubscribe(subId);
            ctrl->ClearConductorSubId("BossAttack3");
        }
    }

    void BossEnemyAttackAction3::OnBeginning(EnemyAgent& agent)
    {
        agent.GetState().SetInAction(true);
        mHasLunged = false;
        mHitboxSpawned = false;
        mHasPlunged = false;
        mHitbox2Spawned = false;

        auto* ctrl = agent.GetGameObject()->GetScript<EnemyController>();
        uint32_t oldId = ctrl->GetConductorSubId("BossAttack3");
        if (oldId != 0)
        {
            ConductorSystem::Get().onBeat.Unsubscribe(oldId);
            ctrl->ClearConductorSubId("BossAttack3");
        }

        ConductorSystem& cs = ConductorSystem::Get();
        uint32_t subId = cs.onBeat.Subscribe(
            [this, &agent](mts::BeatInfo info)
            {
                if (info.isHalfBeat)
                    FireBullet(agent);
            });
        ctrl->SetConductorSubId("BossAttack3", subId);
    }

    bool BossEnemyAttackAction3::IsAvailable(const EnemyState& state,
                                             const EnemyPreference& preferences) const
    {
        if (state.GetInAction() ||
            std::get<float>(state.params.at(HashAction("Attack3Cooldown"))->GetValue()) > 0 ||
            abs(state.GetVectorToPlayer().x) > 4 ||
            abs(state.GetVectorToPlayer().y) > 1.5f)
        {
            return false;
        }
        return true;
    }

    EnemyActionResult BossEnemyAttackAction3::Update(EnemyAgent* agent, float dt, float totalTime)
    {
        if (!agent)
        {
            return Invalid;
        }

        // Lunge forward and spawn attack hitbox at frame 4
        auto* anim = agent->GetGameObject()->GetComponent<Animation>();
        if (!mHasLunged && totalTime >= anim->GetFrameDuration() * 4.f)
        {
            agent->GetGameObject()->GetScript<EnemyController>()->PlayBossMoanSFX();
            agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));
            mHasLunged = true;
            bool flipX = agent->GetGameObject()->GetComponent<SpriteRenderer>()->GetFlipX();
            float dir = flipX ? -1.0f : 1.0f;
            float lungeDist = 2.0f;
            float frameDur = anim->GetFrameDuration();
            float lungeSpeed = lungeDist / (frameDur * 3.0f);
            agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(
                glm::vec2{dir * lungeSpeed, 0.0f});

            // Spawn attack hitbox in front of the boss
            if (!mHitboxSpawned)
            {
                mHitboxSpawned = true;
                auto* ctrl = agent->GetGameObject()->GetScript<EnemyController>();
                auto* oldHitbox = ctrl->GetAtk3Hitbox();
                if (oldHitbox && !oldHitbox->IsDead())
                    oldHitbox->Destroy();
                auto* mAttackHitbox =
                    GameObjectFactory::CreateFromArchetype("Data/AttackHitbox.prefab");
                ctrl->SetAtk3Hitbox(mAttackHitbox);
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
            agent->GetGameObject()->GetScript<EnemyController>()->GetAtk3Hitbox();
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
        // Spawn front and behind hitboxes at frame 10
        if (!mHasPlunged && totalTime >= anim->GetFrameDuration() * 12.f)
        {
            mHasPlunged = true;
            agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(
                glm::vec2(0));
            bool flipX = agent->GetGameObject()->GetComponent<SpriteRenderer>()->GetFlipX();
            float dir = flipX ? -1.0f : 1.0f;
            auto* bossTr = agent->GetGameObject()->GetComponent<Transform>();
            glm::vec2 bossPos = bossTr->GetPosition();
            float hitboxOffsetX = 3.0f;

            auto* ctrl2 = agent->GetGameObject()->GetScript<EnemyController>();
            // Front hitbox
            auto* oldFront = ctrl2->GetAtk3HitboxFront2();
            if (oldFront && !oldFront->IsDead())
                oldFront->Destroy();
            auto* mAttackHitboxFront2 =
                GameObjectFactory::CreateFromArchetype("Data/AttackHitbox.prefab");
            ctrl2->SetAtk3HitboxFront2(mAttackHitboxFront2);
            if (mAttackHitboxFront2)
            {
                glm::vec2 frontPos = bossPos;
                frontPos.x += dir * hitboxOffsetX;
                frontPos.y += -1.f;
                mAttackHitboxFront2->GetComponent<Transform>()->SetPosition(frontPos);
                mAttackHitboxFront2->GetComponent<Transform>()->SetScale(
                    glm::vec3(3.25f, 1.2f, 1.0f));
                mAttackHitboxFront2->SetActive(true);

                auto* script = mAttackHitboxFront2->GetScript<AttackHitboxScript>();
                if (script)
                {
                    script->SetDamage(1.0f);
                    script->SetTeam(AttackHitboxScript::Team::Enemy);
                    glm::vec2 knockbackDir = glm::normalize(
                        glm::vec2(
                            agent->GetPlayerReference()->GetComponent<Transform>()->GetPosition()) -
                        glm::vec2(bossTr->GetPosition()));
                    script->SetLifetime(0.7f);
                    script->SetDirection(knockbackDir);
                }
            }

            // Behind hitbox
            auto* oldBehind = ctrl2->GetAtk3HitboxBehind();
            if (oldBehind && !oldBehind->IsDead())
                oldBehind->Destroy();
            auto* mAttackHitboxBehind =
                GameObjectFactory::CreateFromArchetype("Data/AttackHitbox.prefab");
            ctrl2->SetAtk3HitboxBehind(mAttackHitboxBehind);
            if (mAttackHitboxBehind)
            {
                glm::vec2 behindPos = bossPos;
                behindPos.x -= dir * hitboxOffsetX;
                behindPos.y += -1.f;
                mAttackHitboxBehind->GetComponent<Transform>()->SetPosition(behindPos);
                mAttackHitboxBehind->GetComponent<Transform>()->SetScale(
                    glm::vec3(3.25f, 1.2f, 1.0f));
                mAttackHitboxBehind->SetActive(true);

                auto* script = mAttackHitboxBehind->GetScript<AttackHitboxScript>();
                if (script)
                {
                    script->SetDamage(1.0f);
                    script->SetTeam(AttackHitboxScript::Team::Enemy);
                    glm::vec2 knockbackDir = glm::normalize(
                        glm::vec2(
                            agent->GetPlayerReference()->GetComponent<Transform>()->GetPosition()) -
                        glm::vec2(bossTr->GetPosition()));
                    script->SetLifetime(0.7f);
                    script->SetDirection(knockbackDir);
                }
            }
        }
        // Keep frame 10 hitboxes following the boss
        if (mHasPlunged)
        {
            auto* ctrlFollow = agent->GetGameObject()->GetScript<EnemyController>();
            auto* bossTr = agent->GetGameObject()->GetComponent<Transform>();
            bool flipX = agent->GetGameObject()->GetComponent<SpriteRenderer>()->GetFlipX();
            float dir = flipX ? -1.0f : 1.0f;
            glm::vec2 bossPos = bossTr->GetPosition();

            auto* mAttackHitboxFront2 = ctrlFollow->GetAtk3HitboxFront2();
            if (mAttackHitboxFront2 && !mAttackHitboxFront2->IsDead() &&
                mAttackHitboxFront2->IsActive())
            {
                glm::vec2 frontPos = bossPos;
                frontPos.x += dir * 3.0f;
                frontPos.y += -1.f;
                mAttackHitboxFront2->GetComponent<Transform>()->SetPosition(frontPos);
            }
            auto* mAttackHitboxBehind = ctrlFollow->GetAtk3HitboxBehind();
            if (mAttackHitboxBehind && !mAttackHitboxBehind->IsDead() &&
                mAttackHitboxBehind->IsActive())
            {
                glm::vec2 behindPos = bossPos;
                behindPos.x -= dir * 3.0f;
                behindPos.y += -1.f;
                mAttackHitboxBehind->GetComponent<Transform>()->SetPosition(behindPos);
            }
        }
        if (anim->AnimationIsDone())
        {
            agent->GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));
            // cooldown after attack
            agent->GetState()
                .params.at(HashAction("Attack3Cooldown"))
                ->SetValue(dynamic_cast<AttackCountdownParam*>(
                               agent->GetState().params.at(HashAction("Attack3Cooldown")))
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

    void BossEnemyAttackAction3::OnEnd(EnemyAgent& agent, EnemyActionResult result)
    {
        auto* ctrl = agent.GetGameObject()->GetScript<EnemyController>();
        uint32_t subId = ctrl->GetConductorSubId("BossAttack3");
        if (subId != 0)
        {
            ConductorSystem::Get().onBeat.Unsubscribe(subId);
            ctrl->ClearConductorSubId("BossAttack3");
        }
        if (result == Failed)
        {
            agent.GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));
            agent.GetState().SetInAction(false);
            LOG_INFO("Attack Failed, unsubscribing from beat");
        }
    }

    std::unordered_map<EnemyGoal, float> BossEnemyAttackAction3::InternalHeuristic(
        const EnemyState& state,
        const EnemyPreference& preference)
    {
        return {
            {EnemyGoal::PlayerElimination, 1.0f * preference[EnemyGoal::PlayerElimination]},
            {EnemyGoal::SelfPreservation, 0.5f * preference[EnemyGoal::SelfPreservation]},
        };
    }

}  // namespace mts