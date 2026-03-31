/**
 * @file ScenesPanel.cpp
 * @author sumin.park
 * @brief  panel for managing the runtime scene registry
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "ScenesPanel.h"

#include <imgui.h>
#include <string_view>
#include "Engine/SceneSystem/SceneSystem.h"

namespace mts
{
    void ScenesPanel::Draw(EditorContext& /*ctx*/)
    {
        ImGui::Begin("Scenes");

        ImGui::TextUnformatted("Scene build list (index used by SceneSystem::SwitchScene(n))");
        ImGui::Separator();

        const auto& scenes = SceneSystem::GetRegisteredScenes();

        // --- Registered scene list ---
        int removeIndex = -1;
        if (scenes.empty())
        {
            ImGui::Spacing();
            ImGui::TextDisabled("No scenes registered.");
            ImGui::Spacing();
        }
        else
        {
            for (int i = 0; i < static_cast<int>(scenes.size()); ++i)
            {
                ImGui::PushID(i);
                ImGui::Text("%d", i);
                ImGui::SameLine();
                ImGui::TextUnformatted(scenes[i].c_str());
                ImGui::SameLine();
                if (ImGui::SmallButton("X"))
                    removeIndex = i;
                ImGui::PopID();
            }
            ImGui::Spacing();
        }

        if (removeIndex >= 0)
        {
            SceneSystem::UnregisterScene(removeIndex);
            SceneSystem::SaveRegistry(mRegistryPath);
        }

        ImGui::Separator();

        // --- Visible drag-drop zone ---
        // Button turns green on hover (while dragging) to confirm drop target
        float zoneW = ImGui::GetContentRegionAvail().x;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.14f, 0.14f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.45f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.14f, 0.14f, 0.14f, 1.0f));
        ImGui::Button("  Drop [S] scene from Assets panel here  ", ImVec2(zoneW, 36.0f));
        ImGui::PopStyleColor(3);

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
            {
                std::string_view droppedPath(static_cast<const char*>(payload->Data),
                                             payload->DataSize - 1);
                if (droppedPath.size() >= 6 &&
                    droppedPath.substr(droppedPath.size() - 6) == ".scene")
                {
                    SceneSystem::RegisterScene(std::string(droppedPath));
                    SceneSystem::SaveRegistry(mRegistryPath);
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::End();
    }

}  // namespace mts
