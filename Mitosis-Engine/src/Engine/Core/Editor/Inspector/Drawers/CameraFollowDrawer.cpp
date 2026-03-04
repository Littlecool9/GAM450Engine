/**
 * @file CameraFollowDrawer.h
 * @author sumin.park
 * @brief drawer for CameraFollow component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "CameraFollowDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/Component/Camera/CameraFollow.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"
#include "Engine/UUID/UUID.h"

namespace mts
{
    bool CameraFollowDrawer::Draw(GameObject* go)
    {
        auto* cf = go->GetComponent<CameraFollow>();
        if (!cf)
            return false;

        ImGui::PushID("CameraFollowDrawer");

        bool open = ImGui::CollapsingHeader("CameraFollow", ImGuiTreeNodeFlags_DefaultOpen);
        bool removed = false;
        if (ImGui::BeginPopupContextItem("##remove_ctx"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                go->RemoveComponent(cf);
                removed = true;
            }
            ImGui::EndPopup();
        }
        if (open && !removed)
        {
            Scene* scene = SceneSystem::GetCurrentScene();

            // --- GO reference slot ---
            std::string targetName = "None";
            if (cf->GetTargetId().IsValid() && scene)
            {
                if (GameObject* tgt = scene->FindGameObjectByUUID(cf->GetTargetId()))
                    targetName = tgt->GetName().empty() ? "Unnamed" : tgt->GetName();
            }

            ImGui::Text("Target:");
            ImGui::SameLine();
            if (ImGui::Button(targetName.c_str()))
                ImGui::OpenPopup("##goref_pick");

            // Accept drag-drop from hierarchy
            if (ImGui::BeginDragDropTarget())
            {
                if (auto* p = ImGui::AcceptDragDropPayload("GAMEOBJECT_UUID"))
                {
                    UUID id = UUID::FromString(static_cast<const char*>(p->Data));
                    cf->SetTargetId(id);
                    if (scene)
                    {
                        if (GameObject* tgt = scene->FindGameObjectByUUID(id))
                            cf->SetObjectToFollow(tgt->GetComponent<Transform>());
                    }
                }
                ImGui::EndDragDropTarget();
            }

            // Popup picker
            if (ImGui::BeginPopup("##goref_pick") && scene)
            {
                int i = 0;
                for (GameObject* c : scene->GetGameObjects())
                {
                    if (!c || c->IsDead())
                        continue;
                    std::string label = c->GetName().empty() ? "Unnamed" : c->GetName();
                    label += "##";
                    label += std::to_string(i++);
                    if (ImGui::Selectable(label.c_str()))
                    {
                        cf->SetTargetId(c->GetId());
                        cf->SetObjectToFollow(c->GetComponent<Transform>());
                    }
                }
                ImGui::EndPopup();
            }

            // --- Numeric fields ---
            float maxAccel = cf->GetMaxAcceleration();
            if (ImGui::DragFloat("Max Acceleration", &maxAccel, 0.05f))
                cf->SetMaxAcceleration(maxAccel);

            float interp = cf->GetInterpolant();
            if (ImGui::DragFloat("Interpolant", &interp, 0.05f))
                cf->SetInterpolant(interp);

            float speedLimit = cf->GetSpeedLimit();
            if (ImGui::DragFloat("Speed Limit", &speedLimit, 0.05f))
                cf->SetSpeedLimit(speedLimit);

            glm::vec3 offset = cf->GetCameraOffset();
            if (ImGui::DragFloat3("Camera Offset", &offset.x, 0.05f))
                cf->SetCameraOffset(offset);
        }

        ImGui::PopID();
        return true;
    }
}  // namespace mts
