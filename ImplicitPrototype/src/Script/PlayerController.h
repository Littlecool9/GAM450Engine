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
#include "Engine/Conductor/ConductorSystem.h"

#include <glm/common.hpp>

class CameraController;
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

    static float MoveToward(float val_from, float val_to, float delta)
    {
        return glm::abs(val_to - val_from) <= delta
                   ? val_to
                   : val_from + glm::sign(val_to - val_from) * delta;
    }

    void Test();
    void Init(mts::ConductorSystem& system, CameraController* cam);
    void SetCheckPoint(CheckPoint*);
    void ReturnToCheckPoint();
    void TriggerCameraVFX(bool isAction, bool inWindow);

    SCRIPT_CLASS(PlayerController)

   private:
    void Dash(glm::vec2 inputDir, bool inWindow);
    void Jump(bool inWindow);

    glm::vec2 currVelocity{0.0f, 0.0f};
    std::function<void()> queuedDash = nullptr;
    std::function<void()> queuedJump = nullptr;

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
    int dashMax = 1;
    float dashCooldown = 0.2f;
    // beat boosts are multipliers to the strength of actions
    // these are applied if the player hits the action on beat
    float dashBeatBoost = 1.0f;
    float jumpBeatBoost = 1.0f;

    float hitWindowEarlyEdge = 0.1f;
    float hitWindowLateEdge = 0.1f;

    bool isGrounded = false;
    int dashCount = 1;
    float dashTimer = -1.0f;

    // rhythm lock means the player can only jump/dash if hitting on beat
    bool dashRhythmLock = true;
    bool jumpRhythmLock = true;

    Direction facingDir = RIGHT;

    // CheckPoint Properties
    CheckPoint* actvitedCheckPoint = nullptr;

    // Camera VFX properties
    CameraController* cam = nullptr;
    float camZoomSpeed = 0.5f;  // Quick zoom duration

    // Conductor event handlers
    void OnBeat(mts::BeatInfo info);
    void OnBar(mts::BeatInfo info);
    mts::Delegate<mts::BeatInfo>::Id mBeatSubId = 0;
    mts::Delegate<mts::BeatInfo>::Id mBarSubId = 0;

};