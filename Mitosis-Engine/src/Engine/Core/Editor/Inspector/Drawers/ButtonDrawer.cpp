/**
 * @file ButtonDrawer.cpp
 * @author Dong Ta
 * @brief  Inspector drawer for the Button component
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "ButtonDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/Component/Button.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    bool ButtonDrawer::Draw(GameObject* go)
    {
        auto* btn = go->GetComponent<Button>();
        if (!btn)
            return false;

        ImGui::PushID("ButtonDrawer");

        bool open = ImGui::CollapsingHeader("Button", ImGuiTreeNodeFlags_DefaultOpen);
        bool removed = false;
        if (ImGui::BeginPopupContextItem("##remove_ctx"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                go->RemoveComponent(btn);
                removed = true;
            }
            ImGui::EndPopup();
        }

        if (open && !removed)
        {
            ImGui::TextDisabled("Hit area derives from transform scale (half-extents).");

            // Read-only state display for debugging
            const char* stateLabels[] = {"Idle", "Hovered", "Pressed"};
            int stateIdx = static_cast<int>(btn->GetState());
            ImGui::BeginDisabled(true);
            ImGui::Combo("State", &stateIdx, stateLabels, IM_ARRAYSIZE(stateLabels));
            ImGui::EndDisabled();
        }

        ImGui::PopID();
        return !removed;
    }

}  // namespace mts
