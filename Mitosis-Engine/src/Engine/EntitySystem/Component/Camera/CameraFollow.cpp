/**
 * @file CameraFollow.cpp
 * @author Dong Ta
 * @brief Functionality for Camera follow component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "CameraFollow.h"

#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/SceneSystem/Scene.h"

namespace mts
{
    CameraFollow::CameraFollow()
        : ObjectToFollow(nullptr),
          MaxAccel(1.0f),
          Interpolant(1.0f),
          Velocity(0.0f),
          CameraOffset(0.0f)
    {
    }
    CameraFollow::CameraFollow(GameObject* owner)
        : Component(owner),
          ObjectToFollow(nullptr),
          MaxAccel(1.0f),
          Interpolant(1.0f),
          Velocity(0.0f),
          CameraOffset(0.0f)
    {
    }

    void CameraFollow::Deserialize(JsonValue const& jsonObj)
    {
        if (jsonObj.Has("MaxAcceleration"))
        {
            MaxAccel = jsonObj["MaxAcceleration"].GetFloat(MaxAccel);
        }
        if (jsonObj.Has("Interpolant"))
        {
            Interpolant = jsonObj["Interpolant"].GetFloat(Interpolant);
        }
        if (jsonObj.Has("CameraOffset"))
        {
            CameraOffset = jsonObj["CameraOffset"].GetVec3();
        }
        if (jsonObj.Has("TargetUUID"))
        {
            mTargetId = UUID::FromString(jsonObj["TargetUUID"].GetString());
        }
    }
    void CameraFollow::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "CameraFollow");

        doc.BeginObject("Data");
        doc.WriteFloat("MaxAcceleration", MaxAccel);
        doc.WriteFloat("Interpolant", Interpolant);
        doc.WriteVec3("CameraOffset", CameraOffset);
        doc.WriteString("TargetUUID", mTargetId.ToString().c_str());

        doc.EndObject();
    }
    void CameraFollow::Init()
    {
        if (mTargetId.IsValid())
        {
            if (Scene* scene = SceneSystem::GetCurrentScene())
            {
                if (GameObject* tgt = scene->FindGameObjectByUUID(mTargetId))
                    ObjectToFollow = tgt->GetComponent<Transform>();
            }
        }
    }
    void CameraFollow::Update(float dt)
    {
        FollowTarget(dt);
    }
    void CameraFollow::LateUpdate(float dt)
    {
    }
    void CameraFollow::SetObjectToFollow(Transform* target)
    {
        ObjectToFollow = target;
    }
    void CameraFollow::SetMaxAcceleration(float accel)
    {
        MaxAccel = accel;
    }
    void CameraFollow::SetInterpolant(float interpolant)
    {
        Interpolant = interpolant;
    }
    void CameraFollow::SetSpeedLimit(float v)
    {
        float len = glm::length(Velocity);
        if (len > 0.0001f)
            Velocity = (Velocity / len) * v;
    }
    void CameraFollow::FollowTarget(float dt)
    {
        if (!ObjectToFollow || dt <= 0.0f)
        {
            return;
        }

        Transform* transform = GetGameObject()->GetComponent<Transform>();
        glm::vec3 cur = transform->GetPosition();
        glm::vec3 target = ObjectToFollow->GetPosition() + CameraOffset;
        glm::vec2 toTarget(target.x - cur.x, target.y - cur.y);
        float dist = glm::length(toTarget);

        if (dist < 0.0001f)
        {
            Velocity = glm::vec2(0.0f);
            return;
        }

        // Desired velocity: exponential-decay style, speed proportional to distance
        glm::vec2 desiredVelocity = toTarget * Interpolant;

        // Compute needed change in velocity (acceleration)
        glm::vec2 accel = desiredVelocity - Velocity;
        float accelMag = glm::length(accel);

        // Clamp acceleration to MaxAccel
        float maxAccelThisFrame = MaxAccel * dt;
        if (accelMag > maxAccelThisFrame && accelMag > 0.0001f)
        {
            accel = (accel / accelMag) * maxAccelThisFrame;
        }

        // Apply acceleration to get new velocity
        Velocity += accel;

        // Compute movement step
        glm::vec2 step = Velocity * dt;

        // Overshoot guard: don't move past the target
        if (glm::length(step) > dist)
        {
            step = toTarget;
            Velocity = toTarget / dt;
        }

        // Apply movement (preserve Z)
        transform->SetPosition(cur + glm::vec3(step, 0.0f));
    }
    Component* CameraFollow::Clone(GameObject* newOwner) const
    {
        CameraFollow* c = new CameraFollow(*this);
        c->InitFromClone(newOwner);
        c->ObjectToFollow = ObjectToFollow;
        c->Velocity = glm::vec2(0.0f);
        return c;
    }
}  // namespace mts