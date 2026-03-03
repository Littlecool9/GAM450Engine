/**
 * @file CameraController.cpp
 * @author Jackey Cen
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "CameraController.h"

#include "Engine/Renderer.h"
#include "Engine/EntitySystem/Component/Camera/Camera.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#define MAIN_CAMERA_NAME "CameraObject"

void CameraController::OnCreate()
{
    GetAndInitCamera();
    cachedCameraComponent = nullptr;  // Reset cache
}

void CameraController::OnLateUpdate(float dt)
{
    if (!mainCamera)
    {
        mainCamera = const_cast<mts::RenderCamera*>(&mts::Renderer::GetActiveCamera());
        if (!mainCamera)
            return;
    }

    // Modify mainCamera's parameters
    HandleZoom(dt);   // Update mainCamera with new zoom level if needed
    HandleShake(dt);  // Update mainCamera position with shake offset if shaking

    // Update the active camera in the renderer to ensure changes take effect
    LOG_INFO("MainCamera Projection matrix after update: [{:.2f}, {:.2f}, {:.2f}, {:.2f}]",
             mainCamera->proj[0][0],
             mainCamera->proj[1][1],
             mainCamera->proj[2][2],
             mainCamera->proj[3][3]);
    mts::Renderer::SetActiveCamera(*mainCamera);
}


void CameraController::TriggerZoom(float zoomLevel, float speed)
{
    targetZoom = zoomLevel;
    zoomSpeed = speed;
}

void CameraController::TriggerShake(float duration, float intensity)
{
    shakeDuration = duration;
    shakeIntensity = intensity;
}

void CameraController::HandleZoom(float dt)
{
    // Use epsilon comparison instead of exact equality
    if (std::abs(currentZoom - targetZoom) > ZOOM_EPSILON)
    {
        float zoomDifference = targetZoom - currentZoom;
        float zoomStep = zoomSpeed * dt;

        if (std::abs(zoomDifference) <= zoomStep)
        {
            currentZoom = targetZoom;  // Snap to target if close enough
        }
        else
        {
            currentZoom += (zoomDifference > 0 ? zoomStep : -zoomStep);
        }

        // Update dimensions for both snap and gradual zoom
        currentHeight = originalHeight * currentZoom;
        currentWidth = originalWidth * currentZoom;

        // Cache the camera component to avoid expensive lookup every frame
        if (!cachedCameraComponent)
        {
            mts::GameObject* cameraObj =
                mts::SceneSystem::GetCurrentScene()->FindGameObjectByName(MAIN_CAMERA_NAME);
            if (!cameraObj)
            {
                LOG_ERROR(
                    "CameraController: Failed to find MainCamera GameObject. Zoom will not "
                    "work.");
                return;
            }

            cachedCameraComponent = cameraObj->GetComponent<mts::CameraComponent>();
            if (!cachedCameraComponent)
            {
                LOG_ERROR(
                    "CameraController: Failed to get CameraComponent from MainCamera. Zoom "
                    "will not work.");
                return;
            }
        }
    }
    // Update the camera's projection matrix
    if (cachedCameraComponent)
    {

        mainCamera->proj = glm::ortho(-currentWidth / 2.0f,
                                        currentWidth / 2.0f,
                                        -currentHeight / 2.0f,
                                        currentHeight / 2.0f,
                                        cachedCameraComponent->nearPlane,
                                        cachedCameraComponent->farPlane);
        mainCamera->viewProj =
            mainCamera->proj * mainCamera->view;  // Update view-projection matrix
    }
}

void CameraController::HandleShake(float dt)
{
    if (shakeDuration > 0.0f)
    {
        shakeDuration -= dt;
        float offsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * shakeIntensity;
        float offsetY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * shakeIntensity;
        
        // Apply shake offset to position
        mainCamera->position += glm::vec3(offsetX, offsetY, 0.0f);
        shakeOffset += glm::vec3(offsetX, offsetY, 0.0f);  // Track total offset
        
        // CRITICAL: Update view matrix after position change
        glm::mat4 V(1.0f);
        V = glm::translate(V, -mainCamera->position);  // Simple 2D view: inverse of position
        mainCamera->view = V;
        
        // CRITICAL: Recalculate viewProj
        mainCamera->viewProj = mainCamera->proj * mainCamera->view;
    }
    else if (shakeOffset != glm::vec3(0.0f))  // Only reset ONCE when shake ends
    {
        mainCamera->position -= shakeOffset;  // Remove accumulated shake offset
        
        // Update view matrix for reset position
        glm::mat4 V(1.0f);
        V = glm::translate(V, -mainCamera->position);
        mainCamera->view = V;
        mainCamera->viewProj = mainCamera->proj * mainCamera->view;
        
        shakeOffset = glm::vec3(0.0f);  // Clear offset so this doesn't run again
    }
}

bool CameraController::GetAndInitCamera()
{
    if (!mainCamera)
    {
        mainCamera = const_cast<mts::RenderCamera*>(&mts::Renderer::GetActiveCamera());
        if (!mainCamera)
            return false;
    }

    glm::ivec4 vp{};
    glGetIntegerv(GL_VIEWPORT, &vp[0]);

    const float w = static_cast<float>(vp.z);
    const float h = static_cast<float>(vp.w);

    originalWidth = currentWidth = w / mts::PixelsPerUnit;
    originalHeight = currentHeight = h / mts::PixelsPerUnit;

    return true;  
}
