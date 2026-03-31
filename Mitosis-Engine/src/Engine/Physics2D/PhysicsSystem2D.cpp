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
#include <unordered_set>
#include "Collision2D.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/CircleCollider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Collider2D.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/RigidBody2D.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/Scene.h"

namespace mts::physics
{
#pragma region data structures
    struct AABB
    {
        glm::vec2 min;
        glm::vec2 max;
    };

    inline bool AABBOverlap(const AABB& a, const AABB& b)
    {
        return (a.min.x <= b.max.x && a.max.x >= b.min.x && a.min.y <= b.max.y &&
                a.max.y >= b.min.y);
    }

    struct OBB
    {
        glm::vec2 c;  // center
        glm::vec2 r;  // right (unit)
        glm::vec2 u;  // up (unit)
        glm::vec2 h;  // half extents
    };

    struct PhysicsBody
    {
        RigidBody2D* rb = nullptr;
        GameObject* go = nullptr;
        Transform* tr = nullptr;
        Collider2D* col = nullptr;

        Collider2D::ColliderType type{};
        bool isTrigger = false;
        bool isDynamic = false;

        AABB aabb{};
        OBB obb{};
        glm::vec2 circleC{};
        float circleR = 0.0f;
    };

    struct Pair
    {
        uint32_t i, j;
    };

    inline uint64_t PackPair(uint32_t i, uint32_t j)
    {
        if (i > j)
            std::swap(i, j);
        return (uint64_t(i) << 32) | uint64_t(j);
    }

    struct ContactInfo
    {
        uint32_t i, j;
        glm::vec2 normal;  // A->B
        float penetration;
        glm::vec2 contactPoint;  // world-space contact point
        bool isTrigger;
    };

    constexpr float cellSize = 10.0f;

    struct GOPairKey
    {
        GameObject* a;
        GameObject* b;
    };

    inline bool operator==(const GOPairKey& x, const GOPairKey& y)
    {
        return x.a == y.a && x.b == y.b;
    }

    struct GOPairHash
    {
        size_t operator()(const GOPairKey& k) const noexcept
        {
            // pointer hashing
            size_t ha = std::hash<void*>{}(k.a);
            size_t hb = std::hash<void*>{}(k.b);
            return ha ^ (hb + 0x9e3779b97f4a7c15ull + (ha << 6) + (ha >> 2));
        }
    };

    inline GOPairKey MakeOrderedPair(GameObject* a, GameObject* b)
    {
        if (a > b)
            std::swap(a, b);  // consistent ordering
        return {a, b};
    }
#pragma endregion

    static std::vector<PhysicsCommand2D> cmdQueue = std::vector<PhysicsCommand2D>();
    static std::vector<PhysicsBody> physicsBodies = std::vector<PhysicsBody>();

    static std::vector<ContactInfo> contactInfos = std::vector<ContactInfo>();
    static std::unordered_map<GOPairKey, ContactInfo, GOPairHash> prevContacts;
    static std::unordered_map<GOPairKey, ContactInfo, GOPairHash> currContacts;

    static Scene* currentScene = nullptr;

    static const glm::vec2 gravity = {0, -9.8};
    static const int collisionResolveNum = 4;

    void PhysicsSystem2D::Reset()
    {
        cmdQueue.clear();
        physicsBodies.clear();
        contactInfos.clear();
        prevContacts.clear();
        currContacts.clear();
        currentScene = nullptr;
    }

    void PhysicsSystem2D::PushCommand(const PhysicsCommand2D& cmd)
    {
        cmdQueue.push_back(cmd);
    }

    void PhysicsSystem2D::FixedUpdate(Scene* scene, float dt)
    {
        currentScene = scene;

        ProcessCommandQueue();

        // Gather and integrate
        physicsBodies.clear();
        physicsBodies.reserve(scene->GetGameObjects().size());
        GatherAndIntegrateBodies(dt);

        for (int i = 0; i < collisionResolveNum; i++)
        {
            contactInfos.clear();
            CheckCollision(false, contactInfos);
            ResolveCollision();
            RefreshBodyCacheForMoved();
        }
        contactInfos.clear();
        CheckCollision(true, contactInfos);
        FireEvents();
    }

