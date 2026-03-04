/**
 * @file CameraDrawer.cpp
 * @author sumin.park
 * @brief drawer for CameraComponent
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "CameraDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/Component/Camera/Camera.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    bool CameraDrawer::Draw(GameObject* go)
    {
        auto* cam = go->GetComponent<CameraComponent>();
        if (!cam)
            return false;

        ImGui::PushID("CameraDrawer");

        bool open = ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen);
        bool removed = false;
        if (ImGui::BeginPopupContextItem("##remove_ctx"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                go->RemoveComponent(cam);
                removed = true;
            }
            ImGui::EndPopup();
        }
        if (open && !removed)
        {
            ImGui::Checkbox("Active", &cam->isActive);
            ImGui::DragFloat("Near Plane", &cam->nearPlane, 0.01f);
            ImGui::DragFloat("Far Plane", &cam->farPlane, 0.1f);
        }

        ImGui::PopID();
        return true;
    }
}  // namespace mts
