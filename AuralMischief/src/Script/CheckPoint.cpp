/**
 * @file CheckPoint.cpp
 * @author 11210
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "CheckPoint.h"

#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Physics2D/Collision2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/EntitySystem/Component/Transform.h"

#include "Player/PlayerController.h"

void CheckPoint::OnCollisionEnter2D(mts::Collision2D const& collision)
{
    if (collision.other->GetGameObject()->GetName() == "Player")
    {
        PlayerController* playerScript =
            collision.other->GetGameObject()->GetScript<PlayerController>();
        if (!playerScript)
        {
            LOG_WARN("[CheckPoint] Collided with Player but no PlayerController script found");
            return;
        }
        playerScript->SetCheckPoint(this);
    }
}

glm::vec3 CheckPoint::GetCheckPointPosition() const
{
    mts::Transform* transform = GetGameObject()->GetComponent<mts::Transform>();
    if (!transform)
    {
        LOG_ERROR("[CheckPoint] No Transform component found on CheckPoint");
        return glm::vec3(0.0f);
    }
    return transform->GetPosition();
}
