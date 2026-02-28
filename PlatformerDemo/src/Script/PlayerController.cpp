/**
 * @file PlayerController.cpp
 * @author josh.siegel
 * @brief // Player movement/animation script
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "PlayerController.h"

#include "Engine/Physics2D/Collision2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/Input/Input.h"
#include <glm/vec2.hpp>

void PlayerController::OnCreate()
{
    rb = GetGameObject()->GetComponent<mts::RigidBody2D>();
    anim = GetGameObject()->GetComponent<mts::AnimationSystem>();
    sprite = GetGameObject()->GetComponent<mts::SpriteRenderer>();
    transform = GetGameObject()->GetComponent<mts::Transform>();
}

void PlayerController::OnUpdate(float dt)
{
    // Check all important held keys at start of loop
    // This cleans up the code and reduces function calls
    bool upHeld = mts::Input::IsKeyDown(mts::Key::Up);
    bool downHeld = mts::Input::IsKeyDown(mts::Key::Down);
    bool leftHeld = mts::Input::IsKeyDown(mts::Key::Left);
    bool rightHeld = mts::Input::IsKeyDown(mts::Key::Right);
    glm::vec2 inputDir = {static_cast<int>(rightHeld) - static_cast<int>(leftHeld),
                          static_cast<int>(upHeld) - static_cast<int>(downHeld)};
    bool jumpPressed = mts::Input::IsKeyPressed(mts::Key::Space);
    bool dashPressed = mts::Input::IsKeyPressed(mts::Key::LeftShift);

    // Reset dash count if on ground and dash cooldown is done
    if (isGrounded && dashTimer < 0.0f)
    {
        dashCount = dashMax;
    }

    // Get facing direction
    float moveDir = 0.0f;
    if (leftHeld)
    {
        facingDir = LEFT;
        moveDir = -1.0f;
        sprite->SetFlipX(true);
    }
    if (rightHeld)
    {
        facingDir = RIGHT;
        moveDir = 1.0f;
        sprite->SetFlipX(false);
    }

    auto currVelocity = rb->GetVelocity();
    // Calculate horizontal velocity
    if (static_cast<int>(moveDir) != 0)
    {
        // Turning around
        if (currVelocity.x != 0.0f &&
            static_cast<int>(glm::sign(currVelocity.x)) != static_cast<int>(glm::sign(moveDir)))
            currVelocity.x = MoveToward(currVelocity.x, 0.0f, turnSpeed * dt);
        // Accelerating
        else if ((moveDir < 0.0f && (currVelocity.x > moveDir * moveSpeed)) ||
                 ((moveDir > 0.0f && (currVelocity.x < moveDir * moveSpeed))))
            currVelocity.x = MoveToward(currVelocity.x, moveDir * moveSpeed, acceleration * dt);
        // Decelerating
        else
            currVelocity.x = MoveToward(currVelocity.x, moveDir * moveSpeed, deceleration * dt);
    }
    else
        currVelocity.x = MoveToward(currVelocity.x, 0.0f, friction * dt);

    // Jump check
    if (jumpPressed && isGrounded)
    {
        isGrounded = false;
        currVelocity.y = jumpSpeed;
        // Reset dash cooldown on jump
        dashTimer = -1.0f;
        dashCount = dashMax;
    }

    // Dash calculation
    if (dashPressed && dashCount > 0)
    {
        glm::vec2 dashDir{0.0f, 0.0f};
        // If not holding a direction, use direction character is facing (left or right)
        if (inputDir == glm::vec2(0))
            facingDir == LEFT ? dashDir = {-1.0f, 0.0f} : dashDir = {1.0f, 0.0f};
        else
            dashDir = inputDir;
        // Deceleration when above max speed affects dash distance only to left and right
        // dashDampening is used to adjust horizontal dash distance
        if (dashDampening > 1.0f)
        {
            if (static_cast<int>(dashDir.y) == 0)
                dashDir.x *= dashDampening;
        }
        else if (dashDampening < 1.0f)
        {
            if (static_cast<int>(dashDir.y) != 0)
                dashDir.x *= dashDampening;
        }
        // Set velocity directly in direction of dash but don't change velocity where component of
        // dash direction is zero
        currVelocity = (currVelocity * (1.0f - glm::abs(dashDir))) + dashSpeed * dashDir;
        --dashCount;
        dashTimer = dashCooldown;
    }

    dashTimer -= dt;

    rb->SetVelocity(currVelocity);

    // Set animation states based on movement
    if (!isGrounded && currVelocity.y > 0.1f)
        anim->ChangeState(mts::AnimationSystem::States::Jump);
    else if (!isGrounded && currVelocity.y < -0.1f)
        anim->ChangeState(mts::AnimationSystem::States::Falling);
    else if (std::abs(currVelocity.x) > 0.01f)
        anim->ChangeState(mts::AnimationSystem::States::Walking);
    else
        anim->ChangeState(mts::AnimationSystem::States::Idle);
}

void PlayerController::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("moveSpeed"))
        moveSpeed = fields["moveSpeed"].GetFloat(moveSpeed);
    if (fields.Has("jumpSpeed"))
        jumpSpeed = fields["jumpSpeed"].GetFloat(jumpSpeed);
    if (fields.Has("dashSpeed"))
        dashSpeed = fields["dashSpeed"].GetFloat(dashSpeed);
    if (fields.Has("friction"))
        friction = fields["friction"].GetFloat(friction);
    if (fields.Has("acceleration"))
        acceleration = fields["acceleration"].GetFloat(acceleration);
    if (fields.Has("deceleration"))
        deceleration = fields["deceleration"].GetFloat(deceleration);
    if (fields.Has("dashDampening"))
        dashDampening = fields["dashDampening"].GetFloat(dashDampening);
    if (fields.Has("dashCooldown"))
        dashCooldown = fields["dashCooldown"].GetFloat(dashCooldown);
    if (fields.Has("turnSpeed"))
        turnSpeed = fields["turnSpeed"].GetFloat(turnSpeed);
    if (fields.Has("dashCount"))
        dashCount = fields["dashCount"].GetInt(dashCount);
}

void PlayerController::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteFloat("moveSpeed", moveSpeed);
    out.WriteFloat("jumpSpeed", jumpSpeed);
    out.WriteFloat("dashSpeed", dashSpeed);
    out.WriteFloat("friction", friction);
    out.WriteFloat("acceleration", acceleration);
    out.WriteFloat("deceleration", deceleration);
    out.WriteFloat("dashDampening", dashDampening);
    out.WriteFloat("dashCooldown", dashCooldown);
    out.WriteFloat("turnSpeed", turnSpeed);
    out.WriteInt("dashCount", dashCount);
}

void PlayerController::OnCollisionEnter2D(const mts::Collision2D& collision)
{
    if (collision.other->GetGameObject()->GetLayer() == mts::GameObject::Ground)
    {
        isGrounded = true;
    }
}