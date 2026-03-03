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

#include "Engine/Physics2D/Collision2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Rigidbody2D.h"
#include "Engine/EntitySystem/Component/AnimationSystem.h"
#include "Engine/EntitySystem/Component/SpriteRenderer.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/Audio/AudioManager.h"
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

    // Get current audio state for beat check
    mts::AudioManager& audio = mts::AudioManager::Get();
    bool inWindow = audio.IsInBeatWindow(0.100f, 0.100f);


    // Reset dash count if on ground and dash cooldown is done
    if (isGrounded && dashTimer < 0.0f)
    {
        dashCount = dashMax;
    }

    if (mts::Input::IsKeyDown(mts::Key::Q))
    {
        GetGameObject()->GetComponent<mts::AnimationSystem>()->ChangeState(
            mts::AnimationSystem::States::Attack1);
        return;
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
        if (!jumpRhythmLock || (jumpRhythmLock && inWindow))
        {
            isGrounded = false;

            // currently collider is getting stuck in ground occasionally
            // This moves player a tiny bit up to make sure it doesn't get stuck
            // TODO: remove if physics collider fixed
            transform->SetPosition(
                glm::vec2(transform->GetPosition().x, transform->GetPosition().y + 0.05f));

            // multiply jump speed if hit on beat
            float adjustedJumpSpeed = inWindow ? jumpSpeed * jumpBeatBoost : jumpSpeed;
            currVelocity.y = adjustedJumpSpeed;
            // Reset dash cooldown on jump
            dashTimer = -1.0f;
            dashCount = dashMax;
        }
        else
        {
            LOG_INFO("Jumped off beat");
        }
    }

    // Dash calculation
    if (dashPressed && dashCount > 0)
    {
        if (!dashRhythmLock || (dashRhythmLock && inWindow))
        {
            // currently collider is getting stuck in ground occasionally
            // This moves player a tiny bit up to make sure it doesn't get stuck
            // TODO: remove if physics collider fixed
            transform->SetPosition(
                glm::vec2(transform->GetPosition().x, transform->GetPosition().y + 0.05f));

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
            // Multiply dash speed if hit on beat
            float adjustedDashSpeed = inWindow ? dashSpeed * dashBeatBoost : dashSpeed;

            // Set velocity directly in direction of dash but don't change velocity where component
            // of dash direction is zero
            currVelocity =
                (currVelocity * (1.0f - glm::abs(dashDir))) + adjustedDashSpeed * dashDir;
            --dashCount;
            dashTimer = dashCooldown;
        }
        else
        {
            LOG_INFO("Dashed off beat");
        }
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

    // Possible animation/sprite/color change while dash is in cooldown
    if (dashTimer > 0.0f)
        sprite->SetTintColor(glm::vec3(0.7f));
    else
        sprite->SetTintColor(glm::vec3(1.0f));
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
    if (fields.Has("turnSpeed"))
        turnSpeed = fields["turnSpeed"].GetFloat(turnSpeed);
    if (fields.Has("dashDampening"))
        dashDampening = fields["dashDampening"].GetFloat(dashDampening);
    if (fields.Has("dashCooldown"))
        dashCooldown = fields["dashCooldown"].GetFloat(dashCooldown);
    if (fields.Has("dashCount"))
        dashCount = fields["dashCount"].GetInt(dashCount);
    if (fields.Has("dashBeatBoost"))
        dashBeatBoost = fields["dashBeatBoost"].GetFloat(dashBeatBoost);
    if (fields.Has("jumpBeatBoost"))
        jumpBeatBoost = fields["jumpBeatBoost"].GetFloat(jumpBeatBoost);
    if (fields.Has("dashRhythmLock"))
        dashRhythmLock = fields["dashRhythmLock"].GetBool(dashRhythmLock);
    if (fields.Has("jumpRhythmLock"))
        jumpRhythmLock = fields["jumpRhythmLock"].GetBool(jumpRhythmLock);
}

