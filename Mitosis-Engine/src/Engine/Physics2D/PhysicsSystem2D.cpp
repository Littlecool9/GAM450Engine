/**
 * @file PhysicsSystem2D.cpp
 * @author sumin.park
 * @brief  The core physics system in 2D.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "PhysicsSystem2D.h"

#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/RigidBody2D.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/Scene.h"

namespace mts::physics
{
    static std::vector<PhysicsCommand2D> cmdQueue = std::vector<PhysicsCommand2D>();
    static const glm::vec2 gravity = {0, 9.8};

    void PhysicsSystem2D::PushCommand(const PhysicsCommand2D& cmd)
    {
        cmdQueue.push_back(cmd);
    }

    void PhysicsSystem2D::FixedUpdate(Scene* scene, float dt)
    {
        for (const auto& i : cmdQueue)
            ProcessCommand(i);
        cmdQueue.clear();
        IntegrateBodies(scene, dt);
    }

    void PhysicsSystem2D::IntegrateBodies(Scene* scene, float dt)
    {
        auto gameObjects = scene->GetGameObjects();
        for (auto gameObject : gameObjects)
        {
            if (gameObject->IsDead())
                continue;

            auto rb = gameObject->GetComponent<RigidBody2D>();
            if (rb == nullptr)
                continue;

            if (rb->mBodyType == RigidBody2D::BodyType::Static)
            {
                rb->mForceAccumulation = {};
                rb->mTorqueAccumulation = 0.0f;
                continue;
            }

            if (rb->mBodyType == RigidBody2D::BodyType::Kinematic)
            {
                continue;
            }

            // linear body
            Transform* tr = gameObject->GetComponent<Transform>();

            glm::vec2 accel = gravity * rb->mGravityScale;
            if (rb->mInvMass > 0.0f)
                accel += rb->mForceAccumulation * rb->mInvMass;

            rb->mLinearVelocity += accel * dt;
            rb->mLinearVelocity *= 1.0f / (1.0f + rb->mLinearDamping * dt);

            glm::vec2 position = math::ToVec2(tr->GetPosition());
            position += rb->mLinearVelocity * dt;
            tr->SetPosition(math::ToVec3(position, tr->GetPosition().z));

            // angular body
            if (rb->mInvInertia > 0.0f)
            {
                rb->mAngularVelocity += rb->mTorqueAccumulation * rb->mInvInertia * dt;
                rb->mAngularVelocity *= 1.0f / (1.0f + rb->mAngularDamping * dt);

                float rotation = tr->GetRotation();
                rotation += rb->mAngularVelocity * dt;
                tr->SetRotation(rotation);
            }

            rb->mForceAccumulation = {};
            rb->mTorqueAccumulation = 0.0f;
        }
    }

    void PhysicsSystem2D::ProcessCommand(const PhysicsCommand2D& cmd)
    {
        switch (cmd.type)
        {
            case PhysicsCmdType2D::AddForce:
                ApplyAddForce(cmd.rb, cmd.v);
                break;

            case PhysicsCmdType2D::AddImpulse:
                ApplyAddImpulse(cmd.rb, cmd.v);
                break;

            case PhysicsCmdType2D::AddTorque:
                ApplyAddTorque(cmd.rb, cmd.w);
                break;

            case PhysicsCmdType2D::AddAngularImpulse:
                ApplyAddAngularImpulse(cmd.rb, cmd.w);
                break;

            case PhysicsCmdType2D::SetVelocity:
                ApplySetVelocity(cmd.rb, cmd.v);
                break;

            case PhysicsCmdType2D::SetAngularVelocity:
                ApplySetAngularVelocity(cmd.rb, cmd.w);
                break;

            case PhysicsCmdType2D::Teleport:
                ApplyTeleport(cmd.rb, cmd.v, cmd.w);
                break;
        }
    }

    void PhysicsSystem2D::ApplyAddForce(RigidBody2D* rb, const glm::vec2& force)
    {
        if (rb->GetGameObject()->IsDead() || rb->mBodyType != RigidBody2D::BodyType::Dynamic)
            return;

        rb->mForceAccumulation += force;
    }

    void PhysicsSystem2D::ApplyAddImpulse(RigidBody2D* rb, const glm::vec2& impulse)
    {
        if (rb->GetGameObject()->IsDead() || rb->mBodyType != RigidBody2D::BodyType::Dynamic ||
            rb->mInvMass == 0.0f)
            return;

        rb->mLinearVelocity += impulse * rb->mInvMass;
    }

    void PhysicsSystem2D::ApplyAddTorque(RigidBody2D* rb, float torque)
    {
        if (rb->GetGameObject()->IsDead() || rb->mBodyType != RigidBody2D::BodyType::Dynamic)
            return;

        rb->mTorqueAccumulation += torque;
    }

    void PhysicsSystem2D::ApplyAddAngularImpulse(RigidBody2D* rb, float angularImpulse)
    {
        if (rb->GetGameObject()->IsDead() || rb->mBodyType != RigidBody2D::BodyType::Dynamic ||
            rb->mInvInertia == 0.0f)
            return;

        rb->mAngularVelocity += angularImpulse * rb->mInvInertia;
    }

    void PhysicsSystem2D::ApplySetVelocity(RigidBody2D* rb, const glm::vec2& velocity)
    {
        if (rb->GetGameObject()->IsDead() || rb->mBodyType == RigidBody2D::BodyType::Static)
            return;

        rb->mLinearVelocity = velocity;
    }

    void PhysicsSystem2D::ApplySetAngularVelocity(RigidBody2D* rb, float angularVelocity)
    {
        if (rb->GetGameObject()->IsDead() || rb->mBodyType == RigidBody2D::BodyType::Static)
            return;

        rb->mAngularVelocity = angularVelocity;
    }

    void PhysicsSystem2D::ApplyTeleport(RigidBody2D* rb, const glm::vec2 position, float rotation)
    {
        if (rb->GetGameObject()->IsDead())
            return;

        Transform* tr = rb->GetGameObject()->GetComponent<Transform>();
        tr->SetPosition(math::ToVec3(position, tr->GetPosition().z));
        tr->SetRotation(rotation);

        rb->mLinearVelocity = glm::vec2(0.0);
        rb->mAngularVelocity = 0.0f;
        rb->mForceAccumulation = glm::vec2(0.0);
        rb->mTorqueAccumulation = 0.0f;
    }
}  // namespace mts::physics