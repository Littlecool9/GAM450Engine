/**
 * @file BoxCollider2DDrawer.cpp
 * @author sumin.park
 * @brief  drawer for boxcollider2d
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "BoxCollider2DDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    bool BoxCollider2DDrawer::Draw(GameObject* go)
    {
        auto* box = go->GetComponent<BoxCollider2D>();
        if (!box)
            return false;

        ImGui::PushID("BoxCollider2DDrawer");

        bool open = ImGui::CollapsingHeader("BoxCollider2D", ImGuiTreeNodeFlags_DefaultOpen);
        bool removed = false;
        if (ImGui::BeginPopupContextItem("##remove_ctx"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                go->RemoveComponent(box);
                removed = true;
            }
            ImGui::EndPopup();
        }
        if (open && !removed)
        {
            glm::vec2 size = box->GetLocalSize();
            float w = size.x;
            float h = size.y;

            if (ImGui::DragFloat("Width", &w, 0.05f, 0.0f, 100000.0f))
                box->SetLocalWidth(w);

            if (ImGui::DragFloat("Height", &h, 0.05f, 0.0f, 100000.0f))
                box->SetLocalHeight(h);

            glm::vec2 off = box->GetOffset();
            if (ImGui::DragFloat2("Offset", &off.x, 0.05f))
                box->SetOffset(off);

            // Debug info (read-only)
            glm::vec2 half = box->GetHalfExtents();
            ImGui::Text("Half Extents: (%.3f, %.3f)", half.x, half.y);

            glm::vec2 center = box->GetCenter();
            ImGui::Text("Center: (%.3f, %.3f)", center.x, center.y);
        }

        ImGui::PopID();
        return true;
    }
}  // namespace mts