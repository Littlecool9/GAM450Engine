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
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/CircleCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/RigidBody2D.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/UUID/UUIDPairKey.h"

namespace mts::physics
{
    struct ContactInfo
    {
        RigidBody2D* a;
        RigidBody2D* b;
        glm::vec2 contactNormal;
        float penetration;
        bool isAnyTrigger;
    };

    static std::vector<PhysicsCommand2D> cmdQueue = std::vector<PhysicsCommand2D>();
    static std::vector<RigidBody2D*> rigidBodies = std::vector<RigidBody2D*>();
    static std::vector<ContactInfo> contactInfos = std::vector<ContactInfo>();
    static std::unordered_set<UUIDPairKey> prevPairs = std::unordered_set<UUIDPairKey>();
    static std::unordered_set<UUIDPairKey> currPairs = std::unordered_set<UUIDPairKey>();
    static Scene* currentScene = nullptr;

    static const glm::vec2 gravity = {0, -9.8};
    static const int collisionResolveNum = 4;

    void PhysicsSystem2D::PushCommand(const PhysicsCommand2D& cmd)
    {
        cmdQueue.push_back(cmd);
    }

    void PhysicsSystem2D::FixedUpdate(Scene* scene, float dt)
    {
        currentScene = scene;
        ProcessCommandQueue();
        IntegrateBodies(dt);

        for (int i = 0; i < collisionResolveNum; i++)
        {
            contactInfos.clear();
            CheckCollision(false);
            ResolveCollision();
        }
        contactInfos.clear();
        CheckCollision(true);

        FireEvents();

        rigidBodies.clear();
    }

    void PhysicsSystem2D::ProcessCommandQueue()
    {
        for (const auto& i : cmdQueue)
            ProcessCommand(i);

        cmdQueue.clear();
    }

