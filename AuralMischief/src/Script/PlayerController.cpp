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
#include "Engine/Conductor/ConductorSystem.h"
#include <glm/vec2.hpp>

#include "MovablePlatform.h"
#include "CheckPoint.h"
#include "CameraController.h"
#include "AttackHitboxScript.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/Serialization/SerializationUtility.h"

void PlayerController::OnCreate()
{
    rb = GetGameObject()->GetComponent<mts::RigidBody2D>();
    anim = GetGameObject()->GetComponent<mts::AnimationSystem>();
    sprite = GetGameObject()->GetComponent<mts::SpriteRenderer>();
    transform = GetGameObject()->GetComponent<mts::Transform>();

    defaultGravityScale = rb->GetGravityScale();
}

void PlayerController::OnUpdate(float dt)
{
    // Check all important held keys at start of loop
    // This cleans up the code and reduces function calls
    float upStrength = mts::Input::IsKeyDown(mts::Key::Up) ? 1.0f : 0.0f;
    float downStrength = mts::Input::IsKeyDown(mts::Key::Down) ? 1.0f : 0.0f;
    float leftStrength = mts::Input::IsKeyDown(mts::Key::Left) ? 1.0f : 0.0f;
    float rightStrength = mts::Input::IsKeyDown(mts::Key::Right) ? 1.0f : 0.0f;
    glm::vec2 inputDir = {rightStrength - leftStrength, upStrength - downStrength};
    bool jumpPressed = mts::Input::IsKeyPressed(mts::Key::Space);
    bool dashPressed = mts::Input::IsKeyPressed(mts::Key::LeftShift);
    bool attackPressed = mts::Input::IsKeyPressed(mts::Key::Z);

    // Get current audio state for beat check
    mts::AudioManager& audio = mts::AudioManager::Get();
    bool inWindow = audio.IsInBeatWindow(mHitWindowEarlyEdge, mHitWindowLateEdge);

    // Reset dash count if on ground and dash cooldown is done
    if (isGrounded && dashTimer < 0.0f)
    {
        dashCount = mDashMax;
    }

    // Change sprite facing direction if able
    if (!isAttacking)
    {
        if (leftStrength > 0.0f)
        {
            sprite->SetFlipX(true);
        }
        if (rightStrength > 0.0f)
        {
            sprite->SetFlipX(false);
        }
    }

    // Jump check
    if (jumpPressed && isGrounded && !isAttacking)
    {
        if (!mJumpRhythmLock || (mJumpRhythmLock && inWindow))
        {
            if (!queuedJump)
            {
                queuedJump = [this, inWindow]()
                {
                    TriggerCameraVFX(true, inWindow);
                    Jump(inWindow);
                };
                LOG_INFO("Jumped {} beat", inWindow ? "on" : "off");
            }
        }
        else
        {
            LOG_INFO("Jumped off beat");
        }
    }

    // Dash check
    if (dashPressed && dashCount > 0 && !isAttacking)
    {
        if (!mDashRhythmLock || (mDashRhythmLock && inWindow))
        {
            if (!queuedDash)
            {
                queuedDash = [this, inputDir, inWindow]()
                {
                    TriggerCameraVFX(true, inWindow);
                    Dash(inputDir, inWindow);
                };
                LOG_INFO("Dashed {} beat", inWindow ? "on" : "off");
            }
        }
        else
        {
            LOG_INFO("Dashed off beat");
        }
    }
    if (dashTimer >= 0.0f)
        dashTimer -= dt;

    // Attack check
    if (attackPressed && !isAttacking)
    {
        if (!mAttackRhythmLock || (mAttackRhythmLock && inWindow))
        {
            Attack(inWindow);
            LOG_INFO("Attacked {} beat", inWindow ? "on" : "off");
        }
        else
        {
            LOG_INFO("Attacked off beat");
        }
    }
    if (attackTimer >= 0.0f)
        attackTimer -= dt;
    if (attackComboTimer >= 0.0f)
        attackComboTimer -= dt;

    // Spawn attack hitbox after set delay
    if (attackDelayTimer > 0.0f)
    {
        attackDelayTimer -= dt;
    }
    else if (attackDelayTimer > -1.0f)
    {
        queuedAttackSpawn();
        attackDelayTimer = -2.0f;
    }

    // Set animation states based on movement
    if (attackTimer < 0.0f && anim->IsDone())
    {
        isAttacking = false;
    }
    if (!isAttacking)
    {
        if (dashTimer > 0.0f)
            anim->ChangeState(mts::AnimationSystem::States::Jump);
        else if (!isGrounded && currVelocity.y > 0.1f)
            anim->ChangeState(mts::AnimationSystem::States::Jump);
        else if (!isGrounded && currVelocity.y < -0.1f)
            anim->ChangeState(mts::AnimationSystem::States::Falling);
        else if (std::abs(currVelocity.x) > 0.01f)
            anim->ChangeState(mts::AnimationSystem::States::Walking);
        else
            anim->ChangeState(mts::AnimationSystem::States::Idle);
    }

    // Possible animation/sprite/color change while dash is in cooldown
    if (dashTimer > 0.0f)
    {
        sprite->SetTintColor(glm::vec3(0.7f));
        rb->SetGravityScale(0.0f);
    }
    else
    {
        sprite->SetTintColor(glm::vec3(1.0f));
        rb->SetGravityScale(defaultGravityScale);
    }
}