void PlayerController::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteFloat("moveSpeed", moveSpeed);
    out.WriteFloat("jumpSpeed", jumpSpeed);
    out.WriteFloat("dashSpeed", dashSpeed);
    out.WriteFloat("friction", friction);
    out.WriteFloat("acceleration", acceleration);
    out.WriteFloat("deceleration", deceleration);
    out.WriteFloat("turnSpeed", turnSpeed);
    out.WriteFloat("dashDampening", dashDampening);
    out.WriteFloat("dashCooldown", dashCooldown);
    out.WriteInt("dashCount", dashCount);
    out.WriteFloat("dashBeatBoost", dashBeatBoost);
    out.WriteFloat("jumpBeatBoost", jumpBeatBoost);
    out.WriteBool("dashRhythmLock", dashRhythmLock);
    out.WriteBool("jumpRhythmLock", jumpRhythmLock);
}

void PlayerController::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "moveSpeed",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &moveSpeed},
                      .step = 0.1f});
    fields.push_back({.name = "jumpSpeed",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &jumpSpeed},
                      .step = 0.1f});
    fields.push_back({.name = "dashSpeed",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &dashSpeed},
                      .step = 0.1f});
    fields.push_back({.name = "friction",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &friction},
                      .step = 0.1f});
    fields.push_back({.name = "acceleration",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &acceleration},
                      .step = 0.1f});
    fields.push_back({.name = "deceleration",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &deceleration},
                      .step = 0.1f});
    fields.push_back({.name = "turnSpeed",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &turnSpeed},
                      .step = 0.1f});
    fields.push_back({.name = "dashDampening",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &dashDampening},
                      .step = 0.1f});
    fields.push_back({.name = "dashCooldown",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &dashCooldown},
                      .step = 0.1f});
    fields.push_back({.name = "dashCount",
                      .type = mts::ScriptField::Type::Int,
                      .ptr = {.i = &dashCount},
                      .step = 1});
    fields.push_back({.name = "dashBeatBoost",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &dashBeatBoost},
                      .step = 0.1f});
    fields.push_back({.name = "jumpBeatBoost",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &jumpBeatBoost},
                      .step = 0.1f});
    fields.push_back({.name = "dashRhythmLock",
                      .type = mts::ScriptField::Type::Bool,
                      .ptr = {.b = &dashRhythmLock}});
    fields.push_back({.name = "jumpRhythmLock",
                      .type = mts::ScriptField::Type::Bool,
                      .ptr = {.b = &jumpRhythmLock}});
}

void PlayerController::OnCollisionEnter2D(const mts::Collision2D& collision)
{
    if (collision.other->GetGameObject()->GetLayer() == mts::GameObject::Ground)
    {
        LOG_INFO("Collision normal dot up: {}", glm::dot(glm::vec2(0.0f, -1.0f), collision.normal));
        if (abs(glm::dot(glm::vec2(0.0f, -1.0f), collision.normal) - 1.0f) < 0.1f)
        {
            isGrounded = true;
        }
    }
}

void PlayerController::OnCollisionStay2D(const mts::Collision2D& collision)
{
    if (collision.other->GetGameObject()->GetLayer() == mts::GameObject::Ground)
    {
        if (abs(glm::dot(glm::vec2(0.0f, -1.0f), collision.normal) - 1.0f) < 0.1f)
        {
            isGrounded = true;
        }
    }
}

void PlayerController::OnCollisionExit2D(const mts::Collision2D& collision)
{
    if (collision.other->GetGameObject()->GetLayer() == mts::GameObject::Ground)
    {
        LOG_INFO("Collision exit normal dot up: {}",
                 glm::dot(glm::vec2(0.0f, -1.0f), collision.normal));
        if (abs(glm::dot(glm::vec2(0.0f, -1.0f), collision.normal) + 1.0f) < 0.1f)
        {
            isGrounded = false;
        }
    }
}