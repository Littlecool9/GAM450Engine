/**
 * @file EnemyController.cpp
 * @author sumin.park
 * @brief  Enemy behavior script
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EnemyController.h"

// #include "BarScaler.h"
#include "../AttackHitboxScript.h"
#include "../Player/PlayerController.h"
#include "EnemyAudioController.h"

#include "EnemyActions/EnemyMoveToPlayerAction.h"
#include "EnemyActions/EnemyPatrolAction.h"
#include "EnemyActions/FlyingEnemyAttackAction.h"
#include "EnemyActions/SentryEnemyAttackAction.h"
#include "EnemyActions/SlugEnemyAttackAction.h"
#include "EnemyParameters/AggroRangeParam.h"
#include "EnemyParameters/AggroYRangeParam.h"
#include "EnemyParameters/AttackCountdownParam.h"
#include "EnemyParameters/PlayerHealthRatioParam.h"
#include "Engine/EntitySystem/Component/ActionList.h"
#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyActionList.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/RigidBody2D.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "../../Actions/ColliderScaleAction.h"
#include "EnemyActions/BossEnemyAttackAction.h"
#include "EnemyActions/BossEnemyAttackAction2.h"
#include "EnemyActions/BossEnemyAttackAction3.h"
#include "EnemyActions/EnemyDodgeAction.h"
#include "EnemyParameters/AttackRangeParam.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/Helpers/Random.h"
#include "Engine/Physics2D/Collision2D.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/SceneSystem/Scene.h"

REGISTER_ENEMY_ACTION("SlugEnemy", mts::EnemyPatrolAction);
REGISTER_ENEMY_ACTION("SlugEnemy", mts::SlugEnemyAttackAction);

REGISTER_ENEMY_ACTION("SentryEnemy", mts::EnemyPatrolAction);
REGISTER_ENEMY_ACTION("SentryEnemy", mts::SentryEnemyAttackAction);

REGISTER_ENEMY_ACTION("FlyEnemy", mts::EnemyPatrolAction);
REGISTER_ENEMY_ACTION("FlyEnemy", mts::EnemyMoveToPlayerAction);
REGISTER_ENEMY_ACTION("FlyEnemy", mts::FlyingEnemyAttackAction);

REGISTER_ENEMY_ACTION("BossEnemy", mts::BossEnemyAttackAction);
REGISTER_ENEMY_ACTION("BossEnemy", mts::BossEnemyAttackAction2);
REGISTER_ENEMY_ACTION("BossEnemy", mts::EnemyMoveToPlayerAction);
REGISTER_ENEMY_ACTION("BossEnemy", mts::BossEnemyAttackAction3)
REGISTER_ENEMY_ACTION("BossEnemy", mts::EnemyDodgeAction)


namespace mts
{
    void EnemyController::Init()
    {
        curhp = maxhp;
        SetEnemyPlans(type);
        origin = GetGameObject()->GetComponent<Transform>()->GetPosition();
    }

    void EnemyController::Start()
    {
        auto* playerGO = SceneSystem::GetCurrentScene()->FindGameObjectByName("Player");
        GetGameObject()->GetComponent<mts::EnemyAgent>()->SetPlayerReference(playerGO);
        MTS_ASSERT(playerGO && "EnemyController could not find Player GameObject in scene");
        if (playerGO)
            mPlayerController = playerGO->GetScript<PlayerController>();

        auto* audioGO =
            SceneSystem::GetCurrentScene()->FindGameObjectByName("EnemyAudioController");
        if (audioGO)
            mAudioController = audioGO->GetScript<EnemyAudioController>();
        else
        {
            auto o = GameObjectFactory::CreateFromArchetype("Data/EnemyAudioController.prefab");
            if (o)
            {
                mAudioController = o->GetScript<EnemyAudioController>();
            }
        }

        // Pre-warm alerted=true so frame-1 OnUpdate doesn't fire alert sounds while
        // EnemyAgent distance is still at its default of 0. Enemies outside real aggro
        // range self-correct to alerted=false once the agent computes the real distance
        // (the false→true transition is what triggers the sound, not true→false).
        alerted = true;
    }

    void EnemyController::TakeDamage(int dmg)
    {
        if (mIsInvincible)
            return;
        if (mDying)
            return;

        curhp -= dmg;
        LOG_INFO("Enemy took damage, current hp: {}", curhp);
        if (curhp <= 0)
        {
            curhp = 0;
            LOG_INFO("Enemy died");
        }
    }

    void EnemyController::Explode()
    {
        if (mDying)
            return;  // already dying from other cause, don't explode

        mShouldExplode = true;
        // curhp = 0;
        LOG_INFO("Enemy exploding");
    }

    void EnemyController::OnUpdate(float dt)
    {

        if (enemyType == EnemyType::CrawlEnemy && mShouldExplode)
        {
            auto* animSys = GetGameObject()->GetComponent<AnimationSystem>();
            auto* anim = GetGameObject()->GetComponent<Animation>();
            auto* sprite = GetGameObject()->GetComponent<SpriteRenderer>();

            // If fading out (killed during Attack2 wind-up), just fade and destroy
            if (mFadingOut)
            {
                GetGameObject()->GetComponent<EnemyAgent>()->SetEnabled(false);
                GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));
                float alpha = sprite->GetAlpha();
                alpha -= dt * 2.0f;
                if (alpha <= 0.0f)
                {
                    GetGameObject()->Destroy();
                }
                else
                {
                    sprite->SetAlpha(alpha);
                }
                return;
            }

            auto currentState = animSys->GetCurrentState();

            if (currentState != AnimationSystem::States::Attack2 &&
                currentState != AnimationSystem::States::Death)
            {
                animSys->ChangeState(AnimationSystem::States::Attack2);
                GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));
                if (!mDeathBeepPlayed)
                {
                    if (mAudioController)
                    {
                        mFuseHandle = mAudioController->PlaySlugBeep();
                        LOG_INFO("Playing slug beep with handle {}", mFuseHandle.value);
                    }
                    mDeathBeepPlayed = true;
                }
                return;
            }

            if (currentState == AnimationSystem::States::Attack2)
            {
                if (anim->GetIsDone())
                    mAttack2Done = true;

                // Check if killed during Attack2 wind-up explosion, fade out
                if (!mIsInvincible && curhp <= 0)
                {
                    mShouldExplode = false;
                    mFadingOut = true;
                    mDying = true;

                    return;
                }

                if (!mAttack2Done)
                    return;

                // Attack2 done, to fuse/explode
            }

            //Fuse timer
            if (currentState != AnimationSystem::States::Death)
            {
                if (mTimeTillExplode > 0 && deathAnimFirstTime)
                {
                    mTimeTillExplode -= dt;
                    return;
                }

                //switch to Death animation and scale up collider
                mDying = true;
                curhp = 0;
                GetGameObject()->GetComponent<EnemyAgent>()->SetEnabled(false);
                animSys->ChangeState(AnimationSystem::States::Death);
                if (deathAnimFirstTime)
                {
                    if (mAudioController)
                        mAudioController->PlaySlugExplode(mFuseHandle);
                    // GetGameObject()->GetComponent<BoxCollider2D>()->SetIsTrigger(true);
                    // GetGameObject()->GetComponent<RigidBody2D>()->SetEnabled(false);
                    auto a = GetGameObject()->GetComponent<ActionList>();
                    auto frameDur = anim->GetFrameDuration();
                    glm::vec2 newSize =
                        GetGameObject()->GetComponent<BoxCollider2D>()->GetLocalSize() *
                        mExplosionRadiusMultiplier;
                    a->AddAction(new ColliderScaleAction(newSize,
                                                         GetGameObject(),
                                                         1,
                                                         frameDur * 1.f,
                                                         frameDur * 1,
                                                         true,
                                                         BaseAction::EaseType::Linear));
                    a->AddAction(new ColliderScaleAction(glm::vec2(0),
                                                         GetGameObject(),
                                                         1,
                                                         frameDur,
                                                         frameDur * 2,
                                                         true,
                                                         BaseAction::EaseType::Linear));
                    deathAnimFirstTime = false;
                }
            }
            GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));
            //Wait for death animation to finish, then destroy
            if (anim->GetIsDone())
            {
                GetGameObject()->GetComponent<BoxCollider2D>()->SetEnabled(false);
                GetGameObject()->GetComponent<RigidBody2D>()->SetEnabled(false);
                GetGameObject()->Destroy();
            }
            return;
        }

        //normal death
        if (!mIsInvincible && curhp <= 0)
        {
            mDying = true;
            GetGameObject()->GetComponent<EnemyAgent>()->SetEnabled(false);
            GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));

            // CrawlEnemy killed without explosion — fade out
            if (enemyType == EnemyType::CrawlEnemy)
            {
                auto* sprite = GetGameObject()->GetComponent<SpriteRenderer>();
                GetGameObject()->GetComponent<BoxCollider2D>()->SetEnabled(false);
                GetGameObject()->GetComponent<RigidBody2D>()->SetEnabled(false);
                float alpha = sprite->GetAlpha();
                alpha -= dt * 2.0f;
                if (alpha <= 0.0f)
                {
                    GetGameObject()->Destroy();
                }
                else
                {
                    sprite->SetAlpha(alpha);
                }
                return;
            }

            // non CrawlEnemy death
            GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                AnimationSystem::States::Death);
            if (enemyType == EnemyType::FlyEnemy && deathAnimFirstTime)
            {
                if (mAudioController)
                    mAudioController->PlayFlyExplode();
                deathAnimFirstTime = false;
            }
            GetGameObject()->GetComponent<BoxCollider2D>()->SetEnabled(false);
            GetGameObject()->GetComponent<RigidBody2D>()->SetEnabled(false);

            if (GetGameObject()->GetComponent<Animation>()->GetIsDone())
            {
                GetGameObject()->Destroy();
            }
            return;
        }

        auto a = GetGameObject()->GetComponent<EnemyAgent>();
        const glm::vec2 enemyPos = GetGameObject()->GetComponent<Transform>()->GetPosition();
        auto* playerRef = a->GetPlayerReference();
        const glm::vec2 playerPos = playerRef ? glm::vec2(playerRef->GetComponent<Transform>()->GetPosition()) : enemyPos;
        const float xDist = std::abs(playerPos.x - enemyPos.x);
        const float yDist = std::abs(playerPos.y - enemyPos.y);

        if (xDist <= mAggroRange && yDist <= mAggroYRange)
        {
            if (!alerted)
            {
                LOG_INFO("Player entered aggro range");
                alerted = true;
                if (mAudioController)
                    mAudioController->PlayAlertSound(enemyType);
                GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                    AnimationSystem::States::Alerted);

                const glm::vec2 toPlayer = playerPos - enemyPos;
                const float dist = glm::length(toPlayer);
                glm::vec2 dir = (dist > 0.0001f) ? (toPlayer / dist) : glm::vec2{0.0f, 0.0f};
                if (dir.x < 0)
                {
                    GetGameObject()->GetComponent<SpriteRenderer>()->SetFlipX(true);
                }
                else
                {
                    GetGameObject()->GetComponent<SpriteRenderer>()->SetFlipX(false);
                }
            }
        }
        else
        {
            if (alerted)
            {
                alerted = false;
                GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                    AnimationSystem::States::Walking);
            }
        }
    }

    void EnemyController::OnFixedUpdate(float)
    {
        if (enemyType == EnemyType::FlyEnemy)
        {
            glm::vec2 velocity = GetGameObject()->GetComponent<RigidBody2D>()->GetVelocity();
            velocity.x = mDesiredVelocity.x;
            velocity.y = 0;
            GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(velocity);
        }
        if (enemyType == EnemyType::SentryEnemy && alerted)
        {
            GetGameObject()->GetComponent<RigidBody2D>()->SetVelocity(glm::vec2(0));
        }
    }

    void EnemyController::PlayBossMoanSFX()
    {
        mAudioController->PlayBossMoan();
    }

    void EnemyController::SetEnemyPlans(std::string name)
    {
        if (name == "SlugEnemy")
        {
            enemyType = EnemyType::CrawlEnemy;
            GetGameObject()->GetComponent<mts::EnemyAgent>()->Initialize(
                mts::AIPlanner::GetPlannerById("SlugEnemy"));
        }
        else if (name == "FlyEnemy")
        {
            enemyType = EnemyType::FlyEnemy;
            GetGameObject()->GetComponent<mts::EnemyAgent>()->Initialize(
                mts::AIPlanner::GetPlannerById("FlyEnemy"));
        }
        else if (name == "SentryEnemy")
        {
            enemyType = EnemyType::SentryEnemy;
            GetGameObject()->GetComponent<mts::EnemyAgent>()->Initialize(
                mts::AIPlanner::GetPlannerById("SentryEnemy"));
        }
        else if (name == "BossEnemy")
        {
            enemyType = EnemyType::BossEnemy;
            GetGameObject()->GetComponent<mts::EnemyAgent>()->Initialize(
                mts::AIPlanner::GetPlannerById("BossEnemy"));
        }
    }

    void EnemyController::DeserializeFields(mts::JsonValue const& fields)
    {
        if (fields.Has("EnemyType"))
        {
            type = fields["EnemyType"].GetString("");
        }
        auto& p = GetGameObject()->GetComponent<EnemyAgent>()->GetState().params;
        if (fields.Has("MoveSpeed"))
        {
            mMoveSpeed = fields["MoveSpeed"].GetFloat();
        }
        if (fields.Has("PatrolRadius"))
        {
            mPatrolRadius = fields["PatrolRadius"].GetFloat();
        }
        if (fields.Has("AggroRange"))
        {
            mAggroRange = fields["AggroRange"].GetFloat();
            LOG_INFO("Setting aggro range: {}", mAggroRange);
            p.emplace(mts::HashAction("AggroRange"), new mts::AggroRangeParam());
            p.at(HashAction("AggroRange"))->SetValue(mAggroRange);
        }

        if (fields.Has("AttackRange"))
        {
            mAttackRange = fields["AttackRange"].GetFloat();
            p.emplace(mts::HashAction("AttackRange"), new AttackRangeParam());
            p.at(HashAction("AttackRange"))->SetValue(mAttackRange);
        }
        if (fields.Has("AggroYRange"))
        {
            mAggroYRange = fields["AggroYRange"].GetFloat();
        }
        p.emplace(mts::HashAction("AggroYRange"), new mts::AggroYRangeParam());
        p.at(HashAction("AggroYRange"))->SetValue(mAggroYRange);
        if (fields.Has("AttackCooldown"))
        {
            mAttackCooldown = fields["AttackCooldown"].GetFloat();
            p.emplace(mts::HashAction("AttackCooldown"), new mts::AttackCountdownParam());
            dynamic_cast<AttackCountdownParam*>(p.at(HashAction("AttackCooldown")))
                ->SetCooldownTime(mAttackCooldown);
        }
        if (fields.Has("Attack2Cooldown"))
        {
            mAttack2Cooldown = fields["Attack2Cooldown"].GetFloat();
            p.emplace(mts::HashAction("Attack2Cooldown"), new mts::AttackCountdownParam());
            dynamic_cast<AttackCountdownParam*>(p.at(HashAction("Attack2Cooldown")))
                ->SetCooldownTime(mAttack2Cooldown);
        }
        if (fields.Has("Attack3Cooldown"))
        {
            mAttack3Cooldown = fields["Attack3Cooldown"].GetFloat();
            p.emplace(mts::HashAction("Attack3Cooldown"), new mts::AttackCountdownParam());
            dynamic_cast<AttackCountdownParam*>(p.at(HashAction("Attack3Cooldown")))
                ->SetCooldownTime(mAttack3Cooldown);
        }
        if (fields.Has("DodgeCooldown"))
        {
            mDodgeCooldown = fields["DodgeCooldown"].GetFloat();
            p.emplace(mts::HashAction("DodgeCooldown"), new mts::AttackCountdownParam());
            dynamic_cast<AttackCountdownParam*>(p.at(HashAction("DodgeCooldown")))
                ->SetCooldownTime(mDodgeCooldown);
        }
        if (fields.Has("HealthPoints"))
        {
            maxhp = fields["HealthPoints"].GetInt();
            curhp = maxhp;
        }
        if (fields.Has("Explosion Radius"))
        {
            mExplosionRadiusMultiplier = fields["Explosion Radius"].GetFloat();
        }
        if (fields.Has("Explosion Timer"))
        {
            mTimeTillExplode = fields["Explosion Timer"].GetFloat();
        }
        p.emplace(mts::HashAction("PlayerHealthRatio"), new mts::PlayerHealthRatioParam());
        p.at(HashAction("PlayerHealthRatio"))->SetValue(1.0f);
        if (fields.Has("IsInvincible"))
            mIsInvincible = fields["IsInvincible"].GetBool(false);
    }
    void EnemyController::SerializeFields(mts::JsonDocument& out) const
    {
        out.WriteString("EnemyType", type.c_str());
        out.WriteFloat("MoveSpeed", mMoveSpeed);
        out.WriteFloat("PatrolRadius", mPatrolRadius);
        out.WriteFloat("AggroRange", mAggroRange);
        out.WriteFloat("AggroYRange", mAggroYRange);
        out.WriteFloat("AttackCooldown", mAttackCooldown);
        out.WriteFloat("Attack2Cooldown", mAttack2Cooldown);
        out.WriteFloat("Attack3Cooldown", mAttack3Cooldown);
        out.WriteFloat("AttackRange", mAttackRange);
        out.WriteInt("HealthPoints", maxhp);
        out.WriteFloat("DodgeCooldown", mDodgeCooldown);
        out.WriteBool("IsInvincible", mIsInvincible);
        out.WriteFloat("Explosion Radius", mExplosionRadiusMultiplier);
        out.WriteFloat("Explosion Timer", mTimeTillExplode);
    }
    void EnemyController::GetInspectorFields(std::vector<mts::ScriptField>& fields)
    {
        fields.push_back({.name = "MoveSpeed",
                          .type = mts::ScriptField::Type::Float,
                          .ptr = {.f = &mMoveSpeed},
                          .step = 0.1f});
        fields.push_back({.name = "PatrolRadius",
                          .type = mts::ScriptField::Type::Float,
                          .ptr = {.f = &mPatrolRadius},
                          .step = 0.1f});
        fields.push_back({.name = "AggroRange",
                          .type = mts::ScriptField::Type::Float,
                          .ptr = {.f = &mAggroRange},
                          .step = 0.1f});
        fields.push_back({.name = "AggroYRange",
                          .type = mts::ScriptField::Type::Float,
                          .ptr = {.f = &mAggroYRange},
                          .step = 0.1f});
        fields.push_back({.name = "AttackRange",
                          .type = mts::ScriptField::Type::Float,
                          .ptr = {.f = &mAttackRange},
                          .step = 0.1f});
        fields.push_back({.name = "AttackCooldown",
                          .type = mts::ScriptField::Type::Float,
                          .ptr = {.f = &mAttackCooldown},
                          .step = 0.1f});
        fields.push_back({.name = "Attack2Cooldown",
                          .type = mts::ScriptField::Type::Float,
                          .ptr = {.f = &mAttack2Cooldown},
                          .step = 0.1f});
        fields.push_back({.name = "Attack3Cooldown",
                          .type = mts::ScriptField::Type::Float,
                          .ptr = {.f = &mAttack3Cooldown},
                          .step = 0.1f});
        fields.push_back({.name = "DodgeCooldown",
                          .type = mts::ScriptField::Type::Float,
                          .ptr = {.f = &mDodgeCooldown},
                          .step = 0.1f});

        fields.push_back(
            {.name = "HealthPoints", .type = mts::ScriptField::Type::Int, .ptr = {.i = &maxhp}});

        fields.push_back({.name = "IsInvincible",
                          .type = mts::ScriptField::Type::Bool,
                          .ptr = {.b = &mIsInvincible}});
        fields.push_back({.name = "Explosion Radius",
                          .type = mts::ScriptField::Type::Float,
                          .ptr = {.f = &mExplosionRadiusMultiplier},
                          .step = 0.1f});
        fields.push_back({.name = "Explosion Timer",
                          .type = mts::ScriptField::Type::Float,
                          .ptr = {.f = &mTimeTillExplode},
                          .step = 0.1f});
    }
    void EnemyController::OnCollisionEnter2D(const mts::Collision2D& collision)
    {
        auto* hitbox = collision.other->GetGameObject()->GetScript<AttackHitboxScript>();
        if (hitbox && hitbox->GetTeam() == AttackHitboxScript::Team::Player)
        {
            // LOG_INFO("hit");
            float damage = hitbox->GetDamage();
            // LOG_INFO("before hit cur hp {}, is invin: {}", curhp, mIsInvincible);
            if (!mIsInvincible)
                TakeDamage(damage);
            /*LOG_INFO("cur hp {}, is invin: {}", curhp, mIsInvincible);*/

            if (mAudioController && enemyType != EnemyType::BossEnemy)
                mAudioController->PlayHitSound(enemyType);
            else
            {
                PlayBossMoanSFX();
            }

            if (mPlayerController)
            {
                glm::vec3 pos = GetGameObject()->GetComponent<Transform>()->GetPosition();
                mPlayerController->OnEnemyHit(damage, pos);
            }
        }
        if (collision.other->GetGameObject()->HasTag("Player") &&
            enemyType == EnemyType::CrawlEnemy && (curhp <= 0 || !mDying))
        {
            collision.other->GetGameObject()->GetScript<PlayerController>()->TakeDamage(1);
        }
        if (collision.other->GetGameObject()->HasTag("TilemapCollider"))
        {
            auto* box = GetGameObject()->GetComponent<mts::BoxCollider2D>();
            if (!box)
                return;

            const glm::vec2 contact = collision.contactPoint;
            const glm::vec2 center = box->GetCenter();

            auto boxHalf = box->GetHalfExtents();
            const float bottom = center.y - boxHalf.y;

            const float cutoff = bottom + boxHalf.y * 0.25;

            if (contact.y <= cutoff)
                return;

            GetGameObject()->GetComponent<SpriteRenderer>()->SetFlipX(
                !GetGameObject()->GetComponent<SpriteRenderer>()->GetFlipX());
        }
    }

}  // namespace mts