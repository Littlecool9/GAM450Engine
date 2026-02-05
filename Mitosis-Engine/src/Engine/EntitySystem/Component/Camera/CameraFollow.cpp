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

namespace mts
{
    CameraFollow::CameraFollow()
        : ObjectToFollow(nullptr),
          MaxAccel(1.0f),
          Interpolant(1.0f),
          SpeedLimit(0.0f),
          CameraOffset(0.0f)
    {
    }
    CameraFollow::CameraFollow(GameObject* owner)
        : Component(owner),
          ObjectToFollow(nullptr),
          MaxAccel(1.0f),
          Interpolant(1.0f),
          SpeedLimit(0.0f),
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
    }
    void CameraFollow::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "CameraFollow");

        doc.BeginObject("Data");
        doc.WriteFloat("MaxAcceleration", MaxAccel);
        doc.WriteFloat("Interpolant", Interpolant);

        doc.EndObject();
    }
    void CameraFollow::Init()
    {
    }
    void CameraFollow::Update(float dt)
    {
        FollowTarget(dt);
    }
    void CameraFollow::LateUpdate(float dt)
    {
    }
    void CameraFollow::Render()
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
            return;


        glm::vec2 step = toTarget * (Interpolant * dt);


        float maxStep = SpeedLimit * dt;
        float stepLen = glm::length(step);
        if (stepLen > maxStep)
            step = (step / stepLen) * maxStep;

        // Prevent overshoot
        if (glm::length(step) > dist)
            step = toTarget;

        glm::vec3 oldPos = cur;
        transform->SetPosition(oldPos + glm::vec3(step, 0.0f));  // preserves Z

        // Update SpeedLimit (velocity cap) for next frame
        float moved = glm::length(transform->GetPosition() - oldPos);
        float vel = moved / dt;
        SpeedLimit = vel + (MaxAccel * dt);
    }
}