void PlayerController::OnFixedUpdate(float dt)
{
    // Check all important held keys at start of loop
    // This cleans up the code and reduces function calls
    bool leftHeld = mts::Input::IsKeyDown(mts::Key::Left);
    bool rightHeld = mts::Input::IsKeyDown(mts::Key::Right);

    // Get facing direction
    float moveDir = 0.0f;
    if (leftHeld)
    {
        facingDir = LEFT;
        moveDir = -1.0f;
    }
    if (rightHeld)
    {
        facingDir = RIGHT;
        moveDir = 1.0f;
    }

    currVelocity = rb->GetVelocity();
    // Calculate horizontal velocity
    if (dashTimer > 0.0f)
    {
        currVelocity.x = MoveToward(currVelocity.x, moveDir * mMoveSpeed, mDashDeceleration * dt);
        // Only decelerate vertically if dashing vertically up
        if (currentDashDir.y > 0.1f)
        {
            currVelocity.y = MoveToward(currVelocity.y, 0.0f, mDashDeceleration * dt);
        }
    }
    else if (static_cast<int>(moveDir) != 0 && !isAttacking)
    {
        // Turning around
        if (currVelocity.x != 0.0f &&
            static_cast<int>(glm::sign(currVelocity.x)) != static_cast<int>(glm::sign(moveDir)))
            currVelocity.x = MoveToward(currVelocity.x, 0.0f, mTurnSpeed * dt);
        // Accelerating
        else if ((moveDir < 0.0f && (currVelocity.x > moveDir * mMoveSpeed)) ||
                 ((moveDir > 0.0f && (currVelocity.x < moveDir * mMoveSpeed))))
            currVelocity.x = MoveToward(currVelocity.x, moveDir * mMoveSpeed, mAcceleration * dt);
        // Decelerating
        else
            currVelocity.x = MoveToward(currVelocity.x, moveDir * mMoveSpeed, mDeceleration * dt);
    }
    else
        currVelocity.x = MoveToward(currVelocity.x, 0.0f, mFriction * dt);

    // Call queued jump call
    if (queuedJump)
    {
        queuedJump();
        queuedJump = nullptr;
    }

    // Call queued dash call
    if (queuedDash)
    {
        queuedDash();
        queuedDash = nullptr;
    }

    rb->SetVelocity(currVelocity);
}

