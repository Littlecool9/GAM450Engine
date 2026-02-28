/**
 * @file PlayerController.h
 * @author josh.siegel
 * @brief // Player movement/animation script
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/EntitySystem/Component/AnimationSystem.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Rigidbody2D.h"
#include "Engine/Scripting/ScriptBase.h"

#include <glm/common.hpp>

class PlayerController : public mts::ScriptBase
{
    enum Direction
    {
        NONE = 0,
        UP,
        UP_RIGHT,
        RIGHT,
        DOWN_RIGHT,
        DOWN,
        DOWN_LEFT,
        LEFT,
        UP_LEFT
    };

   public:
    void OnCreate() override;
    void OnUpdate(float dt) override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;

    void OnCollisionEnter2D(const mts::Collision2D& collision) override;

    static float MoveToward(float val_from, float val_to, float delta)
    {
        return glm::abs(val_to - val_from) <= delta
                   ? val_to
                   : val_from + glm::sign(val_to - val_from) * delta;
    }

    SCRIPT_CLASS(PlayerController)

   private:
    mts::RigidBody2D* rb = nullptr;
    mts::AnimationSystem* anim = nullptr;
    mts::SpriteRenderer* sprite = nullptr;
    mts::Transform* transform = nullptr;
    float moveSpeed = 5.0f;
    float jumpSpeed = 12.0f;
    float dashSpeed = 12.0f;
    float friction = 20.0f;
    float acceleration = 40.0f;
    float deceleration = 15.0f;
    float dashDampening = 0.8f;
    float turnSpeed = 100.0f;
    bool isGrounded = false;
    int dashCount = 1;
    int dashMax = 1;
    float dashCooldown = 0.2f;
    float dashTimer = -1.0f;
    Direction facingDir = RIGHT;
};