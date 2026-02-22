/**
 * @file PlayerController.h
 * @author sumin.park
 * @brief  Player behavior script
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/EntitySystem/Component/AnimationSystem.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/RigidBody2D.h"
#include "Engine/Scripting/ScriptBase.h"

class PlayerController : public mts::ScriptBase
{
   public:
    void OnCreate() override;
    void OnUpdate(float dt) override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;

    void OnCollisionEnter2D(const mts::GameObject*) override;

   private:
    mts::RigidBody2D* rb = nullptr;
    mts::AnimationSystem* anim = nullptr;
    mts::SpriteRenderer* sprite = nullptr;
    mts::Transform* transform = nullptr;
    float moveSpeed = 5.0f;
    float jumpSpeed = 5.0f;
    bool isGrounded = false;
    float walkParticleTimerLength = 0.5f;
    float walkParticleTimer = 1.0f;
};
