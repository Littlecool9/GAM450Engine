/**
 * @file CameraController.h
 * @author Jackey Cen
 * @brief  script for camera behavior control, including zoom and shake effects, as well as optional
 * target following.
 *
 * @copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include "Engine/Scripting/ScriptBase.h"
#include "Engine/UUID/UUID.h"
#include <queue>

namespace mts
{
    class RenderCamera;
    class CameraComponent;
    class ConductorSystem;
}  // namespace mts

class CameraController : public mts::ScriptBase
{
   public:
    void Init() override;
    void Start() override;
    void OnLateUpdate(float dt) override;

    void DeserializeFields(mts::JsonValue const& fields) override;
    void SerializeFields(mts::JsonDocument& out) const override;
    void GetInspectorFields(std::vector<mts::ScriptField>& fields) override;

    void TriggerZoom(float zoomLevel, float speed);
    void TriggerShake(float duration, float intensity);

    SCRIPT_CLASS(CameraController);
    bool GetAndInitCamera(mts::ConductorSystem&);

   private:
    // Define Zoom Process: When the zoom is triggered, we set a target zoom level and a zoom
    // speed. Each frame, we smoothly interpolate the current zoom level towards the target zoom
    // level using the zoom speed. This creates a smooth zooming effect over time.
    void HandleZoom(float dt);

    // Define Shake Process: When a shake is triggered, we set the shake duration and intensity.
    // Each frame, if the shake duration is greater than zero, we apply a random offset to the
    // camera's
    void HandleShake(float dt);

   private:
    mts::RenderCamera* mainCamera = nullptr;

    // Viewport parameters
    float originalWidth = 0.0f;   // Store the original width of the camera's view
    float originalHeight = 0.0f;  // Store the original height of the camera's view
    float currentWidth = 0.0f;    // Current width of the camera's view (for zooming)
    float currentHeight = 0.0f;   // Current height of the camera's view (for zooming)

    // Zoom parameters
    float zoomSpeed = 1.0f;                         // Speed of zooming
    float targetZoom = 1.0f;                        // Desired zoom level
    float currentZoom = 1.0f;                       // Current zoom level
    const float originalZoom = 1.0f;                // Original zoom level (for anchoring)
    bool isZooming = false;                         // Flag to indicate if zooming is in progress
    std::queue<std::pair<float, float>> zoomQueue;  // Queue to handle multiple zoom requests

    // Shake parameters
    float shakeDuration = 0.0f;   // Remaining duration of the shake effect
    float shakeIntensity = 0.0f;  // Intensity of the shake effect
    glm::vec3 shakeOffset{0.0f};  // Keep track of the accumulated shake offset to reset the
                                  // camera position correctly
    bool isShaking = false;       // Flag to indicate if shaking is in progress

    mts::CameraComponent* cachedCameraComponent = nullptr;
    static constexpr float ZOOM_EPSILON = 0.001f;

    // Follow target
    mts::UUID mFollowTargetUUID = mts::UUID::InvalidID;
    mts::GameObject* mFollowTargetGO = nullptr;  // inspector display cache; resolved via UUID
};
