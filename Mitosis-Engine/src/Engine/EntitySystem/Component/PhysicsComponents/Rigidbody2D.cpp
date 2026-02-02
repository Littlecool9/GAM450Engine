/**
 * @file RigidBody2D.cpp
 * @author sumin.park
 * @brief  Component that stores physics information
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "RigidBody2D.h"

#include "Engine/Physics2D/PhysicsCommand2D.h"
#include "Engine/Physics2D/PhysicsSystem2D.h"

namespace mts
{
    RigidBody2D::~RigidBody2D()
    {
    }

    void RigidBody2D::AddForce(glm::vec2 force)
    {
        auto cmd = physics::PhysicsCommand2D{.type = physics::PhysicsCmdType2D::AddForce,
                                             .rb = this,
                                             .v = force};

        physics::PhysicsSystem2D::PushCommand(cmd);
    }

    void RigidBody2D::AddImpulse(glm::vec2 impulse)
    {
        auto cmd = physics::PhysicsCommand2D{.type = physics::PhysicsCmdType2D::AddImpulse,
                                             .rb = this,
                                             .v = impulse};

        physics::PhysicsSystem2D::PushCommand(cmd);
    }

    void RigidBody2D::AddTorque(float radTorque)
    {
        auto cmd = physics::PhysicsCommand2D{.type = physics::PhysicsCmdType2D::AddTorque,
                                             .rb = this,
                                             .w = radTorque};

        physics::PhysicsSystem2D::PushCommand(cmd);
    }

    void RigidBody2D::AddAngularImpulse(float radImpulse)
    {
        auto cmd = physics::PhysicsCommand2D{.type = physics::PhysicsCmdType2D::AddAngularImpulse,
                                             .rb = this,
                                             .w = radImpulse};

        physics::PhysicsSystem2D::PushCommand(cmd);
    }

    void RigidBody2D::SetVelocity(glm::vec2 velocity)
    {
        auto cmd = physics::PhysicsCommand2D{.type = physics::PhysicsCmdType2D::SetVelocity,
                                             .rb = this,
                                             .v = velocity};

        physics::PhysicsSystem2D::PushCommand(cmd);
    }

    void RigidBody2D::SetAngularVelocity(float radAngularVelocity)
    {
        auto cmd = physics::PhysicsCommand2D{.type = physics::PhysicsCmdType2D::SetAngularVelocity,
                                             .rb = this,
                                             .w = radAngularVelocity};

        physics::PhysicsSystem2D::PushCommand(cmd);
    }

    // TODO: maybe change this to getting a transform
    void RigidBody2D::Teleport(glm::vec2 newPos, float newRot)
    {
        auto cmd = physics::PhysicsCommand2D{.type = physics::PhysicsCmdType2D::Teleport,
                                             .rb = this,
                                             .v = newPos,
                                             .w = newRot};

        physics::PhysicsSystem2D::PushCommand(cmd);
    }

    void RigidBody2D::SetMass(float mass)
    {
        mMass = mass;
        mInvMass = mass ? 1.0f / mass : 0;
    }

    void RigidBody2D::SetInertia(float inertia)
    {
        mInertia = inertia;
        mInvInertia = inertia ? 1.0f / inertia : 0;
    }

    void RigidBody2D::Deserialize(JsonValue const& jsonObj)
    {
        if (jsonObj.Has("Mass"))
            SetMass(jsonObj["Mass"].GetFloat());

        if (jsonObj.Has("Inertia"))
            SetInertia(jsonObj["Inertia"].GetFloat());

        if (jsonObj.Has("LinearVelocity"))
            mLinearVelocity = jsonObj["LinearVelocity"].GetVec2();

        if (jsonObj.Has("AngularVelocity"))
            mAngularVelocity = jsonObj["AngularVelocity"].GetFloat();

        if (jsonObj.Has("LinearDamping"))
            mLinearDamping = jsonObj["LinearDamping"].GetFloat();

        if (jsonObj.Has("AngularDamping"))
            mAngularDamping = jsonObj["AngularDamping"].GetFloat();

        if (jsonObj.Has("GravityScale"))
            mGravityScale = jsonObj["GravityScale"].GetFloat();

        if (jsonObj.Has("BodyType"))
            mBodyType = static_cast<BodyType>(jsonObj["BodyType"].GetInt());
    }

    void RigidBody2D::Serialize(JsonDocument& doc) const
    {
    }
}  // namespace mts