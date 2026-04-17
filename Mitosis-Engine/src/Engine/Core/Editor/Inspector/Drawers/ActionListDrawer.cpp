/**
 * @file ActionListDrawer.cpp
 * @author sumin.park
 * @brief drawer for ActionList component
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "ActionListDrawer.h"

#include <imgui.h>

#include "Engine/EntitySystem/Component/ActionList.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"

namespace mts
{
    bool ActionListDrawer::Draw(GameObject* go)
    {
        auto* al = go->GetComponent<ActionList>();
        if (!al)
            return false;

        ImGui::PushID("ActionListDrawer");

        bool open = ImGui::CollapsingHeader("ActionList", ImGuiTreeNodeFlags_DefaultOpen);
        bool removed = false;
        if (ImGui::BeginPopupContextItem("##remove_ctx"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                go->RemoveComponent(al);
                removed = true;
            }
            ImGui::EndPopup();
        }
        if (open && !removed)
        {
            ImGui::LabelText("Action Count", "%d", al->GetActionCount());
            ImGui::LabelText("Speed Modifier", "%.2f", al->GetSpeedModifier());

            bool paused = al->IsPaused();
            if (ImGui::Checkbox("Paused", &paused))
                al->SetPaused(paused);

            float slowFactor = al->GetSlowFactor();
            if (ImGui::DragFloat("Slow Factor", &slowFactor, 0.01f, 0.01f, 1.0f))
                al->SetSlowFactor(slowFactor);

            float ffSpeed = al->GetFastForwardSpeed();
            if (ImGui::DragFloat("Fast Forward Speed", &ffSpeed, 0.1f, 1.0f, 20.0f))
                al->SetFastForwardSpeed(ffSpeed);

            ImGui::Separator();

            if (ImGui::Button("Slow Down"))
                al->SlowDown();
            ImGui::SameLine();
            if (ImGui::Button("Fast Forward"))
                al->FastForward();
            ImGui::SameLine();
            if (ImGui::Button("Reset Speed"))
                al->ResetSpeed();

            if (ImGui::Button("Clear Actions"))
                al->Clear();
        }

        ImGui::PopID();
        return true;
    }
}  // namespace mts
