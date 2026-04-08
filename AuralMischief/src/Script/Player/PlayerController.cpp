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
#include "Engine/Resource/Texture.h"
#include "Engine/EntitySystem/Component/UITransform.h"
#include "Engine/Audio/AudioManager.h"
#include "Engine/Input/Input.h"
#include "Engine/Conductor/ConductorSystem.h"
#include <glm/vec2.hpp>

#include "../MovablePlatform.h"
#include "../CheckPoint.h"
#include "../CameraController.h"
#include "../AttackHitboxScript.h"
#include "../DamageNumberSpawner.h"
#include "../EnemyBehaviors/BulletBehavior.h"
#include "../EnemyBehaviors/EnemyController.h"
#include "Engine/Helpers/Random.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/Scripting/ScriptFactory.h"
#include "Engine/Serialization/SerializationUtility.h"

void PlayerController::Init()
{
    mts::ScriptFactory::RegisterScript("AttackHitboxScript",
                                       []() { return new AttackHitboxScript(); });
    rb = GetGameObject()->GetComponent<mts::RigidBody2D>();
    anim = GetGameObject()->GetComponent<mts::AnimationSystem>();
    sprite = GetGameObject()->GetComponent<mts::SpriteRenderer>();
    transform = GetGameObject()->GetComponent<mts::Transform>();

    particleController = PlayerParticleController();
    mSpawner = GetGameObject()->GetScript<DamageNumberSpawner>();

    defaultGravityScale = rb->GetGravityScale();

    InitHP();
}

void PlayerController::Awake()
{
    mBeatSubId =
        mts::ConductorSystem::Get().onBeat.Subscribe([this](mts::BeatInfo info) { OnBeat(info); });
    mBarSubId =
        mts::ConductorSystem::Get().onBar.Subscribe([this](mts::BeatInfo info) { OnBar(info); });
}

void PlayerController::Start()
{
    if (mCameraUUID.IsValid())
    {
        auto* scene = mts::SceneSystem::GetCurrentScene();
        if (scene)
        {
            mCameraGO = scene->FindGameObjectByUUID(mCameraUUID);
            if (mCameraGO)
                cam = mCameraGO->GetScript<CameraController>();
        }
    }
}

void PlayerController::OnDisable()
{
    if (mBeatSubId)
    {
        mts::ConductorSystem::Get().onBeat.Unsubscribe(mBeatSubId);
        mBeatSubId = 0;
    }
    if (mBarSubId)
    {
        mts::ConductorSystem::Get().onBar.Unsubscribe(mBarSubId);
        mBarSubId = 0;
    }
}

