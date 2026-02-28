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
    struct OBB;
    struct PhysicsBody;
    struct ContactInfo;
    class PhysicsSystem2D
    {
       public:
        static void PushCommand(const PhysicsCommand2D& cmd);
        static void FixedUpdate(Scene* scene, float dt);
        static void Reset();   // clears persistent collision state between play sessions

       private:
        static void ProcessCommandQueue();
        // integrates and caches colliders at the same time;
        static void GatherAndIntegrateBodies(float dt);
        static void CacheColliderData(PhysicsBody& p);

        static void CheckCollision(bool includeTriggers, std::vector<ContactInfo>& outContacts);
        static void ResolveCollision();
        static void RefreshBodyCacheForMoved();

        static void FireEvents();

        static void ProcessCommand(const PhysicsCommand2D& cmd);
        static void ApplyAddForce(RigidBody2D* rb, const glm::vec2& force);
        static void ApplyAddImpulse(RigidBody2D* rb, const glm::vec2& impulse);
        static void ApplyAddTorque(RigidBody2D* rb, float torque);
        static void ApplyAddAngularImpulse(RigidBody2D* rb, float angularImpulse);
        static void ApplySetVelocity(RigidBody2D* rb, const glm::vec2& velocity);
        static void ApplySetAngularVelocity(RigidBody2D* rb, float angularVelocity);
        static void ApplyTeleport(RigidBody2D* rb, const glm::vec2 position, float rotation);

        static bool CircleVsCircle(const PhysicsBody& A,
                                   const PhysicsBody& B,
                                   glm::vec2& outNormalAtoB,
                                   float& outPenetration);

        static bool OBBvsOBB(const PhysicsBody& A,
                             const PhysicsBody& B,
                             glm::vec2& outNormalAtoB,
                             float& outPenetration);

        static bool CircleVsOBB(const PhysicsBody& circle,
                                const PhysicsBody& box,
                                glm::vec2& outNormalAtoB,
                                float& outPenetration);

        static float ProjectAxis(const OBB& obb, const glm::vec2& axisUnit);
        // return true if no separating axis => could be overlapping
        // return false if separating axis => no overlap;
        static bool TestSATOverlap(const OBB& A,
                                   const OBB& B,
                                   const glm::vec2& axisUnit,
                                   float& inOutMinOverlap,
                                   glm::vec2& inOutBestAxis);
    };

}  // namespace mts::physics