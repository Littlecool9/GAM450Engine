/**
 * @file Camera.cpp
 * @author Dong Ta
 * @brief Functionality for camera component
 *        
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */
#include "pch.h"
#include "Camera.h"

#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/Renderer.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    CameraComponent::CameraComponent(): isActive(true), nearPlane(-1.0f), farPlane(1.0f)
    {
    }
    CameraComponent::CameraComponent(GameObject* owner): Component(owner), isActive(true), nearPlane(-1.0f), farPlane(1.0f)
    {
    }

    void CameraComponent::Deserialize(JsonValue const& jsonObj)
    {
        if (jsonObj.Has("IsActive"))
        {
            isActive = jsonObj["IsActive"].GetInt(isActive);
        }
        if (jsonObj.Has("NearPlane"))
        {
            nearPlane = jsonObj["NearPlane"].GetFloat(nearPlane);
        }
        if (jsonObj.Has("FarPlane"))
        {
            farPlane = jsonObj["FarPlane"].GetFloat(farPlane);
        }
    }
    void CameraComponent::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "Camera");
        doc.BeginObject("Data");

        doc.WriteInt("IsActive", isActive);
        doc.WriteFloat("NearPlane", nearPlane);
        doc.WriteFloat("FarPlane", farPlane);

        doc.EndObject();
    }

    void CameraComponent::Init()
    {
    }
    void CameraComponent::Render()
    {
    }
    glm::mat4 CameraComponent::BuildView2D()
    {
        Transform* trans = GetGameObject()->GetComponent<Transform>();
        MTS_ASSERT(trans != nullptr);

        const glm::vec3 pos = trans->GetPosition();
        const float rot = trans->GetRotation();  

        glm::mat4 V(1.0f);
        V = glm::rotate(V, -rot, glm::vec3(0, 0, 1));
        V = glm::translate(V, -pos);
        return V;
    }

    glm::mat4 CameraComponent::BuildProjFromViewport() const
    {
        // TODO: Potentially change when way to get viewport size changes
        glm::ivec4 vp{};
        glGetIntegerv(GL_VIEWPORT, &vp[0]);

        const float w = static_cast<float>(vp.z);
        const float h = static_cast<float>(vp.w);

        return glm::ortho(0.0f, w, h, 0.0f, nearPlane, farPlane);
    }

    void CameraComponent::Update(float dt)
    {
        if (!isActive)
            return;

        RenderCamera cam;
        cam.view = BuildView2D();
        cam.proj = BuildProjFromViewport();
        cam.viewProj = cam.proj * cam.view;
        cam.valid = true;

        Renderer::SetActiveCamera(cam);
    }

}