void PlayerController::OnUpdate(float dt)
{
    // Check all important held keys at start of loop
    // This cleans up the code and reduces function calls
    float upStrength =
        mts::Input::IsKeyDown(mts::Key::Up) || mts::Input::IsKeyDown(mts::Key::W) ? 1.0f : 0.0f;
    float downStrength =
        mts::Input::IsKeyDown(mts::Key::Down) || mts::Input::IsKeyDown(mts::Key::S) ? 1.0f : 0.0f;
    float leftStrength =
        mts::Input::IsKeyDown(mts::Key::Left) || mts::Input::IsKeyDown(mts::Key::A) ? 1.0f : 0.0f;
    float rightStrength =
        mts::Input::IsKeyDown(mts::Key::Right) || mts::Input::IsKeyDown(mts::Key::D) ? 1.0f : 0.0f;
    glm::vec2 inputDir = {rightStrength - leftStrength, upStrength - downStrength};
    bool jumpPressed = mts::Input::IsKeyPressed(mts::Key::Space);
    bool dashPressed =
        mts::Input::IsKeyPressed(mts::Key::LeftShift) || mts::Input::IsKeyPressed(mts::Key::K);
    bool attackPressed =
        mts::Input::IsKeyPressed(mts::Key::Z) || mts::Input::IsKeyPressed(mts::Key::J);
    bool attackReleased =
        mts::Input::IsKeyReleased(mts::Key::Z) || mts::Input::IsKeyReleased(mts::Key::J);

    // Get current audio state for beat check
    mts::AudioManager& audio = mts::AudioManager::Get();
    bool inWindow = audio.IsInBeatWindow(mHitWindowEarlyEdge, mHitWindowLateEdge);

    if (!mPlayerAudio)
        mPlayerAudio = GetGameObject()->GetScript<PlayerAudioController>();

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
                    // TriggerCameraVFX(true, inWindow);
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
                    // TriggerCameraVFX(true, inWindow);
                    Dash(inputDir, inWindow, 1.0f);
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

    if (attackPressed)
    {
        attackHeldLength = 0.0f;
    }
    else if (attackReleased)
    {
        if (attackHeldLength < 0.35f)
        {
            if (!isAttacking)
            {
                Attack(inWindow, false);
            }
        }
        else if (!isAttacking)
        {
            Attack(inWindow, true);
        }
        isCharging = false;
        chargeComplete = false;
        numBeatsCharged = 1;
        attackHeldLength = -1.0f;
    }

    if (attackHeldLength > 0.35f && !isCharging)
    {
        isCharging = true;
    }
    if (numBeatsCharged >= 4 && !chargeComplete)
    {
        chargeComplete = true;
    }

    if (attackHeldLength > -1.0f && attackTimer < 0.0f)
        attackHeldLength += dt;
    if (attackTimer >= 0.0f)
        attackTimer -= dt;
    if (iFramesTimer >= 0.0f)
        iFramesTimer -= dt;
    timeSinceLastBeat += dt;

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

    // Update attack hitbox position to follow player
    if (attackHitboxTransform)
    {
        glm::vec2 hitBoxPos = transform->GetPosition();
        if (attackHitboxDirection == LEFT)
            hitBoxPos.x -= mAttackHitboxOffsetX;
        else if (attackHitboxDirection == RIGHT)
            hitBoxPos.x += mAttackHitboxOffsetX;
        attackHitboxTransform->SetPosition(hitBoxPos);
    }

    // Set animation states based on movement
    if (attackTimer < 0.0f || anim->IsDone())
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
        if (dashTimer < mDashCooldown - 0.1f)  // Resume
        {
            rb->SetGravityScale(defaultGravityScale);
        }
        else
        {
            rb->SetGravityScale(0.0f);
        }
    }
    else if (dashTimer > -1.0f)
    {
        sprite->SetTintColor(glm::vec3(1.0f));
        rb->SetGravityScale(defaultGravityScale);
        dashTimer = -1.0f;
    }

    chargeFlashTimer -= dt;
    if (chargeFlashTimer < 0.0f && chargeFlashTimer > -1.0f)
    {
        // sprite->SetTintColor(glm::vec3(1.0f));
        if (defaultTexture && sprite)
            sprite->SetTexture(defaultTexture);
        chargeFlashTimer = -1.0f;
    }

    if (isCharging)
    {
        particleController.UpdateParticlePositions(transform->GetPosition());
        particleController.StartParticleEmitter(PlayerParticleController::CHARGE);
    }
    else
    {
        particleController.StopParticleEmitter(PlayerParticleController::CHARGE);
    }
    // particleController.UpdateParticlePositions(transform->GetPosition());
}

