/**
 * @file CameraFollow.h
 * @author Dong Ta
 * @brief Component to attach to camera in conjunction with camera component to allow
 *        the component to follow its designated target
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Engine/EntitySystem/Component/Component.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/UUID/UUID.h"

namespace mts
{
    class CameraFollow : public Component
    {
       public:
        CameraFollow();
        CameraFollow(GameObject* owner);
        ~CameraFollow() override = default;

        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;
        void Init() override;
        void Update(float dt) override;
        void LateUpdate(float dt) override;
        void SetObjectToFollow(Transform* target);
        void SetMaxAcceleration(float accel);
        void SetInterpolant(float interpolant);
        void FollowTarget(float dt);
        Component* Clone(GameObject* newOwner) const override;

        UUID      GetTargetId()         const { return mTargetId; }
        void      SetTargetId(UUID id)        { mTargetId = id; }
        float     GetMaxAcceleration()  const { return MaxAccel; }
        float     GetInterpolant()      const { return Interpolant; }
        float     GetSpeedLimit()       const { return SpeedLimit; }
        glm::vec3 GetCameraOffset()     const { return CameraOffset; }
        void      SetSpeedLimit(float v)       { SpeedLimit = v; }
        void      SetCameraOffset(glm::vec3 v) { CameraOffset = v; }

       private:
        Transform* ObjectToFollow;
        UUID mTargetId = UUID::GenerateNil();
        // Maximum acceleration rate of the camera
        float MaxAccel;
        // Percentage of distance to interpolate over one second
        float Interpolant;

        float SpeedLimit;
        glm::vec3 CameraOffset;
    };

}  // namespace mts
