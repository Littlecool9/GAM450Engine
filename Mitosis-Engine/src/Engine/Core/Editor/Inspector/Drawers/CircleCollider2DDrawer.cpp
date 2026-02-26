/**
 * @file CircleCollider2DDrawer.cpp
 * @author sumin.park
 * @brief  drawer for circlecollider2d
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "CircleCollider2DDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/Component/PhysicsComponents/CircleCollider2D.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    void CircleCollider2DDrawer::Draw(GameObject* go)
    {
        auto* c = go->GetComponent<CircleCollider2D>();
        if (!c)
            return;

        ImGui::PushID("CircleCollider2DDrawer");

        if (ImGui::CollapsingHeader("CircleCollider2D", ImGuiTreeNodeFlags_DefaultOpen))
        {
            float r = c->GetLocalRadius();
            if (ImGui::DragFloat("Radius", &r, 0.05f, 0.0f, 100000.0f))
                c->SetLocalRadius(r);

            glm::vec2 off = c->GetOffset();
            if (ImGui::DragFloat2("Offset", &off.x, 0.05f))
                c->SetOffset(off);

            glm::vec2 center = c->GetCenter();
            ImGui::Text("Center: (%.3f, %.3f)", center.x, center.y);
        }

        ImGui::PopID();
    }

}  // namespace mts