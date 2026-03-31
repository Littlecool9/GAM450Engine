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
#include "EnemyActions/EnemyMoveToPlayerAction.h"
#include "EnemyActions/EnemyPatrolAction.h"
#include "EnemyActions/FlyingEnemyAttackAction.h"
#include "EnemyActions/SentryEnemyAttackAction.h"
#include "EnemyActions/SlugEnemyAttackAction.h"
#include "EnemyParameters/AggroRangeParam.h"
#include "EnemyParameters/AttackCountdownParam.h"
#include "EnemyParameters/PlayerHealthRatioParam.h"
#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyActionList.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/Physics2D/Collision2D.h"
#include "Engine/SceneSystem/SceneSystem.h"

REGISTER_ENEMY_ACTION("SlugEnemy", mts::EnemyPatrolAction);
REGISTER_ENEMY_ACTION("SlugEnemy", mts::SlugEnemyAttackAction);

REGISTER_ENEMY_ACTION("SentryEnemy", mts::EnemyPatrolAction);
REGISTER_ENEMY_ACTION("SentryEnemy", mts::SentryEnemyAttackAction);

REGISTER_ENEMY_ACTION("FlyEnemy", mts::EnemyPatrolAction);
REGISTER_ENEMY_ACTION("FlyEnemy", mts::EnemyMoveToPlayerAction);
REGISTER_ENEMY_ACTION("FlyEnemy", mts::FlyingEnemyAttackAction);

namespace mts
{
    void EnemyController::Init()
    {
        // todo: ActionList deserialization....
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
    }

    void EnemyController::OnUpdate(float dt)
    {
        if (!mIsInvincible && curhp <= 0)
        {
            GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                AnimationSystem::States::Death);
            if ( GetGameObject()->GetComponent<Animation>()->GetIsDone())
            {
                GetGameObject()->Destroy();
            }
        }
        auto a = GetGameObject()->GetComponent<EnemyAgent>();
        if (a->GetState().GetDistanceToPlayer() <= mAggroRange && !a->GetState().GetInAction())
        {
            if (!alerted)
            {
                alerted = true;
                GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                    AnimationSystem::States::Alerted);
                const glm::vec2 enemyPos =
                    GetGameObject()->GetComponent<Transform>()->GetPosition();
                auto p = GetGameObject()->GetComponent<EnemyAgent>()->GetPlayerReference();
                const glm::vec2 playerPos = p->GetComponent<Transform>()->GetPosition();

                const glm::vec2 toPlayer = playerPos - enemyPos;
                const float dist =
                    GetGameObject()->GetComponent<EnemyAgent>()->GetState().GetDistanceToPlayer();
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
            else
            {
                if (GetGameObject()->GetComponent<AnimationSystem>()->GetCurrentState() ==
                        AnimationSystem::States::Alerted &&
                    GetGameObject()->GetComponent<Animation>()->GetIsDone())
                {
                    GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                        AnimationSystem::States::Idle);
                }
            }
        }
        else
        {
            alerted = false;
            if (GetGameObject()->GetComponent<AnimationSystem>()->GetCurrentState() ==
                    AnimationSystem::States::Alerted &&
                GetGameObject()->GetComponent<Animation>()->GetIsDone())
            {
                GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
                    AnimationSystem::States::Idle);
            }
        }
    }

    void EnemyController::SetEnemyPlans(std::string name)
    {
        if (name == "SlugEnemy")
        {
            LOG_INFO("Set to slug");
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
        if (fields.Has("AttackCooldown"))
        {
            mAttackCooldown = fields["AttackCooldown"].GetFloat();
            p.emplace(mts::HashAction("AttackCooldown"), new mts::AttackCountdownParam());
            static_cast<AttackCountdownParam*>(p.at(HashAction("AttackCooldown")))
                ->SetCooldownTime(mAttackCooldown);
        }
        if (fields.Has("HealthPoints"))
        {
            maxhp = fields["HealthPoints"].GetInt();
            curhp = maxhp;
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
        out.WriteFloat("AttackCooldown", mAttackCooldown);
        out.WriteBool("IsInvincible", mIsInvincible);
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
        fields.push_back({.name = "AttackCooldown",
                          .type = mts::ScriptField::Type::Float,
                          .ptr = {.f = &mAttackCooldown},
                          .step = 0.1f});
        fields.push_back({.name = "IsInvincible",
                          .type = mts::ScriptField::Type::Bool,
                          .ptr = {.b = &mIsInvincible}});
    }
    void EnemyController::OnCollisionEnter2D(const mts::Collision2D& collision)
    {
        auto* hitbox = collision.other->GetGameObject()->GetScript<AttackHitboxScript>();
        if (hitbox)
        {
            float damage = hitbox->GetDamage();
            if (!mIsInvincible)
                curhp -= static_cast<int>(damage);
            if (mPlayerController)
            {
                glm::vec3 pos = GetGameObject()->GetComponent<Transform>()->GetPosition();
                mPlayerController->OnEnemyHit(damage, pos);
            }
        }
        if (collision.other->GetGameObject()->HasTag("TilemapCollider"))
        {
            GetGameObject()->GetComponent<SpriteRenderer>()->SetFlipX(
                !GetGameObject()->GetComponent<SpriteRenderer>()->GetFlipX());
        }
    }

}  // namespace mts
