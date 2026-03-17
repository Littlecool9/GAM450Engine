/**
 * @file HazardCollider.cpp
 * @author 11210
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "HazardCollider.h"

#include "Engine/Physics2D/Collision2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

#include "PlayerController.h"

void HazardCollider::OnCollisionEnter2D(const mts::Collision2D& collision)
{
	if (collision.other->GetGameObject()->GetName() == "Player")
	{
		PlayerController* playerScript =
            collision.other->GetGameObject()->GetScript<PlayerController>();
		if (playerScript)
		{
            playerScript->ReturnToCheckPoint();
		}
	}
}
