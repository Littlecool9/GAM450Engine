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


//#include "BarScaler.h"
#include "../AttackHitboxScript.h"
#include "../PlayerController.h"
#include "../../AuralMischiefScene.h"
#include "EnemyActions/EnemyMoveToPlayerAction.h"
#include "EnemyActions/EnemyPatrolAction.h"
#include "EnemyActions/FlyingEnemyAttackAction.h"
#include "EnemyParameters/AggroRangeParam.h"
#include "EnemyParameters/AttackCountdownParam.h"
#include "EnemyParameters/PatrolRadius.h"
#include "EnemyParameters/PlayerHealthRatioParam.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyActionList.h"
#include "Engine/EntitySystem/Component/EnemyBehavior/EnemyAgent.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/Physics2D/Collision2D.h"
#include "Engine/SceneSystem/SceneSystem.h"
REGISTER_ENEMY_ACTION("FlyEnemy", mts::FlyingEnemyAttackAction);
REGISTER_ENEMY_ACTION("SlugEnemy", mts::EnemyPatrolAction);
REGISTER_ENEMY_ACTION("FlyEnemy", mts::EnemyMoveToPlayerAction);
REGISTER_ENEMY_ACTION("FlyEnemy", mts::EnemyPatrolAction);

namespace mts
{
    void EnemyController::OnCreate()
    {
        // todo: ActionList deserialization....
        curhp = maxhp;

        GetGameObject()->GetComponent<mts::EnemyAgent>()
            ->SetPlayerReference(SceneSystem::GetCurrentScene()
            ->FindGameObjectByName("Player"));
        SetEnemyPlans(type);
        origin = GetGameObject()->GetComponent<Transform>()->GetPosition();
    }

    void EnemyController::OnUpdate(float dt)
    {
        if (curhp <= 0)
        {
            GetGameObject()->Destroy();
        }
    }

    void EnemyController::SetEnemyPlans(std::string name)
    {
        if (name == "Crawl")
        {
            enemyType = EnemyType::CrawlEnemy;
            GetGameObject()->GetComponent<mts::EnemyAgent>()->Initialize(
                mts::AIPlanner::GetPlannerById("SlugEnemy"));
        }
        else if (name == "Fly")
        {
            enemyType = EnemyType::FlyEnemy;
            GetGameObject()->GetComponent<mts::EnemyAgent>()->Initialize(
                mts::AIPlanner::GetPlannerById("FlyEnemy"));
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
            p.emplace(mts::HashAction("AggroRange"), new mts::AggroRangeParam());
            p.at(HashAction("AggroRange"))->SetValue(mAggroRange);
        }
        if (fields.Has("AttackCooldown"))
        {
            mAttackCooldown = fields["AttackCooldown"].GetFloat();
            p.emplace(mts::HashAction("AttackCooldown"), new mts::AttackCountdownParam());
            static_cast<AttackCountdownParam*>(p.at(HashAction("AttackCooldown")))->SetCooldownTime(mAttackCooldown);
        }
        p.emplace(mts::HashAction("PlayerHealthRatio"), new mts::PlayerHealthRatioParam());
    }
    void EnemyController::SerializeFields(mts::JsonDocument& out) const
    {
        out.WriteFloat("MoveSpeed", mMoveSpeed);
        out.WriteFloat("PatrolRadius", mPatrolRadius);
        out.WriteFloat("AggroRange", mAggroRange);
        out.WriteFloat("AttackCooldown", mAttackCooldown);
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
    }
    void EnemyController::OnCollisionEnter2D(const mts::Collision2D& collision)
    {
        auto p = collision.other->GetGameObject()->GetScript<AttackHitboxScript>();
        if (p)
        {
            curhp -= static_cast<int>(p->GetDamage());
        }
        if (!collision.other->GetGameObject()->GetScript<PlayerController>())
        {
            GetGameObject()->GetComponent<SpriteRenderer>()->SetFlipX(
                !GetGameObject()->GetComponent<SpriteRenderer>()->GetFlipX());
        }
    }


}

