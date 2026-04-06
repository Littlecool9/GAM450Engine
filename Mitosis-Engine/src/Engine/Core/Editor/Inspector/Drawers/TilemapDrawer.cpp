/**
 * @file TilemapDrawer.cpp
 * @author sumim.park
 * @brief  drawer for tilemap component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "TilemapDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/Tilemap/Tilemap.h"

namespace mts
{
    bool TilemapDrawer::Draw(GameObject* go)
    {
        auto* tilemap = go->GetComponent<mts::Tilemap::Tilemap>();
        if (!tilemap)
            return false;

        ImGui::PushID("TilemapDrawer");

        bool open = ImGui::CollapsingHeader("Tilemap", ImGuiTreeNodeFlags_DefaultOpen);
        bool removed = false;
        if (ImGui::BeginPopupContextItem("##remove_ctx"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                go->RemoveComponent(tilemap);
                removed = true;
            }
            ImGui::EndPopup();
        }

        ImGui::PopID();
        return true;
    }
}  // namespace mts