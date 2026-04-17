/**
 * @file HazardCollider.cpp
 * @author Jackey Cen
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "HazardCollider.h"

#include "Engine/Physics2D/Collision2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

#include "Player/PlayerController.h"

void HazardCollider::OnCollisionEnter2D(const mts::Collision2D& collision)
{
    LOG_INFO("HazardCollider collided with {}", collision.other->GetGameObject()->GetName());
    if (collision.other->GetGameObject()->GetName() == "Player")
    {
        PlayerController* playerScript =
            collision.other->GetGameObject()->GetScript<PlayerController>();
        if (playerScript)
        {
            LOG_INFO("Player hit hazard, returning to checkpoint");
            playerScript->ReturnToCheckPoint();
            playerScript->TakeDamage(1);
        }
    }
}