void PlayerController::OnFixedUpdate(float dt)
{
    // Check all important held keys at start of loop
    // This cleans up the code and reduces function calls
    bool leftHeld = mts::Input::IsKeyDown(mts::Key::Left) || mts::Input::IsKeyDown(mts::Key::A);
    bool rightHeld = mts::Input::IsKeyDown(mts::Key::Right) || mts::Input::IsKeyDown(mts::Key::D);

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
    if (fields.Has("attackDelay"))
        mAttackDelay = fields["attackDelay"].GetFloat(mAttackDelay);
    if (fields.Has("offBeatLightDamage"))
        mOffBeatLightDamage = fields["offBeatLightDamage"].GetFloat(mOffBeatLightDamage);
    if (fields.Has("onBeatLightDamage"))
        mOnBeatLightDamage = fields["onBeatLightDamage"].GetFloat(mOnBeatLightDamage);
    if (fields.Has("offBeatHeavyDamage"))
        mOffBeatHeavyDamage = fields["offBeatHeavyDamage"].GetFloat(mOffBeatHeavyDamage);
    if (fields.Has("onBeatHeavyDamage"))
        mOnBeatHeavyDamage = fields["onBeatHeavyDamage"].GetFloat(mOnBeatHeavyDamage);
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
    if (fields.Has("defaultTextureResource"))
    {
        mDefaultTextureResource =
            fields["defaultTextureResource"].GetString(mDefaultTextureResource);
        defaultTexture = mts::ResourceManager::GetTexture(mDefaultTextureResource);
    }
    if (fields.Has("altTextureResource"))
    {
        mAltTextureResource = fields["altTextureResource"].GetString(mAltTextureResource);
        altTexture = mts::ResourceManager::GetTexture(mAltTextureResource);
    }
    if (fields.Has("cameraUUID"))
        mCameraUUID = mts::UUID::FromString(fields["cameraUUID"].GetString());
    if (fields.Has("maxHP"))
        mMaxHP = fields["maxHP"].GetInt(mMaxHP);
    if (fields.Has("fullHeartFrame"))
        mFullHeartFrame = fields["fullHeartFrame"].GetInt(mFullHeartFrame);
    if (fields.Has("emptyHeartFrame"))
        mEmptyHeartFrame = fields["emptyHeartFrame"].GetInt(mEmptyHeartFrame);
    if (fields.Has("hpStartX"))
        mHPStartX = fields["hpStartX"].GetFloat(mHPStartX);
    if (fields.Has("hpStartY"))
        mHPStartY = fields["hpStartY"].GetFloat(mHPStartY);
    if (fields.Has("hpSpacing"))
        mHPSpacing = fields["hpSpacing"].GetFloat(mHPSpacing);
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
    out.WriteFloat("attackDelay", mAttackDelay);
    out.WriteFloat("offBeatLightDamage", mOffBeatLightDamage);
    out.WriteFloat("onBeatLightDamage", mOnBeatLightDamage);
    out.WriteFloat("offBeatHeavyDamage", mOffBeatHeavyDamage);
    out.WriteFloat("onBeatHeavyDamage", mOnBeatHeavyDamage);
    out.WriteFloat("attackHitboxOffsetX", mAttackHitboxOffsetX);
    out.WriteFloat("dashBeatBoost", mDashBeatBoost);
    out.WriteFloat("jumpBeatBoost", mJumpBeatBoost);
    out.WriteBool("dashRhythmLock", mDashRhythmLock);
    out.WriteBool("jumpRhythmLock", mJumpRhythmLock);
    out.WriteFloat("camZoomSpeed", mCamZoomSpeed);
    out.WriteString("defaultTextureResource", mDefaultTextureResource);
    out.WriteString("altTextureResource", mAltTextureResource);
    out.WriteInt("maxHP", mMaxHP);
    out.WriteInt("fullHeartFrame", mFullHeartFrame);
    out.WriteInt("emptyHeartFrame", mEmptyHeartFrame);
    out.WriteFloat("hpStartX", mHPStartX);
    out.WriteFloat("hpStartY", mHPStartY);
    out.WriteFloat("hpSpacing", mHPSpacing);
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
    fields.push_back({.name = "attackDelay",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mAttackDelay},
                      .step = 0.1f});
    fields.push_back({.name = "offBeatLightDamage",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mOffBeatLightDamage},
                      .step = 0.1f});
    fields.push_back({.name = "onBeatLightDamage",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mOnBeatLightDamage},
                      .step = 0.1f});
    fields.push_back({.name = "offBeatHeavyDamage",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mOffBeatHeavyDamage},
                      .step = 0.1f});
    fields.push_back({.name = "onBeatHeavyDamage",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mOnBeatHeavyDamage},
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
    fields.push_back(
        {.name = "maxHP", .type = mts::ScriptField::Type::Int, .ptr = {.i = &mMaxHP}, .step = 1});
    fields.push_back({.name = "fullHeartFrame",
                      .type = mts::ScriptField::Type::Int,
                      .ptr = {.i = &mFullHeartFrame},
                      .step = 1});
    fields.push_back({.name = "emptyHeartFrame",
                      .type = mts::ScriptField::Type::Int,
                      .ptr = {.i = &mEmptyHeartFrame},
                      .step = 1});
    fields.push_back({.name = "hpStartX",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mHPStartX},
                      .step = 0.01f});
    fields.push_back({.name = "hpStartY",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mHPStartY},
                      .step = 0.01f});
    fields.push_back({.name = "hpSpacing",
                      .type = mts::ScriptField::Type::Float,
                      .ptr = {.f = &mHPSpacing},
                      .step = 0.01f});
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
    auto a = collision.other->GetGameObject()->GetScript<AttackHitboxScript>();
    if ((collision.other->GetGameObject()->HasTag("Enemy") || (a && a->GetTeam() == AttackHitboxScript::Team::Enemy)) && iFramesTimer < 0.0f)
    {
        auto o = collision.other->GetGameObject();
        if (o->GetScript<mts::BulletBehavior>())
        {
            if (collision.other->GetGameObject()->GetScript<mts::BulletBehavior>()->GetIsDying())
            {
                return;
            }
        }
        if (o->GetScript<mts::EnemyController>())
        {
            if (o->GetScript<mts::EnemyController>()->IsDying() && o->GetScript<mts::EnemyController>()->GetEnemyType() != mts::EnemyType::CrawlEnemy)
            {
                LOG_INFO("Collided with dead enemy that isng slug");
                return;
            }
        }
        LOG_INFO("hit by enemy with normal {}, {}", collision.normal.x, collision.normal.y);
        iFramesTimer = iFrames;
        glm::vec2 hitDir = a ? a->GetDirection() : -collision.normal;
        queuedDash = [this, hitDir]() { Dash(hitDir, true, enemyKnockback); };
        if (a)
        {
            TakeDamage(static_cast<int>(a->GetDamage()));
            return;
        }
        TakeDamage(1);
    }
}

