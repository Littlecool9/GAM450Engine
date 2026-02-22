/**
 * @file RigidBody2D.h
 * @author sumin.park
 * @brief  Component that stores physics information
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <glm/common.hpp>

#include "../Component.h"
#include <glm/vec2.hpp>

namespace mts::physics
{
    class PhysicsSystem2D;
}
namespace mts
{
    class RigidBody2D final : public Component
    {
       public:
        enum class BodyType : unsigned char
        {
            Static,    // never moves, but can collide
            Dynamic,   // fully simulated by physics
            Kinematic  // moved by code, but can collide
        };

       public:
        ~RigidBody2D() override;
        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

        void AddForce(glm::vec2 force);
        void AddImpulse(glm::vec2 impulse);
        void AddTorque(float radTorque);
        void AddAngularImpulse(float radImpulse);
        void SetVelocity(glm::vec2 velocity);
        glm::vec2 GetVelocity() const { return mLinearVelocity; }
        void SetAngularVelocity(float radAngularVelocity);
        void Teleport(glm::vec2 newPos, float newRot);

        void SetMass(float mass);
        void SetInertia(float inertia);
        void SetBody(BodyType type) { mBodyType = type; }

       private:
        // invMass = 0 for static objects
        float mMass = 1.0f;
        float mInvMass = 1.0f;

        // rotational mass
        // invInertia = 0 for fixed rotation
        float mInertia = 1.0f;
        float mInvInertia = 1.0f;

        glm::vec2 mLinearVelocity;
        float mAngularVelocity = 0.0f;

        float mLinearDamping = 0.0f;
        float mAngularDamping = 0.0f;

        // for step calculation
        glm::vec2 mForceAccumulation;
        float mTorqueAccumulation = 0.0f;

        // 0 for no gravity
        float mGravityScale = 0.0f;

        BodyType mBodyType = BodyType::Static;

       public:
        friend class physics::PhysicsSystem2D;
    };

}  // namespace mts