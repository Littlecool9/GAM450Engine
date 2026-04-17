/**
 * @file InspectorRegistry.cpp
 * @author dream
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "InspectorRegistry.h"

#include <imgui.h>
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    void InspectorRegistry::DrawAll(GameObject* go)
    {
        std::string goUuidStr = go->GetId().ToString();
        for (auto& d : mDrawers)
        {
            ImGui::PushID(d->GetName());
            if (d->Draw(go))
            {
                ImGui::SmallButton(":::");
                if (ImGui::BeginDragDropSource())
                {
                    ImGui::SetDragDropPayload("COMPONENT_GO_UUID",
                        goUuidStr.c_str(), goUuidStr.size() + 1);
                    ImGui::Text("%s on %s", d->GetName(), go->GetName().c_str());
                    ImGui::EndDragDropSource();
                }
            }
            ImGui::PopID();
        }
    }
}  // namespace mts