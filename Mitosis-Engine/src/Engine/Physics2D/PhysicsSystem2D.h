/**
 * @file PhysicsSystem2D.h
 * @author sumin.park
 * @brief  The core physics system in 2D.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "PhysicsCommand2D.h"
namespace mts
{
    class Scene;
    class RigidBody2D;
}  // namespace mts

namespace mts::physics
{
    class PhysicsSystem2D
    {
       public:
        static void PushCommand(const PhysicsCommand2D& cmd);
        static void FixedUpdate(Scene* scene, float dt);

       private:
        static void IntegrateBodies(Scene* scene, float dt);

        static void ProcessCommand(const PhysicsCommand2D& cmd);
        static void ApplyAddForce(RigidBody2D* rb, const glm::vec2& force);
        static void ApplyAddImpulse(RigidBody2D* rb, const glm::vec2& impulse);
        static void ApplyAddTorque(RigidBody2D* rb, float torque);
        static void ApplyAddAngularImpulse(RigidBody2D* rb, float angularImpulse);
        static void ApplySetVelocity(RigidBody2D* rb, const glm::vec2& velocity);
        static void ApplySetAngularVelocity(RigidBody2D* rb, float angularVelocity);
        static void ApplyTeleport(RigidBody2D* rb, const glm::vec2 position, float rotation);
    };

}  // namespace mts::physics