    void PhysicsSystem2D::IntegrateBodies(float dt)
    {
        auto gameObjects = currentScene->GetGameObjects();

        for (auto gameObject : gameObjects)
        {
            if (gameObject->IsDead())
                continue;

            auto rb = gameObject->GetComponent<RigidBody2D>();
            if (rb == nullptr)
                continue;

            rigidBodies.push_back(rb);

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
            tr->SetPosition(math::ToVec3(position));

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

    void PhysicsSystem2D::CheckCollision(bool includeTriggers)
    {
        for (size_t i = 0; i < rigidBodies.size(); i++)
        {
            auto rbA = rigidBodies[i];
            auto colA = rbA->GetGameObject()->GetComponent<Collider2D>();
            if (colA == nullptr)
                continue;
            if (!includeTriggers && colA->IsTrigger())
                continue;

            for (size_t j = i + 1; j < rigidBodies.size(); j++)
            {
                auto rbB = rigidBodies[j];
                auto colB = rbB->GetGameObject()->GetComponent<Collider2D>();
                if (colB == nullptr)
                    continue;
                if (!includeTriggers && colB->IsTrigger())
                    continue;

                if (rbA->mBodyType == RigidBody2D::BodyType::Static &&
                    rbB->mBodyType == RigidBody2D::BodyType::Static)
                    continue;

                ContactInfo c{};
                c.a = rbA;
                c.b = rbB;
                c.isAnyTrigger = colA->IsTrigger() || colB->IsTrigger();

                bool hit = false;
                if (colA->GetColliderType() == Collider2D::ColliderType::Circle &&
                    colB->GetColliderType() == Collider2D::ColliderType::Circle)
                {
                    hit = CircleVsCircle(static_cast<CircleCollider2D*>(colA),
                                         static_cast<CircleCollider2D*>(colB),
                                         c.contactNormal,
                                         c.penetration);
                }
                else if (colA->GetColliderType() == Collider2D::ColliderType::Box &&
                         colB->GetColliderType() == Collider2D::ColliderType::Box)
                {
                    hit = OBBvsOBB(static_cast<BoxCollider2D*>(colA),
                                   static_cast<BoxCollider2D*>(colB),
                                   c.contactNormal,
                                   c.penetration);
                }
                else if (colA->GetColliderType() == Collider2D::ColliderType::Circle &&
                         colB->GetColliderType() == Collider2D::ColliderType::Box)
                {
                    hit = CircleVsOBB(static_cast<CircleCollider2D*>(colA),
                                      static_cast<BoxCollider2D*>(colB),
                                      c.contactNormal,
                                      c.penetration);
                }
                else if (colA->GetColliderType() == Collider2D::ColliderType::Box &&
                         colB->GetColliderType() == Collider2D::ColliderType::Circle)
                {
                    hit = CircleVsOBB(static_cast<CircleCollider2D*>(colB),
                                      static_cast<BoxCollider2D*>(colA),
                                      c.contactNormal,
                                      c.penetration);
                    c.contactNormal *= -1;
                }

                if (!hit)
                    continue;

                contactInfos.push_back(c);
            }
        }
    }

    void PhysicsSystem2D::ResolveCollision()
    {
        const float penetrationSlop = 0.01f;   // allowed penetration
        const float correctionPercent = 0.8f;  // aggressiveness of correction
        for (auto& c : contactInfos)
        {
            if (c.isAnyTrigger)
                continue;
            float invMassSum = c.a->mInvMass + c.b->mInvMass;
            if (invMassSum <= 0)
                continue;

            Transform* trA = c.a->GetGameObject()->GetComponent<Transform>();
            Transform* trB = c.b->GetGameObject()->GetComponent<Transform>();

            float correctionMag =
                std::max(c.penetration - penetrationSlop, 0.0f) * correctionPercent;
            if (correctionMag > 0.0f)
            {
                glm::vec2 correction = (correctionMag / invMassSum) * c.contactNormal;

                glm::vec2 posA = trA->GetPosition();
                glm::vec2 posB = trB->GetPosition();

                posA -= correction * c.a->mInvMass;
                posB += correction * c.b->mInvMass;

                if (c.a->mBodyType == RigidBody2D::BodyType::Dynamic)
                    trA->SetPosition(posA);
                if (c.b->mBodyType == RigidBody2D::BodyType::Dynamic)
                    trB->SetPosition(posB);
            }

            // Relative velocity from A -> B
            const glm::vec2 rv = c.b->mLinearVelocity - c.a->mLinearVelocity;
            const float velAlongNormal = glm::dot(rv, c.contactNormal);

            // If velAlongNormal > 0, bodies are separating along the normal already
            if (velAlongNormal > 0.0f)
                continue;

            // canceling inward normal component
            // if we want to add bounce, bounce impulse goes here
            const float j = (-velAlongNormal) / invMassSum;
            const glm::vec2 impulse = j * c.contactNormal;

            if (c.a->mBodyType == RigidBody2D::BodyType::Dynamic)
                c.a->mLinearVelocity -= impulse * c.a->mInvMass;
            if (c.b->mBodyType == RigidBody2D::BodyType::Dynamic)
                c.b->mLinearVelocity += impulse * c.b->mInvMass;
        }
    }

    void PhysicsSystem2D::FireEvents()
    {
        // save collision pairs
        currPairs.clear();
        currPairs.reserve(contactInfos.size());
        for (auto& c : contactInfos)
        {
            const UUID& idA = c.a->GetGameObject()->GetId();
            const UUID& idB = c.b->GetGameObject()->GetId();

            currPairs.insert(UUIDPairKey(idA, idB));
        }

        for (const UUIDPairKey& k : currPairs)
        {
            bool wasColliding = prevPairs.find(k) != prevPairs.end();
            GameObject* gH = currentScene->FindGameObjectUUID(k.hi);
            GameObject* gL = currentScene->FindGameObjectUUID(k.lo);

            // dead objects shouldn't be on currPairs in the first place
            // todo : find bug later
            // MTS_ASSERT(gH && gL && !gH->IsDead() && !gL->IsDead());

            if (gH && gL && !gH->IsDead() && !gL->IsDead())
            {
                if (!wasColliding)
                {
                    gH->OnCollisionEnter2D(gL);
                    gL->OnCollisionEnter2D(gH);
                }
                else
                {
                    gH->OnCollisionStay2D(gL);
                    gL->OnCollisionStay2D(gH);
                }
            }
        }

        for (const UUIDPairKey& k : prevPairs)
        {
            if (currPairs.find(k) != currPairs.end())
                continue;

            GameObject* gH = currentScene->FindGameObjectUUID(k.hi);
            GameObject* gL = currentScene->FindGameObjectUUID(k.lo);

            if (!gH || !gL)
                continue;

            gH->OnCollisionExit2D(gL);
            gL->OnCollisionExit2D(gH);
        }

        prevPairs = currPairs;
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
        tr->SetPosition(math::ToVec3(position));
        tr->SetRotation(rotation);

        rb->mLinearVelocity = glm::vec2(0.0);
        rb->mAngularVelocity = 0.0f;
        rb->mForceAccumulation = glm::vec2(0.0);
        rb->mTorqueAccumulation = 0.0f;
    }

    // compare center distance against combined radius.
    bool PhysicsSystem2D::CircleVsCircle(const CircleCollider2D* a,
                                         const CircleCollider2D* b,
                                         glm::vec2& outNormalAtoB,
                                         float& outPenetration)
    {
        glm::vec2 ca = a->GetCenter();
        glm::vec2 cb = b->GetCenter();

        float ra = a->GetRadius();
        float rb = b->GetRadius();

        glm::vec2 d = cb - ca;
        float distSqr = glm::dot(d, d);
        float r = ra + rb;
        float rr = r * r;

        if (distSqr >= rr)
            return false;

        // complete overlap
        if (distSqr < 1e-12f)
        {
            outNormalAtoB = glm::vec2(1.0f, 0.0f);
            outPenetration = r;
            return true;
        }

        float dist = std::sqrt(distSqr);
        outNormalAtoB = d / dist;
        outPenetration = r - dist;

        return true;
    }

    // test a set of box axis
    // if all overlap = collision
    bool PhysicsSystem2D::OBBvsOBB(const BoxCollider2D* a,
                                   const BoxCollider2D* b,
                                   glm::vec2& outNormalAtoB,
                                   float& outPenetration)
    {
        float minOverlap = FLT_MAX;
        glm::vec2 bestAxis(0.0f, 0.0f);

        if (!TestSATOverlap(a, b, a->GetUp(), minOverlap, bestAxis))
            return false;
        if (!TestSATOverlap(a, b, a->GetRight(), minOverlap, bestAxis))
            return false;
        if (!TestSATOverlap(a, b, b->GetUp(), minOverlap, bestAxis))
            return false;
        if (!TestSATOverlap(a, b, b->GetRight(), minOverlap, bestAxis))
            return false;

        // ensure the chosen axis points from A -> B
        glm::vec2 dir = b->GetCenter() - a->GetCenter();
        if (glm::dot(dir, bestAxis) < 0.0f)
            bestAxis = -bestAxis;

        outNormalAtoB = bestAxis;
        outPenetration = minOverlap;

        return true;
    }

    // get circle center in the box's local coordinate
    // clamp that local point to the box extents to get the closest point on box.
    // if center is inside the box = collision
    // choose the closest box face direction for contact normal
    bool PhysicsSystem2D::CircleVsOBB(const CircleCollider2D* a,
                                      const BoxCollider2D* b,
                                      glm::vec2& outNormalAtoB,
                                      float& outPenetration)
    {
        const glm::vec2 cA = a->GetCenter();
        const float rA = a->GetRadius();

        const glm::vec2 cB = b->GetCenter();
        const glm::vec2 rightB = b->GetRight();
        const glm::vec2 upB = b->GetUp();
        const glm::vec2 hB = b->GetHalfExtents();

        // Circle center relative to box center
        const glm::vec2 d = cA - cB;

        // Circle center in box local coordinates
        const float localX = glm::dot(d, rightB);
        const float localY = glm::dot(d, upB);

        // Closest point on box to circle center (in box local coords)
        const float clampedX = std::clamp(localX, -hB.x, hB.x);
        const float clampedY = std::clamp(localY, -hB.y, hB.y);

        // Closest point in world space
        const glm::vec2 closest = cB + rightB * clampedX + upB * clampedY;

        // Vector from circle center to closest point on box
        glm::vec2 v = closest - cA;
        const float distSqr = glm::dot(v, v);

        // Normal outside-case
        if (distSqr > 1e-12f)
        {
            const float dist = std::sqrt(distSqr);

            if (dist >= rA)
                return false;

            outNormalAtoB = v / dist;
            outPenetration = rA - dist;
            return true;
        }

        // Inside-box case: circle center is inside (or exactly on) the box.
        // Choose the nearest face and push outward.
        const float dx = hB.x - std::abs(localX);
        const float dy = hB.y - std::abs(localY);

        if (dx < dy)
        {
            const float sign = (localX >= 0.0f) ? 1.0f : -1.0f;
            outNormalAtoB = rightB * sign;
            outPenetration = rA + dx;
        }
        else
        {
            const float sign = (localY >= 0.0f) ? 1.0f : -1.0f;
            outNormalAtoB = upB * sign;
            outPenetration = rA + dy;
        }

        return true;
    }

    float PhysicsSystem2D::ProjectOBBOnAxis(const BoxCollider2D* col, const glm::vec2& axis)
    {
        glm::vec2 half = col->GetHalfExtents();
        float ax = glm::abs(glm::dot(col->GetRight(), axis)) * half.x;
        float ay = glm::abs(glm::dot(col->GetUp(), axis)) * half.y;
        return ax + ay;
    }

    bool PhysicsSystem2D::TestSATOverlap(const BoxCollider2D* a,
                                         const BoxCollider2D* b,
                                         const glm::vec2& axisCandidate,
                                         float& inOutMinOverlap,
                                         glm::vec2& inOutBestAxis)
    {
        float len = glm::length(axisCandidate);
        MTS_ASSERT(len > 1e-12f && "axis not valid");

        glm::vec2 n = glm::normalize(axisCandidate);
        glm::vec2 d = b->GetCenter() - a->GetCenter();
        float dist = glm::abs(glm::dot(d, n));

        float pA = ProjectOBBOnAxis(a, n);
        float pB = ProjectOBBOnAxis(b, n);

        // separating axis found
        float overlap = (pA + pB) - dist;
        if (overlap <= 0.0f)
            return false;

        if (overlap < inOutMinOverlap)
        {
            inOutMinOverlap = overlap;
            inOutBestAxis = n;
        }
        return true;
    }

}  // namespace mts::physics