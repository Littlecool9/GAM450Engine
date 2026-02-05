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
        void Render() override;
        void SetObjectToFollow(Transform* target);
        void SetMaxAcceleration(float accel);
        void SetInterpolant(float interpolant);
        void FollowTarget(float dt);

    private:
        Transform* ObjectToFollow;
        // Maximum acceleration rate of the camera
        float MaxAccel;
        // Percentage of distance to interpolate over one second
        float Interpolant;

        float SpeedLimit;
        glm::vec3 CameraOffset;
    };

}
