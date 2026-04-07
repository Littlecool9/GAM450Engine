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

#include "EnemyController.h"
#include "../Player/PlayerController.h"

#include "Engine/Audio/AudioManager.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/RigidBody2D.h"

#include "Engine/Physics2D/Collision2D.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/Animation.h"
#include "Engine/Resource/ResourceManager.h"

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
        // If dying, wait for the death animation to finish, then destroy
        if (mDying)
        {
            if (GetGameObject()->GetComponent<Animation>()->GetIsDone())
            {
                GetGameObject()->Destroy();
            }
            return;
        }

        lifetime -= dt;
        if (lifetime <= 0 || mDestroy)
        {
            BeginDeath();
            return;
        }

        if (GetGameObject()->GetComponent<Transform>() == nullptr)
            return;

        glm::vec2 bulletPos = GetGameObject()->GetComponent<Transform>()->GetPosition();


        if (mHoming)
        {
            if (playerTransform == nullptr)
                return;

            glm::vec2 playerPos = playerTransform->GetPosition();
            glm::vec2 toPlayer = playerPos - bulletPos;

            if (glm::length(toPlayer) > 0.0001f)
                toPlayer = glm::normalize(toPlayer);
            else
                toPlayer = mDirection;

            float homingStrength = 2.0f;

            glm::vec2 newDir = glm::normalize(glm::mix(mDirection, toPlayer, homingStrength * dt));
            mDirection = newDir;
        }


        float angle = std::atan2(mDirection.y, mDirection.x);
        GetGameObject()->GetComponent<Transform>()->SetRotation(angle + glm::pi<float>());

        GetGameObject()->GetComponent<Transform>()->SetPosition(bulletPos +
                                                                mDirection * mBulletSpeed * dt);
    }


    void BulletBehavior::DeserializeFields(JsonValue const& fields)
    {
        // LOG_INFO("Bullet");
        if (fields.Has("BulletSpeed"))
        {
            mBulletSpeed = fields["BulletSpeed"].GetFloat();
        }
        if (fields.Has("Lifetime"))
        {
            lifetime = fields["Lifetime"].GetFloat();
        }
    }

    void BulletBehavior::SerializeFields(JsonDocument& out) const
    {
        out.WriteFloat("BulletSpeed", mBulletSpeed);
        out.WriteFloat("Lifetime", lifetime);
    }

    void BulletBehavior::OnCollisionEnter2D(const Collision2D& collision)
    {

        if (mDying)
            return;

        std::string otherName = collision.other->GetGameObject()->GetName();


        if (collision.other->GetGameObject()->HasTag("TilemapCollider"))
        {
            BeginDeath();
            return;
        }


        if (mOwner == BulletOwner::Enemy && otherName == "Player" && !mDying)
        {
            collision.other->GetGameObject()->GetScript<PlayerController>()->TakeDamage(
                static_cast<int>(dmg));
            BeginDeath();
            return;
        }


        if (mOwner == BulletOwner::Player)
        {
            auto* enemyCtrl = collision.other->GetGameObject()->GetScript<EnemyController>();
            if (enemyCtrl)
            {
                enemyCtrl->TakeDamage(static_cast<int>(dmg) * 3);
                BeginDeath();
                return;
            }
        }


        if (collision.other->GetGameObject()->HasTag("AttackHitBox"))
        {
            mts::AudioManager& audio = mts::AudioManager::Get();
            if (audio.IsInBeatWindow(0.1f, 0.1f))
            {
                LOG_INFO("Bullet deflected by attack");

                mDirection = -mDirection;

                mOwner = BulletOwner::Player;

                mHoming = false;

                lifetime = 5.f;
            }
            else
            {
                LOG_INFO("Bullet hit by attack but not in beat window, not deflected");
            }
        }
        else
        {
            LOG_INFO("Not hitbox");
        }
    }

    void BulletBehavior::BeginDeath()
    {
        if (mDying)
            return;
        LOG_INFO("Disabling stuff");
        mDying = true;

        // Disable collision so the bullet doesn't interact with anything during death
        auto* collider = GetGameObject()->GetComponent<BoxCollider2D>();
        if (collider)
            collider->SetEnabled(false);

        auto* rb = GetGameObject()->GetComponent<RigidBody2D>();
        if (rb)
            rb->SetEnabled(false);

        // Switch to explosion spritesheet and play death animation
        GetGameObject()->GetComponent<SpriteRenderer>()->SetTexture(
            ResourceManager::GetTexture("Data/Resources/ExplosionTexture.tex"));
        GetGameObject()->GetComponent<AnimationSystem>()->ChangeState(
            AnimationSystem::States::Death);
    }
}  // namespace mts