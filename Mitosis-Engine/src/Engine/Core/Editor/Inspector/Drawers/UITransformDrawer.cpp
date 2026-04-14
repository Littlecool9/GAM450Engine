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
            UIAnchor cur = tr->GetAnchor();

            const char* labels[9] = {"TL", "TC", "TR", "ML", "CC", "MR", "BL", "BC", "BR"};
            UIAnchor values[9] = {
                UIAnchor::TopLeft,
                UIAnchor::TopCenter,
                UIAnchor::TopRight,
                UIAnchor::MiddleLeft,
                UIAnchor::Center,
                UIAnchor::MiddleRight,
                UIAnchor::BottomLeft,
                UIAnchor::BottomCenter,
                UIAnchor::BottomRight,
            };

            ImGui::Text("Anchor");
            for (int row = 0; row < 3; row++)
            {
                for (int col = 0; col < 3; col++)
                {
                    int idx = row * 3 + col;
                    bool selected = (cur == values[idx]);
                    if (selected)
                        ImGui::PushStyleColor(ImGuiCol_Button,
                                              ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
                    if (ImGui::Button(labels[idx], ImVec2(28, 22)))
                        tr->SetAnchor(values[idx]);
                    if (selected)
                        ImGui::PopStyleColor();
                    if (col < 2)
                        ImGui::SameLine();
                }
            }

            glm::vec2 off = tr->GetOffset();
            if (ImGui::DragFloat2("Offset (px)", &off.x, 1.0f))
                tr->SetOffset(off);

            float depth = tr->GetDepth();
            if (ImGui::DragFloat("Depth", &depth, 0.01f))
                tr->SetDepth(depth);

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
