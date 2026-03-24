/**
 * @file UITransformDrawer.cpp
 * @author sumin.park
 * @brief  drawer for UITransform Component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "UITransformDrawer.h"
#include "Engine/EntitySystem/Component/UITransform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include <imgui.h>

namespace mts
{
    bool UITransformDrawer::Draw(GameObject* go)
    {
        auto* tr = go->GetComponent<UITransform>();
        if (!tr)
            return false;

        ImGui::PushID("UITransformDrawer");
        bool open = ImGui::CollapsingHeader("UI Transform", ImGuiTreeNodeFlags_DefaultOpen);
        bool removed = false;
        if (ImGui::BeginPopupContextItem("##remove_ctx"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                go->RemoveComponent(tr);
                removed = true;
            }
            ImGui::EndPopup();
        }
        if (open && !removed)
        {
            ImGui::TextDisabled("(screen-space, pixels)");

            glm::vec3 pos = tr->GetPosition();
            if (ImGui::DragFloat3("Position", &pos.x, 0.05f))
                tr->SetPosition(pos);

            glm::vec3 scale = tr->GetScale();
            if (ImGui::DragFloat3("Scale", &scale.x, 0.05f))
                tr->SetScale(scale);

            float rot = tr->GetRotation();
            if (ImGui::DragFloat("Rotation", &rot, 0.05f))
                tr->SetRotation(rot);
        }
        ImGui::PopID();
        return !removed;
    }
}  // namespace mts