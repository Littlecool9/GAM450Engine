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
#include "Engine/Serialization/SerializationUtility.h"

namespace mts
{
    HierarchyPanel::HierarchyPanel()
    {
    }

    void HierarchyPanel::Draw(Scene* scene, EditorContext& ctx)
    {
        ImGui::Begin("Hierarchy");
        ImGui::SetWindowFontScale(mFontScale);

        if (!scene)
        {
            ImGui::TextUnformatted("No active scene.");
            ImGui::End();
            return;
        }
        if (ImGui::Button("+ Add GameObject"))
            GameObjectFactory::CreateGameObject(scene);
        ImGui::Separator();

        ImGui::BeginChild("##go_list", ImVec2(0, 0), false);
        for (GameObject* go : scene->GetGameObjects())
        {
            if (!go || go->IsDead())
                continue;

            const UUID id = go->GetId();
            uint64_t combined = id.High() ^ (id.Low() << 1);
            ImGui::PushID((void*) (uintptr_t) combined);

            const std::string& rawName = go->GetName();
            const char* name = rawName.empty() ? "Unnamed" : rawName.c_str();

            const bool selected = (ctx.selected == id);

            if (ImGui::Selectable(name, selected))
                ctx.selected = id;

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
            {
                std::string uuidStr = go->GetId().ToString();
                ImGui::SetDragDropPayload("GAMEOBJECT_UUID", uuidStr.c_str(), uuidStr.size() + 1);
                ImGui::Text("GO: %s", name);
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginPopupContextItem("go_context"))
            {
                if (ImGui::MenuItem("Delete"))
                {
                    go->Destroy();
                    if (ctx.selected == id)
                        ctx.selected = UUID::InvalidID;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Save as Prefab"))
                {
                    std::string savePath = "../TechDemo/Data/" + go->GetName() + ".prefab";
                    if (SerializationUtility::SerializeToFile(go, savePath))
                        LOG_INFO("Prefab saved: {}", savePath);
                    else
                        LOG_WARN("Failed to save prefab '{}'", go->GetName());
                }
                ImGui::EndPopup();
            }

            ImGui::PopID();
        }
        ImGui::EndChild();

        if (ImGui::BeginDragDropTarget())
        {
            if (auto* payload = ImGui::AcceptDragDropPayload("PREFAB_PATH"))
            {
                const char* path = static_cast<const char*>(payload->Data);
                if (GameObject* spawned = GameObjectFactory::InstantiateFromJSON(path))
                    ctx.selected = spawned->GetId();
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::End();
    }
}  // namespace mts