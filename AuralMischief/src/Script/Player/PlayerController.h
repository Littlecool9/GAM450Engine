/**
 * @file PlayerController.h
 * @author josh.siegel
 * @brief // Player movement/animation script
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "PlayerAudioController.h"
#include "PlayerParticleController.h"
#include "Engine/EntitySystem/Component/AnimationSystem.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Rigidbody2D.h"
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/Conductor/ConductorSystem.h"
#include "Engine/UUID/UUID.h"

#include <glm/common.hpp>
#include <vector>

class AttackHitboxScript;
class CameraController;
class CheckPoint;
class DamageNumberSpawner;
class mts::Texture;

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
    void Init() override;
    void Awake() override;
    void Start() override;
    void OnDisable() override;
    void OnUpdate(float dt) override;
    void OnFixedUpdate(float dt) override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

    void OnCollisionEnter2D(const mts::Collision2D& collision) override;
    void OnCollisionStay2D(const mts::Collision2D& collision) override;
    void OnCollisionExit2D(const mts::Collision2D& collision) override;

    static float MoveToward(float val_from, float val_to, float delta)
    {
        return glm::abs(val_to - val_from) <= delta
                   ? val_to
                   : val_from + glm::sign(val_to - val_from) * delta;
    }

    void SetCheckPoint(CheckPoint*);
    void ReturnToCheckPoint();
    void TriggerCameraVFX(bool isAction, bool inWindow);
    void TakeDamage(int damage);
    void Heal(int healAmount);
    void OnEnemyHit(float damage, glm::vec3 pos);

    SCRIPT_CLASS(PlayerController)

   private:
    void Dash(glm::vec2 inputDir, bool inWindow, float speedScale);
    void Jump(bool inWindow);
    void Attack(bool inWindow, bool isChargeAttack);
    void SpawnAttackHitbox(float damage,
                           float scaleX,
                           float scaleY,
                           PlayerParticleController::ParticleType type);
    void RefreshHeartVisuals();
    void InitHP();

    glm::vec2 currVelocity{0.0f, 0.0f};
    std::function<void()> queuedDash = nullptr;
    std::function<void()> queuedJump = nullptr;
    std::function<void()> queuedAttackSpawn = nullptr;

    mts::RigidBody2D* rb = nullptr;
    mts::AnimationSystem* anim = nullptr;
    mts::SpriteRenderer* sprite = nullptr;
    mts::Transform* transform = nullptr;

    mts::GameObject* attackHitbox = nullptr;
    AttackHitboxScript* attackHitboxScript = nullptr;
    mts::Transform* attackHitboxTransform = nullptr;
    Direction attackHitboxDirection = RIGHT;

    mts::Texture* defaultTexture = nullptr;
    mts::Texture* altTexture = nullptr;

    PlayerParticleController particleController;

    // Adjustable parameters
    float mMoveSpeed = 5.0f;  // Max Speed
    float mJumpSpeed = 12.0f;
    float mDashSpeed = 12.0f;
    float mFriction = 20.0f;           // Deceleration when not moving
    float mAcceleration = 40.0f;       // Acceleration when holding direction
    float mDeceleration = 15.0f;       // Deceleration when moving above max speed (mMoveSpeed)
    float mDashDeceleration = 100.0f;  // Deceleration when in dash state
    float mTurnSpeed = 100.0f;         // Acceleration when changing directions
    int mDashMax = 1;                  // Max number of dashes before recharge
    float mDashCooldown = 0.2f;        // Length of dash state

    float mAttackDelay = 0.0f;  // Attack delay will delay the spawned attack hitbox to allow it to
                                // line up with animation
    float mAttackCooldown = 0.5f;
    float mOffBeatLightDamage = 1.0f;
    float mOnBeatLightDamage = 3.0f;
    float mOffBeatHeavyDamage = 2.0f;
    float mOnBeatHeavyDamage = 8.0f;
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

    const char* mDefaultTextureResource = "";
    const char* mAltTextureResource = "";

    // State variables
    bool isGrounded = false;
    int dashCount = 1;
    float dashTimer = -1.0f;
    glm::vec2 currentDashDir = {0.0f, 0.0f};
    float attackTimer = -1.0f;
    float attackDelayTimer = -2.0f;
    float attackHeldLength = -1.0f;
    float defaultGravityScale = 1.0f;
    float chargeFlashTimer = -1.0f;
    bool isAttacking = false;
    bool isCharging = false;
    int numBeatsCharged = 1;
    bool chargeComplete = false;
    bool attackAnimType = true;  // keeps track of if previous attack animation was 1st or 2nd (used
                                 // to alternate animations)
    float iFrames = 0.8f;
    float iFramesTimer = -1.0f;
    float enemyKnockback = 0.7f;
    float timeSinceLastBeat = 0.0f;
    Direction facingDir = RIGHT;

    PlayerAudioController* mPlayerAudio = nullptr;

    DamageNumberSpawner* mSpawner = nullptr;

    // CheckPoint Properties
    CheckPoint* activatedCheckPoint = nullptr;

    // Camera VFX properties
    mts::UUID mCameraUUID = mts::UUID::InvalidID;
    mts::GameObject* mCameraGO = nullptr;  // inspector display cache; resolved via UUID
    CameraController* cam = nullptr;
    float mCamZoomSpeed = 0.5f;  // Quick zoom duration

    // HP system
    int mMaxHP = 3;
    int mFullHeartFrame = 67;
    int mEmptyHeartFrame = 39;
    float mHPStartX = 5.0f;
    float mHPStartY = 3.0f;
    float mHPSpacing = 2.2f;
    int mCurrentHP = 3;
    std::vector<mts::GameObject*> mHearts;

    // Conductor event handlers
    void OnBeat(mts::BeatInfo info);
    void OnBar(mts::BeatInfo info);
    mts::Delegate<mts::BeatInfo>::Id mBeatSubId = 0;
    mts::Delegate<mts::BeatInfo>::Id mBarSubId = 0;
};