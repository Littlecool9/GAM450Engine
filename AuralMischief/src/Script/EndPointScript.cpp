/**
 * @file EndPointScript.cpp
 * @author sumin.park
 * @brief  trigger win
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EndPointScript.h"

#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/Physics2D/Collision2D.h"
#include "Engine/SceneSystem/SceneSystem.h"

void EndPointScript::OnCollisionEnter2D(const mts::Collision2D& collision)
{
    if (collision.other->GetGameObject()->HasTag("Player"))
        mts::SceneSystem::SwitchScene(2);
}