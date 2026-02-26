/**
 * @file HierarchyPanel.cpp
 * @author sumin.park
 * @brief hierarchy ui
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "HierarchyPanel.h"

#include <imgui.h>

#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/SceneSystem/SceneSystem.h"

namespace mts
{
    HierarchyPanel::HierarchyPanel()
    {
    }

    void HierarchyPanel::Draw(Scene* scene, EditorContext& ctx)
    {
        ImGui::Begin("Hierarchy");

        if (!scene)
        {
            ImGui::TextUnformatted("No active scene.");
            ImGui::End();
            return;
        }
        if (ImGui::Button("+ Add GameObject"))
            GameObjectFactory::CreateGameObject(scene);
        ImGui::Separator();

        for (GameObject* go : scene->GetGameObjects())
        {
            if (!go || go->IsDead())
                continue;

            const UUID id = go->GetId();
            uint64_t combined = id.High() ^ (id.Low() << 1);
            ImGui::PushID((void*) (uintptr_t) combined);

            std::string name = go->GetName();  // keep alive
            if (name.empty())
                name = "Unnamed";

            const bool selected = (ctx.selected == id);

            if (ImGui::Selectable(name.c_str(), selected))
                ctx.selected = id;

            if (ImGui::BeginPopupContextItem("go_context"))
            {
                if (ImGui::MenuItem("Delete"))
                {
                    go->Destroy();
                    if (ctx.selected == id)
                        ctx.selected = UUID::InvalidID;
                }
                ImGui::EndPopup();
            }

            ImGui::PopID();
        }

        ImGui::End();
    }
}  // namespace mts