void PlayerController::OnCollisionExit2D(const mts::Collision2D& collision)
{
    if (!collision.other->GetGameObject())
    {
        return;
    }
    if (collision.other->GetGameObject()->GetLayer() == mts::GameObject::Ground)
    {
        if (abs(glm::dot(glm::vec2(0.0f, -1.0f), collision.normal) + 1.0f) < 0.1f)
        {
            isGrounded = false;
        }
    }
}

void PlayerController::Dash(glm::vec2 inputDir, bool inWindow, float speedScale)
{
    glm::vec2 dashDir{0.0f, 0.0f};
    // If not holding a direction, use direction character is facing (left or right)
    if (inputDir == glm::vec2(0))
        // if (inputDir.x == 0)
        facingDir == LEFT ? dashDir = {-1.0f, 0.0f} : dashDir = {1.0f, 0.0f};
    else
        dashDir = inputDir;

    dashDir = glm::vec2(dashDir.x, dashDir.y / 1.5f);

    // Multiply dash speed if hit on beat
    float adjustedDashSpeed = inWindow ? mDashSpeed * mDashBeatBoost : mDashSpeed;

    currVelocity = adjustedDashSpeed * speedScale * dashDir;
    --dashCount;
    dashTimer = mDashCooldown;
    currentDashDir = dashDir;
    if (mPlayerAudio)
        mPlayerAudio->PlayOneShotSound(PlayerAudioController::DASH);
}

void PlayerController::Jump(bool inWindow)
{
    isGrounded = false;

    // multiply jump speed if hit on beat
    float adjustedJumpSpeed = inWindow ? mJumpSpeed * mJumpBeatBoost : mJumpSpeed;
    currVelocity.y = adjustedJumpSpeed;

    // If in dash state, increase horizontal velocity and decrease jump height
    if (dashTimer > 0.0f)
    {
        currVelocity.y *= 0.8f;
        if (currentDashDir.x < 0.0f)
            currVelocity.x -= mDashSpeed * 0.2f;
        else
            currVelocity.x += mDashSpeed * 0.2f;
    }
    dashCount = mDashMax;
    if (mPlayerAudio)
        mPlayerAudio->PlayOneShotSound(PlayerAudioController::EVADE);
}