void PlayerController::DeserializeFields(mts::JsonValue const& fields)
{
    if (fields.Has("moveSpeed"))
        mMoveSpeed = fields["moveSpeed"].GetFloat(mMoveSpeed);
    if (fields.Has("jumpSpeed"))
        mJumpSpeed = fields["jumpSpeed"].GetFloat(mJumpSpeed);
    if (fields.Has("dashSpeed"))
        mDashSpeed = fields["dashSpeed"].GetFloat(mDashSpeed);
    if (fields.Has("friction"))
        mFriction = fields["friction"].GetFloat(mFriction);
    if (fields.Has("acceleration"))
        mAcceleration = fields["acceleration"].GetFloat(mAcceleration);
    if (fields.Has("deceleration"))
        mDeceleration = fields["deceleration"].GetFloat(mDeceleration);
    if (fields.Has("turnSpeed"))
        mTurnSpeed = fields["turnSpeed"].GetFloat(mTurnSpeed);
    if (fields.Has("dashDeceleration"))
        mDashDeceleration = fields["dashDeceleration"].GetFloat(mDashDeceleration);
    if (fields.Has("dashCooldown"))
        mDashCooldown = fields["dashCooldown"].GetFloat(mDashCooldown);
    if (fields.Has("dashMax"))
        mDashMax = fields["dashMax"].GetInt(mDashMax);
    if (fields.Has("attackCooldown"))
        mAttackCooldown = fields["attackCooldown"].GetFloat(mAttackCooldown);
    if (fields.Has("attackComboMaxDelay"))
        mAttackComboMaxDelay = fields["attackComboMaxDelay"].GetFloat(mAttackComboMaxDelay);
    if (fields.Has("attackDelay"))
        mAttackDelay = fields["attackDelay"].GetFloat(mAttackDelay);
    if (fields.Has("attackLightDamage"))
        mAttackLightDamage = fields["attackLightDamage"].GetFloat(mAttackLightDamage);
    if (fields.Has("attackHeavyDamage"))
        mAttackHeavyDamage = fields["attackHeavyDamage"].GetFloat(mAttackHeavyDamage);
    if (fields.Has("attackHitboxOffsetX"))
        mAttackHitboxOffsetX = fields["attackHitboxOffsetX"].GetFloat(mAttackHitboxOffsetX);
    if (fields.Has("dashBeatBoost"))
        mDashBeatBoost = fields["dashBeatBoost"].GetFloat(mDashBeatBoost);
    if (fields.Has("jumpBeatBoost"))
        mJumpBeatBoost = fields["jumpBeatBoost"].GetFloat(mJumpBeatBoost);
    if (fields.Has("dashRhythmLock"))
        mDashRhythmLock = fields["dashRhythmLock"].GetBool(mDashRhythmLock);
    if (fields.Has("jumpRhythmLock"))
        mJumpRhythmLock = fields["jumpRhythmLock"].GetBool(mJumpRhythmLock);
    if (fields.Has("attackRhythmLock"))
        mAttackRhythmLock = fields["attackRhythmLock"].GetBool(mAttackRhythmLock);
    if (fields.Has("hitWindowEarlyEdge"))
        mHitWindowEarlyEdge = fields["hitWindowEarlyEdge"].GetFloat(mHitWindowEarlyEdge);
    if (fields.Has("hitWindowLateEdge"))
        mHitWindowLateEdge = fields["hitWindowLateEdge"].GetFloat(mHitWindowLateEdge);
    if (fields.Has("camZoomSpeed"))
        mCamZoomSpeed = fields["camZoomSpeed"].GetFloat(mCamZoomSpeed);
}

void PlayerController::SerializeFields(mts::JsonDocument& out) const
{
    out.WriteFloat("moveSpeed", mMoveSpeed);
    out.WriteFloat("jumpSpeed", mJumpSpeed);
    out.WriteFloat("dashSpeed", mDashSpeed);
    out.WriteFloat("friction", mFriction);
    out.WriteFloat("acceleration", mAcceleration);
    out.WriteFloat("deceleration", mDeceleration);
    out.WriteFloat("turnSpeed", mTurnSpeed);
    out.WriteFloat("dashDeceleration", mDashDeceleration);
    out.WriteFloat("dashCooldown", mDashCooldown);
    out.WriteInt("dashMax", mDashMax);
    out.WriteFloat("attackCooldown", mAttackCooldown);
    out.WriteFloat("attackComboMaxDelay", mAttackComboMaxDelay);
    out.WriteFloat("attackDelay", mAttackDelay);
    out.WriteFloat("attackLightDamage", mAttackLightDamage);
    out.WriteFloat("attackHeavyDamage", mAttackHeavyDamage);
    out.WriteFloat("attackHitboxOffsetX", mAttackHitboxOffsetX);
    out.WriteFloat("dashBeatBoost", mDashBeatBoost);
    out.WriteFloat("jumpBeatBoost", mJumpBeatBoost);
    out.WriteBool("dashRhythmLock", mDashRhythmLock);
    out.WriteBool("jumpRhythmLock", mJumpRhythmLock);
    out.WriteFloat("camZoomSpeed", mCamZoomSpeed);
}

