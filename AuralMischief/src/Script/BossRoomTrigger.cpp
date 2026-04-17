/**
 * @file BossRoomTrigger.cpp
 * @author sumin.park
 * @brief  Tracks whether the player is currently inside the boss room trigger zone
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "BossRoomTrigger.h"

#include "Engine/Physics2D/Collision2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

void BossRoomTrigger::OnDisable()
{
    mPlayerInside = false;
}

void BossRoomTrigger::OnCollisionExit2D(mts::Collision2D const& collision)
{
    if (collision.other->GetGameObject()->GetName() == "Player")
        mPlayerInside = true;
}
