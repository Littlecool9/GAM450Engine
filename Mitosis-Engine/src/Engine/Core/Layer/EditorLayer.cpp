/**
 * @file EditorLayer.cpp
 * @author sumin.park
 * @brief  layer for editing ui
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "EditorLayer.h"

#include <imgui.h>

#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"

namespace mts
{
    EditorLayer::EditorLayer(EditorContext ctx) : Layer("EditorLayer"), mCtx(ctx)
    {
    }

    void EditorLayer::OnImGuiRender(float dt)
    {
        if (!mCtx.showEditorUI)
            return;
        DrawDockspace();
        DrawHierarchy();
        DrawInspector();
    }
    void EditorLayer::DrawHierarchy()
    {
        ImGui::Begin("Hierarchy");

        auto* scene = SceneSystem::GetCurrentScene();
        if (!scene)
        {
            ImGui::TextUnformatted("No active scene.");
            ImGui::End();
            return;
        }
        for (GameObject* go : scene->GetGameObjects())
        {
            if (!go)
                continue;

            const UUID id = go->GetId();
            uint64_t combined = id.High() ^ (id.Low() << 1);
            ImGui::PushID((void*) (uintptr_t) combined);

            std::string name = go->GetName();  // keep alive
            if (name.empty())
                name = "Unnamed";

            const bool selected = (mCtx.selected == id);

            if (ImGui::Selectable(name.c_str(), selected))
                mCtx.selected = id;

            ImGui::PopID();
        }

        ImGui::End();
    }

    void EditorLayer::DrawInspector()
    {
        ImGui::Begin("Inspector");

        if (mCtx.selected == UUID::InvalidID)
        {
            ImGui::TextUnformatted("No selection.");
            ImGui::End();
            return;
        }

        auto* go = mts::SceneSystem::GetCurrentScene()->FindGameObjectByUUID(mCtx.selected);
        if (!go)
        {
            ImGui::TextUnformatted("Selection invalid.");
            ImGui::End();
            return;
        }

        // MVP: transform only
        if (auto* tr = go->GetComponent<mts::Transform>())
        {
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
        }

        ImGui::End();
    }

    void EditorLayer::DrawDockspace()
    {
    }
}  // namespace mts