void PlayerController::GetInspectorFields(std::vector<mts::ScriptField>& fields)
{
    fields.push_back({.name = "moveSpeed",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mMoveSpeed},
                      .step = 0.1f});
    fields.push_back({.name = "jumpSpeed",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mJumpSpeed},
                      .step = 0.1f});
    fields.push_back({.name = "dashSpeed",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mDashSpeed},
                      .step = 0.1f});
    fields.push_back({.name = "friction",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mFriction},
                      .step = 0.1f});
    fields.push_back({.name = "acceleration",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mAcceleration},
                      .step = 0.1f});
    fields.push_back({.name = "deceleration",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mDeceleration},
                      .step = 0.1f});
    fields.push_back({.name = "turnSpeed",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mTurnSpeed},
                      .step = 0.1f});
    fields.push_back({.name = "dashDeceleration",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mDashDeceleration},
                      .step = 0.1f});
    fields.push_back({.name = "dashCooldown",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mDashCooldown},
                      .step = 0.1f});
    fields.push_back({.name = "dashMax",
                      .type = mts::ScriptField::Type::Int,
                      .ptr = {.i = &mDashMax},
                      .step = 1});
    fields.push_back({.name = "attackCooldown",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mAttackCooldown},
                      .step = 0.1f});
    fields.push_back({.name = "attackComboMaxDelay",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mAttackComboMaxDelay},
                      .step = 0.1f});
    fields.push_back({.name = "attackDelay",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mAttackDelay},
                      .step = 0.1f});
    fields.push_back({.name = "attackLightDamage",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mAttackLightDamage},
                      .step = 0.1f});
    fields.push_back({.name = "attackHeavyDamage",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mAttackHeavyDamage},
                      .step = 0.1f});
    fields.push_back({.name = "attackHitboxOffsetX",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mAttackHitboxOffsetX},
                      .step = 0.1f});
    fields.push_back({.name = "dashBeatBoost",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mDashBeatBoost},
                      .step = 0.1f});
    fields.push_back({.name = "jumpBeatBoost",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mJumpBeatBoost},
                      .step = 0.1f});
    fields.push_back({.name = "dashRhythmLock",
                      .type = mts::ScriptField::Type::Bool,
                      .ptr = {.b = &mDashRhythmLock}});
    fields.push_back({.name = "jumpRhythmLock",
                      .type = mts::ScriptField::Type::Bool,
                      .ptr = {.b = &mJumpRhythmLock}});
    fields.push_back({.name = "hitWindowEarlyEdge",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mHitWindowEarlyEdge},
                      .step = 0.01f});
    fields.push_back({.name = "hitWindowLateEdge",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mHitWindowLateEdge},
                      .step = 0.01f});
    fields.push_back({.name = "camZoomSpeed",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mCamZoomSpeed},
                      .step = 0.1f});
}

void PlayerController::OnCollisionEnter2D(const mts::Collision2D& collision)
{
    if (collision.other->GetGameObject()->GetLayer() == mts::GameObject::Ground)
    {
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
        if (abs(glm::dot(glm::vec2(0.0f, -1.0f), collision.normal) + 1.0f) < 0.1f)
        {
            isGrounded = false;
        }
    }
}

void PlayerController::Dash(glm::vec2 inputDir, bool inWindow)
{
    glm::vec2 dashDir{0.0f, 0.0f};
    // If not holding a direction, use direction character is facing (left or right)
    if (inputDir == glm::vec2(0))
        facingDir == LEFT ? dashDir = {-1.0f, 0.0f} : dashDir = {1.0f, 0.0f};
    else
        dashDir = inputDir;

    // Multiply dash speed if hit on beat
    float adjustedDashSpeed = inWindow ? mDashSpeed * mDashBeatBoost : mDashSpeed;

    currVelocity = adjustedDashSpeed * dashDir;
    --dashCount;
    dashTimer = mDashCooldown;
    currentDashDir = dashDir;
}

void PlayerController::Jump(bool inWindow)
{
    isGrounded = false;

    // multiply jump speed if hit on beat
    float adjustedJumpSpeed = inWindow ? mJumpSpeed * mJumpBeatBoost : mJumpSpeed;
    currVelocity.y = adjustedJumpSpeed;
    // Reset dash cooldown on jump
    dashTimer = -1.0f;
    dashCount = mDashMax;
}

void PlayerController::Attack(bool inWindow)
{
    // Check if in time range to combo attack
    // Also can only combo if on beat
    if (attackComboTimer > 0.0f && inWindow)
    {
        queuedAttackSpawn = [this]() { SpawnAttackHitbox(mAttackHeavyDamage); };
        anim->ChangeState(mts::AnimationSystem::States::Attack2);
        attackComboTimer = -1.0f;
    }
    else
    {
        queuedAttackSpawn = [this]() { SpawnAttackHitbox(mAttackLightDamage); };
        anim->ChangeState(mts::AnimationSystem::States::Attack1);
        attackComboTimer = mAttackComboMaxDelay;
    }

    isAttacking = true;
    attackDelayTimer = mAttackDelay;
    attackTimer = mAttackCooldown;
}