void PlayerController::Attack(bool inWindow, bool isChargeAttack)
{
    PlayerParticleController::ParticleTiming timing = PlayerParticleController::EARLY;
    if (inWindow)
        timing = PlayerParticleController::PERFECT;
    else if (mts::AudioManager::Get().IsInBeatWindow(mHitWindowLateEdge, 0.25f))
        timing = PlayerParticleController::LATE;

    if (isChargeAttack)
    {
        // On beat charge attack
        if (inWindow && numBeatsCharged >= 4)
        {
            queuedAttackSpawn = [this, timing]()
            {
                SpawnAttackHitbox(mOnBeatHeavyDamage,
                                  3.0f,
                                  2.5f,
                                  PlayerParticleController::ON_BEAT_HEAVY, timing);
            };
            LOG_INFO("Heavy attacked on beat, held for {} beats, dealt {} damage",
                     numBeatsCharged,
                     mOnBeatHeavyDamage);
            if (mPlayerAudio)
                mPlayerAudio->PlayOneShotSound(
                    static_cast<PlayerAudioController::SFX>(mts::Random::Range(0, 4)));
        }
        // Off beat charge attack
        else
        {
            queuedAttackSpawn = [this, timing]()
            {
                SpawnAttackHitbox(mOffBeatHeavyDamage,
                                  3.0f,
                                  2.5f,
                                  PlayerParticleController::OFF_BEAT_HEAVY, timing);
            };
            LOG_INFO("Heavy attacked {} beat, held for {} beats, dealt {} damage",
                     inWindow ? "on" : "off",
                     numBeatsCharged,
                     mOffBeatHeavyDamage);
            if (mPlayerAudio)
                mPlayerAudio->PlayOneShotSound(
                    static_cast<PlayerAudioController::SFX>(mts::Random::Range(0, 4)));
        }
        anim->ChangeState(mts::AnimationSystem::States::Attack2);

        // queuedDash = [this]()
        //{
        //     //TriggerCameraVFX(true, true);
        //     Dash(facingDir == LEFT ? glm::vec2(-1.0f, 0.0f) : glm::vec2(1.0f, 0.0f), true, 1.2f);
        // };
    }
    else
    {
        // On beat light attack
        if (inWindow)
        {
            queuedAttackSpawn = [this, timing]()
            {
                SpawnAttackHitbox(mOnBeatLightDamage,
                                  3.0f,
                                  1.2f,
                                  PlayerParticleController::ON_BEAT_LIGHT, timing);
            };
            if (mPlayerAudio)
                mPlayerAudio->PlayOneShotSound(
                    static_cast<PlayerAudioController::SFX>(mts::Random::Range(0, 4)));
        }
        else
        {
            queuedAttackSpawn = [this, timing]()
            {
                SpawnAttackHitbox(mOffBeatLightDamage,
                                  3.0f,
                                  1.2f,
                                  PlayerParticleController::OFF_BEAT_LIGHT, timing);
            };
            if (mPlayerAudio)
                mPlayerAudio->PlayOneShotSound(
                    static_cast<PlayerAudioController::SFX>(mts::Random::Range(0, 4)));
        }
        LOG_INFO("Light attacked {} beat", inWindow ? "on" : "off");

        if (attackAnimType)
            anim->ChangeState(mts::AnimationSystem::States::Attack1);
        else
            // change this if we want to alternate attack animations
            anim->ChangeState(mts::AnimationSystem::States::Attack1);
        attackAnimType = !attackAnimType;
    }

    isAttacking = true;
    attackDelayTimer = mAttackDelay;
    attackTimer = mAttackCooldown;
}