    void PhysicsSystem2D::ProcessCommandQueue()
    {
        for (const auto& i : cmdQueue)
            ProcessCommand(i);

        cmdQueue.clear();
    }

    void PhysicsSystem2D::GatherAndIntegrateBodies(float dt)
    {
        const auto& gameObjects = currentScene->GetGameObjects();

        for (auto gameObject : gameObjects)
        {
            if (gameObject->IsDead() || !gameObject->IsActive())
                continue;

            auto rb = gameObject->GetComponent<RigidBody2D>();
            auto col = gameObject->GetComponent<Collider2D>();
            auto tr = gameObject->GetComponent<Transform>();

            if (!rb || !tr)
                continue;
            PhysicsBody p{};
            p.rb = rb;
            p.col = col;
            p.tr = tr;
            p.go = gameObject;

            p.isDynamic = (rb->mBodyType == RigidBody2D::BodyType::Dynamic);

            if (col)
            {
                p.type = col->GetColliderType();
                p.isTrigger = col->IsTrigger();
            }
            if (p.isDynamic)
            {
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

            if (p.col)
                CacheColliderData(p);

            physicsBodies.push_back(p);
        }
    }

    void PhysicsSystem2D::CacheColliderData(PhysicsBody& p)
    {
        if (p.type == Collider2D::ColliderType::Circle)
        {
            auto* c = static_cast<CircleCollider2D*>(p.col);
            p.circleC = c->GetCenter();
            p.circleR = c->GetRadius();

            glm::vec2 e(p.circleR);
            p.aabb.min = p.circleC - e;
            p.aabb.max = p.circleC + e;
            return;
        }

        if (p.type == Collider2D::ColliderType::Box)
        {
            auto* b = static_cast<BoxCollider2D*>(p.col);

            p.obb.c = b->GetCenter();
            p.obb.r = b->GetRight();
            p.obb.u = b->GetUp();
            p.obb.h = b->GetHalfExtents();

            glm::vec2 aabbHalf =
                math::AbsVec2(p.obb.r) * p.obb.h.x + math::AbsVec2(p.obb.u) * p.obb.h.y;
            p.aabb.min = p.obb.c - aabbHalf;
            p.aabb.max = p.obb.c + aabbHalf;

            MTS_ASSERT(std::abs(glm::length(p.obb.r) - 1.0f) < 1e-3f);
            MTS_ASSERT(std::abs(glm::length(p.obb.u) - 1.0f) < 1e-3f);
            return;
        }
    }

    void PhysicsSystem2D::CheckCollision(bool includeTriggers,
                                         std::vector<ContactInfo>& outContacts)
    {
        for (int i = 0; i < physicsBodies.size(); ++i)
            for (int j = i + 1; j < physicsBodies.size(); ++j)
            {
                const PhysicsBody& A = physicsBodies[i];
                const PhysicsBody& B = physicsBodies[j];

                if (!A.col || !B.col)
                    continue;

                // Two static bodies can never produce a useful collision response — skip early.
                if (!A.isDynamic && !B.isDynamic)
                    continue;

                if (!includeTriggers && (A.isTrigger || B.isTrigger))
                    continue;

                if (!AABBOverlap(A.aabb, B.aabb))
                    continue;

                ContactInfo c{};
                c.i = i;
                c.j = j;
                c.isTrigger = A.isTrigger || B.isTrigger;

                bool hit = false;
                if (A.type == Collider2D::ColliderType::Circle &&
                    B.type == Collider2D::ColliderType::Circle)
                {
                    hit = CircleVsCircle(A, B, c.normal, c.penetration, c.contactPoint);
                }
                else if (A.type == Collider2D::ColliderType::Box &&
                         B.type == Collider2D::ColliderType::Box)
                {
                    hit = OBBvsOBB(A, B, c.normal, c.penetration, c.contactPoint);
                }
                else if (A.type == Collider2D::ColliderType::Circle &&
                         B.type == Collider2D::ColliderType::Box)
                {
                    hit = CircleVsOBB(A, B, c.normal, c.penetration, c.contactPoint);
                }
                else if (A.type == Collider2D::ColliderType::Box &&
                         B.type == Collider2D::ColliderType::Circle)
                {
                    hit = CircleVsOBB(B, A, c.normal, c.penetration, c.contactPoint);
                    c.normal *= -1.0f;
                }

                if (hit)
                    outContacts.push_back(c);
            }
    }

    void PhysicsSystem2D::ResolveCollision()
    {
        const float penetrationSlop = 0.01f;   // allowed penetration
        const float correctionPercent = 0.8f;  // aggressiveness of correction

        for (auto& c : contactInfos)
        {
            if (c.isTrigger)
                continue;

            PhysicsBody& A = physicsBodies[c.i];
            PhysicsBody& B = physicsBodies[c.j];

            RigidBody2D* rbA = A.rb;
            RigidBody2D* rbB = B.rb;
            Transform* trA = A.tr;
            Transform* trB = B.tr;

            if (!rbA || !rbB || !trA || !trB)
                continue;

            float invMassSum = rbA->mInvMass + rbB->mInvMass;
            if (invMassSum <= 0)
                continue;

            const float correctionMag =
                std::max(c.penetration - penetrationSlop, 0.0f) * correctionPercent;

            if (correctionMag > 0.0f)
            {
                const glm::vec2 correction = (correctionMag / invMassSum) * c.normal;

                glm::vec2 posA = math::ToVec2(trA->GetPosition());
                glm::vec2 posB = math::ToVec2(trB->GetPosition());

                if (rbA->mBodyType == RigidBody2D::BodyType::Dynamic)
                    posA -= correction * rbA->mInvMass;
                if (rbB->mBodyType == RigidBody2D::BodyType::Dynamic)
                    posB += correction * rbB->mInvMass;

                trA->SetPosition(math::ToVec3(posA));
                trB->SetPosition(math::ToVec3(posB));
            }

            // Relative velocity from A -> B
            const glm::vec2 rv = rbB->mLinearVelocity - rbA->mLinearVelocity;
            const float velAlongNormal = glm::dot(rv, c.normal);

            // If velAlongNormal > 0, bodies are separating along the normal already
            if (velAlongNormal > 0.0f)
                continue;

            // canceling inward normal component
            // if we want to add bounce, bounce impulse goes here
            const float j = (-velAlongNormal) / invMassSum;
            const glm::vec2 impulse = j * c.normal;

            if (rbA->mBodyType == RigidBody2D::BodyType::Dynamic)
                rbA->mLinearVelocity -= impulse * rbA->mInvMass;
            if (rbB->mBodyType == RigidBody2D::BodyType::Dynamic)
                rbB->mLinearVelocity += impulse * rbB->mInvMass;
        }
    }

    void PhysicsSystem2D::RefreshBodyCacheForMoved()
    {
        for (auto& b : physicsBodies)
        {
            if (!b.isDynamic)
                continue;
            if (!b.col)
                continue;
            CacheColliderData(b);
        }
    }

    void PhysicsSystem2D::FireEvents()
    {
        // Build the set of GOs alive this frame. Any GO not in this set has been
        // deleted and its pointer is dangling — must not be touched.
        std::unordered_set<GameObject*> liveGOs;
        liveGOs.reserve(physicsBodies.size());
        for (const auto& body : physicsBodies)
            liveGOs.insert(body.go);

        // Purge stale prevContacts entries so the exit-event loop never
        // dereferences a freed pointer.
        for (auto it = prevContacts.begin(); it != prevContacts.end();)
        {
            if (!liveGOs.count(it->first.a) || !liveGOs.count(it->first.b))
                it = prevContacts.erase(it);
            else
                ++it;
        }

        currContacts.clear();

        for (const ContactInfo& c : contactInfos)
        {
            GameObject* goA = physicsBodies[c.i].go;
            GameObject* goB = physicsBodies[c.j].go;

            if (!goA || !goB)
                continue;
            if (goA->IsDead() || goB->IsDead())
                continue;

            // change i & j and normal so contact.i always belongs to key.a.
            bool swapped = (goA > goB);
            GOPairKey key = MakeOrderedPair(goA, goB);

            ContactInfo stored = c;
            if (swapped)
            {
                std::swap(stored.i, stored.j);
                stored.normal = -c.normal;
            }
            currContacts.try_emplace(key, stored);
        }

        for (const auto& [key, contact] : currContacts)
        {
            GameObject* a = key.a;
            GameObject* b = key.b;

            if (!a || !b)
                continue;
            if (a->IsDead() || b->IsDead())
                continue;

            bool wasColliding = (prevContacts.find(key) != prevContacts.end());

            Collider2D* colA = physicsBodies[contact.i].col;
            Collider2D* colB = physicsBodies[contact.j].col;

            mts::Collision2D collForA{colB,
                                      colA,
                                      contact.normal,
                                      contact.contactPoint,
                                      contact.penetration};
            mts::Collision2D collForB{colA,
                                      colB,
                                      -contact.normal,
                                      contact.contactPoint,
                                      contact.penetration};

            if (!wasColliding)
            {
                a->OnCollisionEnter2D(collForA);
                b->OnCollisionEnter2D(collForB);
            }
            else
            {
                a->OnCollisionStay2D(collForA);
                b->OnCollisionStay2D(collForB);
            }
        }

        // Exit for pairs that disappeared
        for (const auto& [key, contact] : prevContacts)
        {
            if (currContacts.find(key) != currContacts.end())
                continue;

            GameObject* a = key.a;
            GameObject* b = key.b;

            if (!a || !b)
                continue;
            if (a->IsDead() || b->IsDead())
                continue;

            Collider2D* colA = physicsBodies[contact.i].col;
            Collider2D* colB = physicsBodies[contact.j].col;

            mts::Collision2D collForA{colB, colA, -contact.normal, contact.contactPoint, 0.0f};
            mts::Collision2D collForB{colA, colB, contact.normal, contact.contactPoint, 0.0f};

            a->OnCollisionExit2D(collForA);
            b->OnCollisionExit2D(collForB);
        }

        prevContacts.swap(currContacts);
        currContacts.clear();
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
    bool PhysicsSystem2D::CircleVsCircle(const PhysicsBody& A,
                                         const PhysicsBody& B,
                                         glm::vec2& outNormalAtoB,
                                         float& outPenetration,
                                         glm::vec2& outContactPoint)
    {
        glm::vec2 ca = A.circleC;
        glm::vec2 cb = B.circleC;

        float ra = A.circleR;
        float rb = B.circleR;

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
            outContactPoint = ca + outNormalAtoB * ra;
            return true;
        }

        float dist = std::sqrt(distSqr);
        outNormalAtoB = d / dist;
        outPenetration = r - dist;
        // midpoint between the two surface contact points
        outContactPoint = (ca + outNormalAtoB * ra + cb - outNormalAtoB * rb) * 0.5f;

        return true;
    }

    // test a set of box axis
    // if all overlap = collision
    bool PhysicsSystem2D::OBBvsOBB(const PhysicsBody& A,
                                   const PhysicsBody& B,
                                   glm::vec2& outNormalAtoB,
                                   float& outPenetration,
                                   glm::vec2& outContactPoint)
    {
        const OBB& a = A.obb;
        const OBB& b = B.obb;

        float minOverlap = FLT_MAX;
        glm::vec2 bestAxis(0.0f);

        // These MUST be unit axes. Ensure GetRight/GetUp produce normalized axes.
        if (!TestSATOverlap(a, b, a.u, minOverlap, bestAxis))
            return false;
        if (!TestSATOverlap(a, b, a.r, minOverlap, bestAxis))
            return false;
        if (!TestSATOverlap(a, b, b.u, minOverlap, bestAxis))
            return false;
        if (!TestSATOverlap(a, b, b.r, minOverlap, bestAxis))
            return false;

        // Point bestAxis from A->B
        if (glm::dot(b.c - a.c, bestAxis) < 0.0f)
            bestAxis = -bestAxis;

        outNormalAtoB = bestAxis;
        outPenetration = minOverlap;
        // contact point: point on A's surface toward B, backed off by half penetration
        float projA = ProjectAxis(a, bestAxis);
        outContactPoint = a.c + bestAxis * (projA - outPenetration * 0.5f);
        return true;
    }

    // get circle center in the box's local coordinate
    // clamp that local point to the box extents to get the closest point on box.
    // if center is inside the box = collision
    // choose the closest box face direction for contact normal
    bool PhysicsSystem2D::CircleVsOBB(const PhysicsBody& circle,
                                      const PhysicsBody& box,
                                      glm::vec2& outNormalAtoB,
                                      float& outPenetration,
                                      glm::vec2& outContactPoint)
    {
        const glm::vec2 cA = circle.circleC;
        const float rA = circle.circleR;

        const OBB& B = box.obb;

        // Circle center relative to box center
        const glm::vec2 d = cA - B.c;

        // Circle center in box local coordinates
        const float localX = glm::dot(d, B.r);
        const float localY = glm::dot(d, B.u);

        // Closest point on box to circle center (in box local coords)
        const float clampedX = std::clamp(localX, -B.h.x, B.h.x);
        const float clampedY = std::clamp(localY, -B.h.y, B.h.y);

        // Closest point in world space
        const glm::vec2 closest = B.c + B.r * clampedX + B.u * clampedY;

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
            // midpoint between circle surface point and closest point on box
            outContactPoint = (cA + outNormalAtoB * rA + closest) * 0.5f;
            return true;
        }

        // Inside-box case: circle center is inside (or exactly on) the box.
        // Choose the nearest face and push outward.
        const float dx = B.h.x - std::abs(localX);
        const float dy = B.h.y - std::abs(localY);

        if (dx < dy)
        {
            const float sign = (localX >= 0.0f) ? 1.0f : -1.0f;
            outNormalAtoB = B.r * sign;
            outPenetration = rA + dx;
        }
        else
        {
            const float sign = (localY >= 0.0f) ? 1.0f : -1.0f;
            outNormalAtoB = B.u * sign;
            outPenetration = rA + dy;
        }
        // contact point on circle surface toward box
        outContactPoint = cA + outNormalAtoB * rA;

        return true;
    }

    float PhysicsSystem2D::ProjectAxis(const OBB& obb, const glm::vec2& axisUnit)
    {
        // axisUnit must be unit
        const float ax = std::abs(glm::dot(obb.r, axisUnit)) * obb.h.x;
        const float ay = std::abs(glm::dot(obb.u, axisUnit)) * obb.h.y;
        return ax + ay;
    }

    bool PhysicsSystem2D::TestSATOverlap(const OBB& A,
                                         const OBB& B,
                                         const glm::vec2& axisUnit,
                                         float& inOutMinOverlap,
                                         glm::vec2& inOutBestAxis)
    {
        const glm::vec2 d = B.c - A.c;
        const float dist = std::abs(glm::dot(d, axisUnit));

        const float ra = ProjectAxis(A, axisUnit);
        const float rb = ProjectAxis(B, axisUnit);

        const float overlap = (ra + rb) - dist;
        if (overlap <= 0.0f)
            return false;

        if (overlap < inOutMinOverlap)
        {
            inOutMinOverlap = overlap;
            inOutBestAxis = axisUnit;
        }
        return true;
    }

}  // namespace mts::physics