void PlayerController::SpawnAttackHitbox(float damage)
{
    // Create attack hitbox object
    mts::GameObject* attackHitbox = mts::GameObjectFactory::CreateGameObject();
    mts::SerializationUtility::DeserializeFromFile(attackHitbox, "Data/AttackHitbox.prefab");
    // Get transform and add or subtract offset based on facing dir
    mts::Transform* attackHitboxTransform = attackHitbox->GetComponent<mts::Transform>();
    glm::vec2 hitBoxPos = transform->GetPosition();
    if (facingDir == LEFT)
        hitBoxPos.x -= mAttackHitboxOffsetX;
    else if (facingDir == RIGHT)
        hitBoxPos.x += mAttackHitboxOffsetX;
    attackHitboxTransform->SetPosition(hitBoxPos);
    // Set damage of hitbox
    auto hitboxScript = attackHitbox->GetScript<AttackHitboxScript>();
    if (hitboxScript)
        hitboxScript->SetDamage(damage);
}

void PlayerController::Test()
{
    // Generate code for testing the MovablePlatform's FirePlatformTranslate function
    // Press the key T to fire the test
    if (mts::Input::IsKeyPressed(mts::Key::T))
    {
        LOG_INFO("PlayerController Test: Firing MovablePlatform FirePlatformTranslate");
        // Get all game objects in the current scene
        auto* currentScene = mts::SceneSystem::GetCurrentScene();

        if (currentScene)
        {
            // Find all MovablePlatform scripts and trigger them
            mts::GameObject* testPlatform = currentScene->FindGameObjectByName("TestPlatform");
            auto* platform = testPlatform->GetScript<MovablePlatform>();
            auto* platformTransform = testPlatform->GetComponent<mts::Transform>();
            if (!platformTransform)
            {
                LOG_ERROR("Transform component not found on TestPlatform");
                return;
            }
            if (!platform)
            {
                LOG_ERROR("MovablePlatform script not found on TestPlatform");
                return;
            }
            if (platform)
            {
                platform->FirePlatformTranslate(
                    glm::vec3{
                        platformTransform->GetPosition().x,
                        platformTransform->GetPosition().y + 3.0f,
                        platformTransform->GetPosition().z,
                    },
                    1.0f);
            }
        }
    }
}

void PlayerController::SetCheckPoint(CheckPoint* checkPoint)
{
    LOG_INFO("CheckPoint set at position: ({}, {}, {})",
             checkPoint->GetCheckPointPosition().x,
             checkPoint->GetCheckPointPosition().y,
             checkPoint->GetCheckPointPosition().z);
    activatedCheckPoint = checkPoint;
}

void PlayerController::ReturnToCheckPoint()
{
    if (activatedCheckPoint)
    {
        transform->SetPosition(activatedCheckPoint->GetCheckPointPosition());
        rb->SetVelocity({0.0f, 0.0f});
    }
}

void PlayerController::TriggerCameraVFX(bool isAction, bool inWindow)
{
    // Determine zoom strength based on action and timing
    // Lower zoom value = stronger zoom in effect
    float zoomStrength = 1.0f;
    // float speed = 0.5f; // Quick zoom duration

    if (isAction && inWindow)
    {
        // Strongest: Action performed on beat
        zoomStrength = 0.98f;
    }
    else if (isAction && !inWindow)
    {
        // Medium-strong: Action performed off beat
        // zoomStrength = 0.985f;
    }
    else if (!isAction && inWindow)
    {
        // Medium: Beat indicator (no action)
        // zoomStrength = 0.99f;
    }
    else
    {
        // Weakest: Off-beat, no action
        // zoomStrength = 0.98f;
    }

    // Apply zoom effect
    cam->TriggerZoom(zoomStrength, mCamZoomSpeed);
    // Reset to normal zoom
    cam->TriggerZoom(1.0f, mCamZoomSpeed * 2);
}

void PlayerController::OnBeat(mts::BeatInfo info)
{
    // TriggerCameraVFX(false, true);
}

void PlayerController::OnBar(mts::BeatInfo info)
{
}

void PlayerController::Init(mts::ConductorSystem& system, CameraController* cam)
{
    mBeatSubId = system.onBeat.Subscribe([this](mts::BeatInfo info) { OnBeat(info); });
    mBarSubId = system.onBar.Subscribe([this](mts::BeatInfo info) { OnBar(info); });
    this->cam = cam;
}