void PlayerController::SpawnAttackHitbox(float damage,
                                         float scaleX,
                                         float scaleY,
                                         PlayerParticleController::ParticleType type,
                                         PlayerParticleController::ParticleTiming timing)
{
    if (!attackHitbox)
    {
        attackHitbox = mts::GameObjectFactory::CreateFromArchetype("Data/AttackHitbox.prefab");
        attackHitboxScript = attackHitbox->GetScript<AttackHitboxScript>();
    }
    // Create attack hitbox object
    attackHitbox->SetActive(true);
    // Get transform and add or subtract offset based on facing dir
    attackHitboxTransform = attackHitbox->GetComponent<mts::Transform>();
    if (!attackHitboxTransform)
        return;
    glm::vec2 hitBoxPos = transform->GetPosition();
    glm::vec2 particlePos = transform->GetPosition();
    if (facingDir == LEFT)
    {
        hitBoxPos.x -= mAttackHitboxOffsetX;
        particlePos.x -= scaleX * mAttackHitboxOffsetX;
        attackHitboxDirection = LEFT;
    }
    else if (facingDir == RIGHT)
    {
        hitBoxPos.x += mAttackHitboxOffsetX;
        particlePos.x += scaleX * mAttackHitboxOffsetX;
        attackHitboxDirection = RIGHT;
    }
    attackHitboxTransform->SetPosition(hitBoxPos);
    attackHitboxTransform->SetScale(glm::vec3(scaleX, scaleY, 0.0f));
    // Set damage of hitbox
    attackHitboxScript->SetDamage(damage);

    particleController.SpawnParticleEmitter(
        type,
        particlePos,
        timing,
        glm::vec2(transform->GetPosition().x, transform->GetPosition().y + 0.5f));
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
    if (!cam)
        return;
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
    if (info.isHalfBeat == false && attackHeldLength > timeSinceLastBeat)
    {
        numBeatsCharged++;
        chargeFlashTimer = 0.15f;
        //        if (sprite)
        //            sprite->SetTintColor(glm::vec3(1.5f));
        if (altTexture && sprite)
            sprite->SetTexture(altTexture);
    }
    timeSinceLastBeat = 0.0f;
}

void PlayerController::OnBar(mts::BeatInfo info)
{
}

void PlayerController::RefreshHeartVisuals()
{
    for (int i = 0; i < mMaxHP && i < static_cast<int>(mHearts.size()); ++i)
    {
        LOG_INFO("Refreshing heart {}: current HP {}, max HP {}", i, mCurrentHP, mMaxHP);
        auto* sr = mHearts[i]->GetComponent<mts::SpriteRenderer>();
        if (sr)
        {
            sr->SetFrame(i < mCurrentHP ? mFullHeartFrame : mEmptyHeartFrame);
        }
    }
}

void PlayerController::TakeDamage(int damage)
{
    if (mCurrentHP <= 0)
        return;

    mCurrentHP -= damage;
    if (mCurrentHP < 0)
        mCurrentHP = 0;

    LOG_INFO("[PlayerController] TakeDamage: {} damage, HP now {}/{}", damage, mCurrentHP, mMaxHP);
    RefreshHeartVisuals();

    if (mCurrentHP <= 0)
    {
        LOG_INFO("[PlayerController] Player died!");
        mts::SceneSystem::SwitchScene(3);
    }
}

void PlayerController::Heal(int amount)
{
    if (mCurrentHP >= mMaxHP)
        return;
    mCurrentHP += amount;
    if (mCurrentHP > mMaxHP)
        mCurrentHP = mMaxHP;
    LOG_INFO("[PlayerController] Heal: {} HP, HP now {}/{}", amount, mCurrentHP, mMaxHP);
    RefreshHeartVisuals();
}

void PlayerController::OnEnemyHit(float damage, glm::vec3 pos)
{
    if (mSpawner)
        mSpawner->Spawn(damage, pos);
}

void PlayerController::InitHP()
{
    // HP system initialization
    mCurrentHP = mMaxHP;

    // Collect existing "HP" GameObjects from the scene
    mHearts.clear();
    mts::Scene* scene = mts::SceneSystem::GetCurrentScene();
    for (auto* go : scene->GetGameObjects())
    {
        if (go->GetName() == "HP")
            mHearts.push_back(go);
    }

    // Spawn additional hearts from prefab if needed
    while (static_cast<int>(mHearts.size()) < mMaxHP)
    {
        mts::GameObject* heart = mts::GameObjectFactory::CreateFromArchetype("Data/HP.prefab");
        mHearts.push_back(heart);
    }

    // Deactivate surplus hearts
    for (int i = mMaxHP; i < static_cast<int>(mHearts.size()); ++i)
        mHearts[i]->SetActive(false);

    // Position all active hearts and set to full
    for (int i = 0; i < mMaxHP; ++i)
    {
        mHearts[i]->SetActive(true);
        if (auto* uiTr = mHearts[i]->GetComponent<mts::UITransform>())
            uiTr->SetPosition(glm::vec3(mHPStartX + i * mHPSpacing, mHPStartY, 0.0f));
        if (auto* sr = mHearts[i]->GetComponent<mts::SpriteRenderer>())
            sr->SetFrame(mFullHeartFrame);
    }
}
