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

#include "AttackHitboxScript.h"
#include "BarScaler.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/Physics2D/Collision2D.h"



namespace mts
{
    void EnemyController::OnCreate()
    {
        // todo: ActionList deserialization....
        curhp = maxhp;
    }

    void EnemyController::OnUpdate(float dt)
    {
        if (curhp <= 0)
        {
            GetGameObject()->Destroy();
        }
        if (hpBar)
        {
            hpBar->GetScript<mts::BarScaler>()->SetScaleStandard(static_cast<float>(curhp) / maxhp);
        }
    }

    void EnemyController::DeserializeFields(mts::JsonValue const& fields)
    {
    }

    void EnemyController::SerializeFields(mts::JsonDocument& out) const
    {
    }

    void EnemyController::OnCollisionEnter2D(const mts::Collision2D& collision)
    {
        auto p = collision.other->GetGameObject()->GetScript<AttackHitboxScript>();
        if (p)
        {
            curhp -= static_cast<int>(p->GetDamage());
        }
    }

}

