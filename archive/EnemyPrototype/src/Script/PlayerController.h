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

class CheckPoint;

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

    enum PlayerState
    {
        DEFAULT = 0,
        IDLE,
        JUMP,
        FALL,
        ATTACK_1,
        ATTACK_2,
        DASH,
        WALK
    };

   public:
    void OnCreate() override;
    void OnUpdate(float dt) override;
    void OnFixedUpdate(float dt) override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

    void OnCollisionEnter2D(const mts::Collision2D& collision) override;
    void OnCollisionStay2D(const mts::Collision2D& collision) override;
    void OnCollisionExit2D(const mts::Collision2D& collision) override;

    void SetHpBar(mts::GameObject* bar) { hpBar = bar; }

    static float MoveToward(float val_from, float val_to, float delta)
    {
        return glm::abs(val_to - val_from) <= delta
                   ? val_to
                   : val_from + glm::sign(val_to - val_from) * delta;
    }

    //void Test();
    //void SetCheckPoint(CheckPoint*);
    //void ReturnToCheckPoint();

    SCRIPT_CLASS(PlayerController)

   private:
    void Dash(glm::vec2 inputDir, bool inWindow);
    void Jump(bool inWindow);
    void Attack(bool inWindow);
    void SpawnAttackHitbox(float damage);

    glm::vec2 currVelocity{0.0f, 0.0f};
    std::function<void()> queuedDash = nullptr;
    std::function<void()> queuedJump = nullptr;
    std::function<void()> queuedAttackSpawn = nullptr;

    mts::RigidBody2D* rb = nullptr;
    mts::AnimationSystem* anim = nullptr;
    mts::SpriteRenderer* sprite = nullptr;
    mts::Transform* transform = nullptr;

    // Adjustable parameters
    float mMoveSpeed = 5.0f;
    float mJumpSpeed = 12.0f;
    float mDashSpeed = 12.0f;
    float mFriction = 20.0f;
    float mAcceleration = 40.0f;
    float mDeceleration = 15.0f;
    float mDashDampening = 0.8f;
    float mTurnSpeed = 100.0f;
    int mDashMax = 1;
    float mDashCooldown = 0.2f;
    float mAttackCooldown = 0.5f;
    float mAttackComboMaxDelay = 3.0f;
    // Attack delay will delay the spawned attack hitbox to allow it to line up with animation
    float mAttackDelay = 0.0f;
    float mAttackLightDamage = 1.0f;
    float mAttackHeavyDamage = 2.0f;
    float mAttackHitboxOffsetX = 3.0f;
    // beat boosts are multipliers to the strength of actions
    // these are applied if the player hits the action on beat
    float mDashBeatBoost = 1.0f;
    float mJumpBeatBoost = 1.0f;
    float mHitWindowEarlyEdge = 0.1f;
    float mHitWindowLateEdge = 0.1f;
    // rhythm lock means the player can only jump/dash if hitting on beat
    bool mDashRhythmLock = false;
    bool mJumpRhythmLock = false;
    bool mAttackRhythmLock = false;

    // State variables
    bool isGrounded = false;
    int dashCount = 1;
    float dashTimer = -1.0f;
    float attackTimer = -1.0f;
    float attackComboTimer = -1.0f;
    float attackDelayTimer = -2.0f;
    bool isAttacking = false;
    int maxHp = 5;
    int curHp;
    mts::GameObject* hpBar = nullptr;
    Direction facingDir = RIGHT;

    // CheckPoint Properties
    CheckPoint* activatedCheckPoint = nullptr;
};