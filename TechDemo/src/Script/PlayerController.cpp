/**
 * @file PlayerController.cpp
 * @author sumin.park
 * @brief  Player behavior script
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "PlayerController.h"

#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Rigidbody2D.h"
#include "Engine/EntitySystem/Component/AnimationSystem.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/Input/Input.h"
#include "Engine/Serialization/SerializationUtility.h"

void PlayerController::OnCreate()
{
    rb = GetGameObject()->GetComponent<mts::RigidBody2D>();
    anim = GetGameObject()->GetComponent<mts::AnimationSystem>();
    sprite = GetGameObject()->GetComponent<mts::SpriteRenderer>();
    transform = GetGameObject()->GetComponent<mts::Transform>();
}

void PlayerController::OnUpdate(float dt)
{
    float dir = 0;
    if (mts::Input::IsKeyDown(mts::Key::Left))
    {
        dir = -1;
        sprite->SetFlipX(true);
    }
    if (mts::Input::IsKeyDown(mts::Key::Right))
    {
        dir = 1;
        sprite->SetFlipX(false);
    }

    auto v = rb->GetVelocity();
    v.x = dir * moveSpeed;
    rb->SetVelocity(v);

    if (isGrounded && mts::Input::IsKeyPressed(mts::Key::Space))
    {
        isGrounded = false;
        rb->AddImpulse({0, jumpSpeed});

        mts::GameObject* particle = mts::GameObjectFactory::CreateGameObject();
        mts::SerializationUtility::DeserializeFromFile(particle, "Data/JumpParticle.json");
        mts::Transform* particleTransform = particle->GetComponent<mts::Transform>();
        if (particleTransform)
        {
            particleTransform->SetPosition(transform->GetPosition() - glm::vec3(0.0f, 0.8f, 0.0f));
        }
    }

    if (!isGrounded && v.y > 0.1f)
        anim->ChangeState(mts::AnimationSystem::States::Jump);
    else if (!isGrounded && v.y < -0.1f)
        anim->ChangeState(mts::AnimationSystem::States::Falling);

    else if (std::abs(v.x) > 0.01f)
    {
        anim->ChangeState(mts::AnimationSystem::States::Walking);
        if (walkParticleTimer < 0.0f)
        {
            mts::GameObject* particle = mts::GameObjectFactory::CreateGameObject();
            mts::SerializationUtility::DeserializeFromFile(particle, "Data/WalkParticle.json");
            mts::Transform* particleTransform = particle->GetComponent<mts::Transform>();
            if (particleTransform)
            {
                particleTransform->SetPosition(transform->GetPosition() -
                                               glm::vec3(0.0f, 0.8f, 0.0f));
            }
            walkParticleTimer = walkParticleTimerLength;
        }
    }
    else if (mts::Input::IsKeyDown(mts::Key::Q))
    {
        anim->ChangeState(mts::AnimationSystem::States::Dance);
    }
    else
        anim->ChangeState(mts::AnimationSystem::States::Idle);

    walkParticleTimer -= dt;
}

void PlayerController::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("moveSpeed"))
        moveSpeed = fields["moveSpeed"].GetFloat(moveSpeed);
    if (fields.Has("jumpSpeed"))
        jumpSpeed = fields["jumpSpeed"].GetFloat(jumpSpeed);
    if (fields.Has("walkParticleCooldown"))
        walkParticleTimerLength = fields["walkParticleCooldown"].GetFloat(walkParticleTimerLength);
}

void PlayerController::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteFloat("moveSpeed", moveSpeed);
    out.WriteFloat("jumpSpeed", jumpSpeed);
    out.WriteFloat("walkParticleCooldown", walkParticleTimerLength);
}

void PlayerController::OnCollisionEnter2D(const mts::GameObject* gameObject)
{
    if (gameObject->GetLayer() == mts::GameObject::Ground)
        isGrounded = true;
}