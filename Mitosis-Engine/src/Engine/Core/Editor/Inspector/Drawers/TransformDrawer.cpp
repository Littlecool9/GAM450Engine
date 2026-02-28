/**
 * @file TransformDrawer.cpp
 * @author sumin.park
 * @brief  drawer for transform
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "TransformDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    bool TransformDrawer::Draw(GameObject* go)
    {
        auto* tr = go->GetComponent<Transform>();
        if (!tr)
            return false;

        // Stable ID scope for this component UI
        ImGui::PushID("TransformDrawer");

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
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
        return true;
    }
}  // namespace mts