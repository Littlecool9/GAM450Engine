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
    class CircleCollider2D;
    class BoxCollider2D;
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
        static void ProcessCommandQueue();
        // integrates and caches colliders at the same time;
        static void IntegrateBodies(float dt);
        static void CheckCollision(bool includeTriggers);
        static void ResolveCollision();
        static void FireEvents();

        static void ProcessCommand(const PhysicsCommand2D& cmd);
        static void ApplyAddForce(RigidBody2D* rb, const glm::vec2& force);
        static void ApplyAddImpulse(RigidBody2D* rb, const glm::vec2& impulse);
        static void ApplyAddTorque(RigidBody2D* rb, float torque);
        static void ApplyAddAngularImpulse(RigidBody2D* rb, float angularImpulse);
        static void ApplySetVelocity(RigidBody2D* rb, const glm::vec2& velocity);
        static void ApplySetAngularVelocity(RigidBody2D* rb, float angularVelocity);
        static void ApplyTeleport(RigidBody2D* rb, const glm::vec2 position, float rotation);

        static bool CircleVsCircle(const CircleCollider2D* a,
                                   const CircleCollider2D* b,
                                   glm::vec2& outNormalAtoB,
                                   float& outPenetration);

        static bool OBBvsOBB(const BoxCollider2D* a,
                             const BoxCollider2D* b,
                             glm::vec2& outNormalAtoB,
                             float& outPenetration);

        static bool CircleVsOBB(const CircleCollider2D* a,
                                const BoxCollider2D* b,
                                glm::vec2& outNormalAtoB,
                                float& outPenetration);

        static float ProjectOBBOnAxis(const BoxCollider2D* col, const glm::vec2& axis);

        // return true if no separating axis => could be overlapping
        // return false if separating axis => no overlap;
        static bool TestSATOverlap(const BoxCollider2D* a,
                                   const BoxCollider2D* b,
                                   const glm::vec2& axisCandidate,
                                   float& inOutMinOverlap,
                                   glm::vec2& inOutBestAxis);
    };

}  // namespace mts::physics