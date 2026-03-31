/**
 * @file BulletBehavior.cpp
 * @author Dong Ta
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "BulletBehavior.h"

#include "../Player/PlayerController.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/Physics2D/Collision2D.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    void BulletBehavior::Start()
    {
        auto* playerGO = SceneSystem::GetCurrentScene()->FindGameObjectByName("Player");
        if (playerGO)
            playerTransform = playerGO->GetComponent<Transform>();
    }

    void BulletBehavior::OnUpdate(float dt)
    {
        lifetime -= dt;
        if (lifetime <= 0 || mDestroy)
        {

            GetGameObject()->Destroy();
            return;
        }

        if (playerTransform == nullptr ||
            GetGameObject()->GetComponent<Transform>() == nullptr)
            return;

        glm::vec2 bulletPos = GetGameObject()->GetComponent<Transform>()->GetPosition();
        glm::vec2 playerPos = playerTransform->GetPosition();

        glm::vec2 toPlayer = playerPos - bulletPos;


        if (glm::length(toPlayer) > 0.0001f)
            toPlayer = glm::normalize(toPlayer);
        else
            toPlayer = mDirection;

        float homingStrength = 2.0f;


        glm::vec2 newDir = glm::normalize(glm::mix(mDirection, toPlayer, homingStrength * dt));
        float angle = std::atan2(mDirection.y, mDirection.x);
        GetGameObject()->GetComponent<Transform>()->SetRotation(angle + glm::pi<float>());  
        mDirection = newDir;


        GetGameObject()->GetComponent<Transform>()->SetPosition(
            bulletPos + mDirection * mBulletSpeed * dt);
    }

    void BulletBehavior::DeserializeFields(JsonValue const& fields)
    {
        LOG_INFO("Bullet");
    }

    void BulletBehavior::SerializeFields(JsonDocument& out) const
    {
        
    }

    void BulletBehavior::OnCollisionEnter2D(const Collision2D& collision)
    {
        if (collision.other->GetGameObject()->GetName() == "Player")
        {
            //GetGameObject()->GetComponent<BoxCollider2D>()->SetEnabled(false);
            GetGameObject()->SetActive(false);  
           // GetGameObject()->Destroy();
        }
    }
}  // namespace mts